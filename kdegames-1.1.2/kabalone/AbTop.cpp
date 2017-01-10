/* Class AbTop */

#include <qpopmenu.h>
#include <qkeycode.h>
#include <qtimer.h>
#include <kapp.h>
#include <kiconloader.h>
#include <ktopwidget.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kmsgbox.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include "AbTop.h"
#include "Board.h"
#include "BoardWidget.h"
#include "version.h"

AbTop::AbTop()
{
  timerState = noGame;

  moveNo = 0;
  actValue = 0;
  stop = false;

  setupMenu();
  setupStatusBar();
  setupToolBar();

  timer = new QTimer;
  connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );

  board = new Board();

  connect( board, SIGNAL(searchBreak()), this, SLOT(searchBreak()) );

  CHECK_PTR(board);
  boardWidget = new BoardWidget(*board,this);  

#ifdef SPION
  spy = new Spy(*board);
#endif

  connect( boardWidget, SIGNAL(updateSpy(QString)),
	   this, SLOT(updateSpy(QString)) );

  connect( board, SIGNAL(updateBestMove(Move&,int)),
	   this, SLOT(updateBestMove(Move&,int)) );

  connect( boardWidget, SIGNAL(moveChoosen(Move&)), 
	   this, SLOT(moveChoosen(Move&)) );

  /* default */
  easy();
  play_red();
  showMoveLong = true;
  showSpy = false;
  
  setView(boardWidget);
  boardWidget->show();

  setMinimumSize(150,200);

  updateStatus();
  updateToolBar();
}

AbTop::~AbTop()
{
#ifdef SPION
  delete spy;
#endif
	
  delete toolbar;
}

/* Read config options
 *
 * menu must already be created!
 */
void AbTop::readConfig()
{
  KConfig* config = kapp->getConfig();
  config->setGroup("Options");
	
  readOptions(config);

  config->setGroup("Appearance");
  int x=215, y=280;
  QString entry;
  if (entry = config->readEntry("Geometry"))
	  sscanf(entry,"%dx%d",&x,&y);
  resize(x,y);
  config->setGroup("Rating");
  board->readRating(config);
}

void AbTop::readOptions(KConfig* config)
{	
  QString entry;
  if (entry = config->readEntry("Level")) {
    if (entry == "Easy")  easy();
    else if (entry == "Normal") normal();
    else if (entry == "Hard") hard();
    else if (entry == "Challange") challange();
  }

  if (entry = config->readEntry("Computer")) {
    if (entry == "Red") play_red();
    else if (entry == "Yellow") play_yellow();
    else if (entry == "Both") play_both();
  }

  if (entry = config->readEntry("MoveSlow")) 
    showMoveLong = (entry == "Yes");

  _options->setItemChecked(slow_id, showMoveLong);

  /* We set <showSpy> to inverted value & toggle afterwards */
  showSpy = false;
  if (entry = config->readEntry("ShowSpy")) 
    showSpy = (entry == "No");

  toggleSpy();
}

void AbTop::readProperties(KConfig *config)
{
  QString entry;
	
  readOptions(config);
  board->readRating(config);
	
  if (entry = config->readEntry("TimerState"))
    timerState = entry.toInt();
  if (timerState == noGame) return;

  if (entry = config->readEntry("MoveNumber"))
    moveNo = entry.toInt();

  if (entry = config->readEntry("GameStopped"))  
    stop = (entry == "Yes");

  if (entry = config->readEntry("Position")) {
    board->setState(entry);
    boardWidget->copyPosition();
    boardWidget->draw();
  }
  updateStatus();
  updateToolBar();
  show();
  playGame();
}

void AbTop::writeConfig()
{
  KConfig* config = kapp->getConfig();
  config->setGroup("Options");

  writeOptions(config);

  QString entry;
  config->setGroup("Appearance");
  config->writeEntry("Geometry", entry.sprintf("%dx%d",width(),height()) );
  config->setGroup("Rating");
  board->saveRating(config);
  config->sync();
}


void AbTop::writeOptions(KConfig *config)
{
  QString entry;
  entry = (level_id == normal_id) ? "Normal" :
          (level_id == hard_id) ? "Hard" :
          (level_id == challange_id) ? "Challange" : "Easy";
  config->writeEntry("Level",entry);

  entry = (iplay_id == yellow_id) ? "Yellow" :
          (iplay_id == both_id) ? "Both" : "Red";
  config->writeEntry("Computer",entry);

  entry = showMoveLong ? "Yes" : "No";
  config->writeEntry("MoveSlow",entry);
	
  entry = showSpy ? "Yes" : "No";
  config->writeEntry("ShowSpy",entry);
}

void AbTop::saveProperties(KConfig *config)
{
  QString entry;
  
  writeOptions(config);
  board->saveRating(config);
	
  config->writeEntry("TimerState",entry.setNum(timerState));

  if (timerState == noGame) return;

  config->writeEntry("MoveNumber",entry.setNum(moveNo));
  config->writeEntry("GameStopped", stop ? "Yes":"No");
  config->writeEntry("Position", board->getState());
}

void AbTop::closeEvent(QCloseEvent *)
{
  quit();
}

void AbTop::savePosition()
{
  KConfig* config = kapp->getConfig();
  config->setGroup("SavedPosition");
  config->writeEntry("Position", board->getState());
}

void AbTop::restorePosition()
{
  KConfig* config = kapp->getConfig();
  config->setGroup("SavedPosition");
  QString  entry = config->readEntry("Position");
  
  timerState = notStarted;
  timer->stop();	
  board->begin(Board::color1);
  moveNo = 0;
  stop = false;
  board->setState(entry);
  boardWidget->copyPosition();
  boardWidget->draw();
  updateStatus();
  updateToolBar();  
  playGame();
}

void AbTop::setupStatusBar()
{
  char tmp[100];
  sprintf(tmp,"%s 000", klocale->translate("Move"));
  status = new KStatusBar(this);
  status->insertItem(tmp, 1);
  status->insertItem( klocale->translate("Press F2 for a new game"), 2);
  setStatusBar(status);
}

void AbTop::setupToolBar()
{
	QPixmap pm;
	
	toolbar = new KToolBar(this);

	pm = kapp->getIconLoader()->loadIcon( "ab-start.xpm" );
	toolbar->insertButton(pm, 0, SIGNAL( clicked() ),
			      this, SLOT( newGame() ),
			      TRUE, klocale->translate("New Game"));
	
	pm = kapp->getIconLoader()->loadIcon( "ab-stop.xpm" );
	toolbar->insertButton(pm, 1, SIGNAL( clicked() ),
			      this, SLOT( stopSearch() ),
			      TRUE, klocale->translate("Stop Search"));
	
	pm = kapp->getIconLoader()->loadIcon( "undo.xpm" );
	toolbar->insertButton(pm, 2, SIGNAL( clicked() ),
			      this, SLOT( takeBack() ),
			      TRUE, klocale->translate("Take back"));
	
	pm = kapp->getIconLoader()->loadIcon( "hint.xpm" );
	toolbar->insertButton(pm, 3, SIGNAL( clicked() ),
			      this, SLOT( suggestion() ),
			      TRUE, klocale->translate("Hint"));
	
	pm = kapp->getIconLoader()->loadIcon( "help.xpm" );
	toolbar->insertButton(pm, 4, SIGNAL( clicked() ),
			      this, SLOT( help() ),
			      TRUE, klocale->translate("Help"));

	toolbar->show();	
	addToolBar(toolbar);	
}


void AbTop::setupMenu()
{
  KMenuBar* menu;

  /* is this wrong ? */
  stdAccel = new KStdAccel( kapp->getConfig() );

  _file = new QPopupMenu;
  CHECK_PTR( _file );
  _file->insertItem( klocale->translate("New Game"), 
		   this, SLOT(newGame()), Key_F2 );
  _file->insertSeparator();
  _file->insertItem( klocale->translate("Save Position"), 
		   this, SLOT(savePosition()), stdAccel->save() );
  _file->insertItem( klocale->translate("Restore Position"), 
		   this, SLOT(restorePosition()), stdAccel->open() );
  _file->insertSeparator();
  stop_id = _file->insertItem( klocale->translate("Stop Search"),
			     this, SLOT(stopSearch()), Key_S );
  //  file->insertItem( "Stop", this, SLOT(stopGame()) );
  //  file->insertItem( "Continue", this, SLOT(continueGame()) );
  back_id = _file->insertItem( klocale->translate("Take back"),
			     this, SLOT(takeBack()), stdAccel->undo() );
  hint_id = _file->insertItem( klocale->translate("Hint"),
			     this, SLOT(suggestion()), Key_H );
  _file->insertSeparator();
  _file->insertItem( i18n("&Quit"), 
		     this, SLOT(quit()), stdAccel->quit() );

  _level = new QPopupMenu;
  CHECK_PTR( _level );
  easy_id = _level->insertItem( klocale->translate("Easy"),
			       this, SLOT(easy()) );
  normal_id = _level->insertItem( klocale->translate("Normal"),
				 this, SLOT(normal()) );
  hard_id = _level->insertItem( klocale->translate("Hard"),
			       this, SLOT(hard()) );
  challange_id = _level->insertItem( klocale->translate("Challange"),
				    this, SLOT(challange()) );
  _level->setCheckable( TRUE );

  _iplay = new QPopupMenu;
  CHECK_PTR( _iplay );
  red_id = _iplay->insertItem( klocale->translate("Red"),
			      this, SLOT(play_red()) );
  yellow_id = _iplay->insertItem( klocale->translate("Yellow"),
				 this, SLOT(play_yellow()) );
  both_id = _iplay->insertItem( klocale->translate("Both"), 
			       this, SLOT(play_both()) );
  _iplay->setCheckable( TRUE );

  _options = new QPopupMenu;
  CHECK_PTR( _options );
  _options->insertItem( klocale->translate("Level"), _level );
  _options->insertItem( klocale->translate("Computer plays"), _iplay );
  _options->insertSeparator();
  slow_id = _options->insertItem( klocale->translate("Move slow"),
				  this, SLOT(changeShowMove()) );
  spy_id = _options->insertItem( klocale->translate("Spy"),
				  this, SLOT(toggleSpy()) );
  _options->insertSeparator();
  _options->insertItem( klocale->translate("Save"),
				  this, SLOT(writeConfig()) );	
  _options->setCheckable( TRUE );

  QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Abalone"))
                                         + " " + KABALONE_VERSION
                                         + i18n("\n\nby Josef Weidendorfer")
                                         + " (weidendo@informatik.tu-muenchen.de)"); 

  menu  = new KMenuBar(this);
  CHECK_PTR( menu );
  menu->insertItem( klocale->translate("&File"), _file);
  menu->insertItem( klocale->translate("&Options"), _options);
  menu->insertSeparator();
  menu->insertItem( klocale->translate("&Help"), help);
  menu->show();

  setMenu(menu);
}

void AbTop::updateSpy(QString s)
{
  if (showSpy) {
    if (s.isEmpty())
      updateStatus();
    else
      status->changeItem(s,2);
  }
}

void AbTop::updateBestMove(Move& m, int value)
{
  if (showSpy) {
    board->showMove(m,3);
    boardWidget->copyPosition();
    boardWidget->draw();
    board->showMove(m,0);

    QString tmp;
    tmp.sprintf("%s : %+d", (const char*) m.name(), value-actValue);
    updateSpy(tmp);
    kapp->processEvents();
  }
}


void AbTop::updateStatus()
{
  char tmp[50];

  if (timerState == noGame)
    strcpy(tmp,"");
  else
    sprintf(tmp,"%s %d", klocale->translate("Move"), moveNo/2 + 1);
  status->changeItem(tmp,1);

  if (timerState == noGame)
    strcpy(tmp,klocale->translate("Press F2 for a new game"));
  else {
    if (timerState == gameOver)
      sprintf(tmp,"%s %s !", (board->actColor() == Board::color2) ? 
	      klocale->translate("Red"):klocale->translate("Yellow"),
	      klocale->translate("won"));
    else
      sprintf(tmp,"%s - %s", (board->actColor() == Board::color1) ? 
	      klocale->translate("Red"):klocale->translate("Yellow"),
	      iPlayNow() ? klocale->translate("I am thinking...") : 
	                   klocale->translate("It's your turn !"));
  }
  status->changeItem(tmp,2);
}

/* only <stop search>, <hint>, <take back> have to be updated */
void AbTop::updateToolBar()
{
  if (timerState == noGame || timerState == gameOver) {
    toolbar->setItemEnabled(1, false);
    toolbar->setItemEnabled(2, false);
    toolbar->setItemEnabled(3, false);
    _file->setItemEnabled(stop_id, false);
    _file->setItemEnabled(back_id, false);	  
    _file->setItemEnabled(hint_id, false);	  
    return;
  }

  /* Stop search */
  toolbar->setItemEnabled(1, iPlayNow());
  _file->setItemEnabled(stop_id, iPlayNow());	

  /* Take back */
  toolbar->setItemEnabled(2, board->movesStored() >=2 );
  _file->setItemEnabled(back_id, board->movesStored() >=2 );
	
  /* Hint */
  toolbar->setItemEnabled(3, (haveHint().type != Move::none) );
  _file->setItemEnabled(hint_id, (haveHint().type != Move::none) );
}

/* let the program be responsive even in a long search... */
void AbTop::searchBreak()
{
  kapp->processEvents();
}


void AbTop::timerDone()
{
  int interval = 400;

  switch(timerState) {
  case noGame:
  case notStarted:
    return;
  case showMove:
  case showMove+2:
  case showSugg:
  case showSugg+2:
  case showSugg+4:
    board->showMove(actMove, 2);
    interval = 200;
    break;
  case showMove+1:
  case showMove+3:
  case showSugg+1:
  case showSugg+3:
    board->showMove(actMove, 3);
    break;
  case showSugg+5:
    interval = 800;
  case showMove+4:
    board->showMove(actMove, 4);
    break;
  case showMove+5:
    board->showMove(actMove, 0);
    timerState = moveShown;
    playGame();
    return;
  case showSugg+6:
    board->showMove(actMove, 0);
    timerState = notStarted;
    boardWidget->copyPosition();
    boardWidget->draw();
    return;
  }
  boardWidget->copyPosition();
  boardWidget->draw();
  timerState++;
  timer->start(interval,TRUE);
}

void AbTop::userMove()
{
    /* User has to move */
    static MoveList list;
    
    list.clear();
    board->generateMoves(list);
    boardWidget->choseMove(&list);
}

bool AbTop::iPlayNow()
{
  int c = board->actColor();
    
  /* color1 is red */
  return ((iplay == cBoth) || 
	  ((c == Board::color1) && (iplay == cRed) ) ||
	  ((c == Board::color2) && (iplay == cYellow) ));
}

void AbTop::playGame()
{
  if (timerState == moveShown) {
    board->playMove(actMove);
    actValue = - board->calcValue();
    boardWidget->copyPosition();
    boardWidget->draw();
    timerState = notStarted;
  }
  if (!board->isValid()) { 
    stop = true;
    timerState = gameOver;
  }
  updateStatus();
  updateToolBar();
  if (stop) return;

  moveNo++;

  if (!iPlayNow()) {
    userMove();
    return;
  }
	
  kapp->processEvents();
  if (moveNo <4) {
    /* Chose a random move making the position better for actual color */

    /* If comparing ratings among color1/2 on move, we have to negate one */
    int v = -board->calcValue(), vv;
    do {
      actMove = board->randomMove();
      board->playMove(actMove);
      vv = board->calcValue();
      board->takeBack();
    } while( (board->actColor() == Board::color1) ? (vv<v) : (vv>v) );
  }
  else
    actMove = (board->bestMove());

  timerState = showMoveLong ? showMove : showMove+3;
  timerDone();
}

void AbTop::moveChoosen(Move& m)
{
  actMove = m;
  timerState = moveShown;
  playGame();
}

void AbTop::newGame()
{
  timerState = notStarted;
  timer->stop();	
  board->begin(Board::color1);
  boardWidget->copyPosition();
  boardWidget->draw();
  moveNo = 0;
  stop = false;	
  playGame();
}

void AbTop::stopGame()
{
  stop = true;
  board->stopSearch();
}

void AbTop::stopSearch()
{
  board->stopSearch();
}

void AbTop::quit()
{
  board->stopSearch();
  kapp->quit();
}

void AbTop::continueGame()
{
  if (timerState != noGame && timerState != gameOver) {
    stop = false;
    if (timerState == notStarted)
	    playGame();
  }
}

void AbTop::takeBack()
{
  if (timerState != notStarted) return;
  if (moveNo > 1) {
    board->takeBack();
    board->takeBack();
    boardWidget->copyPosition();
    boardWidget->draw();
    moveNo -=2;
    updateStatus();
    updateToolBar();
    userMove();
  }
}

Move AbTop::haveHint()
{
  static Move m;
  static int oldMoveNo = 0;

  if (timerState != notStarted) {
    m.type = Move::none;
  }
  else if (moveNo != oldMoveNo) {
    MoveList list;
    
    oldMoveNo = moveNo;
    m = board->nextMove();
    board->generateMoves(list);
    if (!list.isElement(m,0))
      m.type = Move::none;
  }
  return m;
}


void AbTop::suggestion()
{
  if (timerState != notStarted) return;
  Move m = haveHint();
  if (m.type == Move::none) return;

  actMove = m;

  timerState = showSugg;
  timerDone();
}

void AbTop::setLevel(int id, int d)
{
  _level->setItemChecked(level_id, false);
  _level->setItemChecked(level_id = id, true);
  depth = d;
  board->setDepth(depth);
}

void AbTop::easy()
{
  setLevel(easy_id, 2);
}

void AbTop::normal()
{
  setLevel(normal_id, 3);
}

void AbTop::hard()
{
  setLevel(hard_id, 4);
}

void AbTop::challange()
{
  setLevel(challange_id, 5);
}

void AbTop::setIplay(int id, int v)
{
  _iplay->setItemChecked(iplay_id, false);
  _iplay->setItemChecked(iplay_id = id, true);
  iplay = v;
  continueGame();
}

void AbTop::play_red()
{
  setIplay(red_id, cRed);
}

void AbTop::play_yellow()
{
  setIplay(yellow_id, cYellow);
}

void AbTop::play_both()
{
  setIplay(both_id, cBoth);
}

void AbTop::changeShowMove()
{
  showMoveLong = showMoveLong ? false:true;
  _options->setItemChecked(slow_id, showMoveLong);
}

void AbTop::toggleSpy()
{
  showSpy = showSpy ? false:true;
  board->updateSpy(showSpy);
  _options->setItemChecked(spy_id, showSpy);

#ifdef SPION
  if (showSpy)
    spy->show();
  else {
    spy->nextStep();
    spy->hide();
  }
#endif

}

void AbTop::help()
{
  kapp->invokeHTMLHelp("kabalone/index.html", "");
}


#include "AbTop.moc"
