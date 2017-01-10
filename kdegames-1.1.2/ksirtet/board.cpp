#include "board.h"
#include "dialogs.h"
#include "dbutton.h"

#include <stdio.h>
#include <unistd.h>

#include <qtimer.h>
#include <qkeycode.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qmsgbox.h>

#include <kapp.h>
#include <kmsgbox.h>

#include "board.moc"


const uint blinkLineTime = 150;
const uint dropDownTime = 10;

Board::Board( KAccel * parentkaccel, QWidget *p, const char *name )
: QFrame( p, name ), kacc( parentkaccel )
{
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setPalette(black);

    multiGame = FALSE;
    timer = new QTimer(this);
    paint = new QPainter(this);
    connect( timer, SIGNAL(timeout()), SLOT(timeout()) );

    colors[0].setRgb(200,100,100);
    colors[1].setRgb(100,200,100);
    colors[2].setRgb(100,100,200);
    colors[3].setRgb(200,200,100);
    colors[4].setRgb(200,100,200);
    colors[5].setRgb(100,200,200);
    colors[6].setRgb(218,170,  0);
    colors[7].setRgb(100,100,100);
    colors[8].setRgb(  0,  0,  0);

    state = NoGame;
    updateTimeoutTime();   /* Sets timeoutTime */

    msg = new QLabel(this);
    msg->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    msg->setAlignment(AlignCenter);

    pb = new QPushButton(this);
    connect( pb, SIGNAL(clicked()), SLOT(buttonClick()) );

    /* configuration & highscore initialisation */
    kconf = kapp->getConfig();
    isConfigWritable =
	(kapp->getConfigState()==KApplication::APPCONFIG_READWRITE);
	
    /* if the entries do not exist : create them */
    kconf->setGroup(HS_GRP);
    QString str1, str2;
    for (int i=0; i<NB_HS; i++) {
	str1.sprintf("%s%i", HS_NAME_KEY, i);
	if ( !kconf->hasKey(str1) )
	    kconf->writeEntry(str1, i18n("Anonymous"));
	str2.sprintf("%s%i", HS_SCORE_KEY, i);    
	if ( !kconf->hasKey(str2) )
	    kconf->writeEntry(str2, 0);
    }

    /* show the midbutton with "start game" */
    midbutton(FALSE);
}


Board::~Board()
{
	delete timer;
	delete paint;
}


void Board::buttonClick()
{
  if(pb->isVisible()) {
    if (state==Paused) 
      pause();
    else 
      startGame();
  }
}


void Board::midbutton(bool game_over)
{
	char spa[50];
	char sre[50];
	int sw, sh, sw2, sh2;

	if (state==Paused) {
		sprintf(spa, i18n("Game paused"));
		QFontMetrics fm = msg->fontMetrics();
		sw = fm.width(spa) + 10;
		sh = fm.height() + 10;
		
		sprintf(sre, i18n("Press to resume"));
		QFontMetrics fm2 = pb->fontMetrics();
		sw2 = fm2.width(sre) + 10;
		sh2 = fm2.height() + 10;
		
		msg->setGeometry( (BOARD_W - sw)/2, (BOARD_H - sh-sh2)/3, sw, sh );
		msg->setText(spa);
		msg->show();
		
		pb->setGeometry( (BOARD_W - sw2)/2, 2*(BOARD_H - sh-sh2)/3, sw2, sh2 );
		pb->setText(sre);
		pb->show();
		return;
	}

	msg->hide();
	if (game_over) {
		sprintf(spa, i18n("Game over"));
		QFontMetrics fm = msg->fontMetrics();
		sw = fm.width(spa) + 15;     
		sh = fm.height() + 15;
		sprintf(sre,  i18n("Press Return to replay"));
	} else {
		sw = 0;
		sh = 0;
		sprintf(sre, i18n("Press to start game"));
	}
	
	QFontMetrics fm2 = pb->fontMetrics();
	sw2 = fm2.width(sre) + 10;
	sh2 = fm2.height() + 10;
	
	if (game_over) {
		msg->setGeometry( (BOARD_W - sw)/2, (BOARD_H - sh-sh2)/3, sw, sh );
		msg->setText(spa);     
		msg->show();
	}
	
	pb->setGeometry( (BOARD_W - sw2)/2, 2*(BOARD_H - sh-sh2)/3, sw2, sh2 );
	pb->setText(sre);
	pb->show();
}


void Board::showBoard()
{
  state = Playing;
  setPieceMovingKeys(TRUE);
  msg->hide();
  pb->hide();
  GenericTetris::showBoard();
}


void Board::startGame()
{
	showBoard();
	GenericTetris::startGame();
	/* Note that the timer is started by updateLevel! */
}


void Board::stopGame()
{
	timer->stop();
	setPieceMovingKeys(FALSE);
}


void Board::setPieceMovingKeys( bool activate )
{
    kacc->setItemEnabled("Move left", activate);
    kacc->setItemEnabled("Move right", activate);
    kacc->setItemEnabled("Drop down", activate);
    kacc->setItemEnabled("One line down", activate);
    kacc->setItemEnabled("Rotate left", activate);
    kacc->setItemEnabled("Rotate right", activate);
}


void Board::pause()
{
	if (multiGame) return;
	
	switch (state) {
	 case NoGame : break;
	 case Paused : 
		/* already in pause : resume game (reactivate the piece-moving keys */
		showBoard();
		removeFullLines();
		if (multiGame) updateOpponents();
        else timer->start(timeoutTime);
		break;
	 default :
		stopGame();
		state = Paused;
		GenericTetris::hideBoard();
		midbutton(FALSE);
	}
}


void Board::drawSquare(int x, int y, int value)
{
    const int X = XOFF  + x*BLOCK_W;
    const int Y = YOFF  + (y - 1)*BLOCK_H;
	
    drawTetrisButton( paint, X, Y, BLOCK_W, BLOCK_H,
					  value == 0 ? &colors[8] : &colors[value-1] );
}


void Board::drawNextSquare(int x, int y, int value)
{
	emit drawNextSquareSignal(x, y, value == 0 ? 0 : &colors[value-1]);
}


void Board::updateRemoved( int noOfLines )
{
	removedLines += noOfLines - oldFullLines;
	oldFullLines = noOfLines;
    emit updateRemovedSignal( noOfLines );
}


void Board::updateScore(int newScore)
{
	GenericTetris::updateScore(newScore);
    emit updateScoreSignal(newScore);
}


void Board::updateLevel(int newLevel)
{
	GenericTetris::updateLevel(newLevel);
	
	if ( !multiGame ) {
		updateTimeoutTime();
		timer->start(timeoutTime);
	}
	
	emit updateLevelSignal(newLevel);
}


void Board::waitAfterLine()
{
	if ( !multiGame ) {
		lightFullLines(TRUE);
		state = WaitingAfterLine;
		loop = 0;
        timer->start(blinkLineTime, TRUE);
	} else {
		removeFullLines();
		newPiece();
	}
}


void Board::gameOver()
{
	stopGame();
	state = NoGame;

	if ( !multiGame ) 
		setHighScore(getScore());
	else {
//		QString msg = i18n("You suck !");
//		net_obj->gameOver(msg);
	}
	
	midbutton(TRUE);
}


void Board::timeout()
{
	if ( multiGame ) {
		/* update own status */
		net_obj->ownStatus(Height-nClearLines, removedLines);
	
		/* call net object playTimeout */
		if ( !net_obj->playTimeout(serror) ) {
			timer->stop();
			gameOver();
			net_obj->gameOver(serror);
			return;
		}
		
		/* if CLIENT_MODE : restart timer (ajust with the server one) */
		if ( net_obj->isClient() )
			timer->start(net_obj->getTimeout());
		
		/* update names & position of opponents on screen */
		emit updateOpponents();
			
		/* check and place opponent gift :) */
		checkOpponentGift();
			
		removedLines = 0;
	}
	
	switch (state) {
	 case WaitingAfterLine :
		switch (loop) {
		 case 0 :
			lightFullLines(FALSE);
			loop = 1;
			timer->start(blinkLineTime, TRUE);
			break;
		 case 1 :
			lightFullLines(TRUE);
			loop = 2;
			timer->start(blinkLineTime, TRUE);
			break;
		 default :
			lightFullLines(FALSE);
			removeFullLines();
			state = Playing;
			newPiece();
			timer->start(timeoutTime);
		}
		break;
	 default : 
		oneLineDown();
	}
}


void Board::paintEvent(QPaintEvent *e)
{
	if (state == Paused) return;
	
	QRect r = e->rect().intersect(QRect(XOFF, YOFF,
									   width()-2*XOFF, height()-2*YOFF));
	if ( r.isEmpty() ) return;
	
	r.moveBy(-XOFF, -YOFF);
    updateBoard(r.left()/BLOCK_W, r.top()/BLOCK_H,
				r.right()/BLOCK_W, r.bottom()/BLOCK_H);
}


void Board::updateTimeoutTime()
{
    timeoutTime = 1000/(1 + getLevel());
}


void Board::options()
{
	Options(this);
}


void Board::showHighScores()
{
	WHighScores(TRUE, 0, this);
}


void Board::setHighScore(int score)
{
	if ( isConfigWritable ) {
		WHighScores(FALSE, score, this);
		/* save the new score (in the file to be sure it won't be lost) */
		kconf->sync();
	} else
		KMsgBox::message(0, i18n("Warning"), 
						 i18n("Highscores file not writable !"),
						 KMsgBox::EXCLAMATION, i18n("Close"));
}


void Board::initMultiGame(NetObject *net_object)
{
	GenericTetris::clearBoard();
	net_obj = net_object;

	if ( !net_obj->initGame(serror) ) ERROR(serror)
	else {
		/* start game */
		multiGame = TRUE;
		emit showOpponents();
		startGame();
			
		/* start timer */
		timer->start(net_obj->getTimeout());
	}
}


void Board::lightLines(int y, int nb, bool)
{
	paint->setPen(white);
	paint->setRasterOp(XorROP);
	paint->drawRect(XOFF, YOFF + (boardHeight()-y)*BLOCK_H,
					Width*BLOCK_W, nb*BLOCK_H);
	paint->setRasterOp(CopyROP);
}


void Board::pDropDown()
{
	if ( state==WaitingAfterLine ) return;
	if ( multiGame ) { dropDown(); return; };
	
	state = DropDown;
	timer->start(dropDownTime);
	oneLineDown();
}
