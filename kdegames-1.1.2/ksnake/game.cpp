
/*
  note: the code to lookup and insert the pixmaps
  into the Options menu was copied and adapted
  from KReversi.
  thanks.
  */
#include <qdir.h>
#include <qregexp.h>

#include <qlcdnum.h>
#include <qkeycode.h>
#include <qcolor.h>
#include <qpopmenu.h>
#include <qmsgbox.h>

#include <kapp.h>
#include <kmenubar.h>

#include <kcolordlg.h>

#include "rattler.h"
#include "score.h"
#include "game.h"
#include "startroom.h"
#include "levels.h"

#include "trys.h"
#include "lcdrange.h"
#include "progress.h"

#include "view.h"
#include "keys.h"

#include "version.h"

Game::Game() :  KTopLevelWidget()
{
    setCaption( kapp->getCaption() );

    setIcon(klocale->translate("Snake Race"));

    conf = kapp->getConfig();
    if(conf == NULL) {
	printf(klocale->translate("KConfig error ??\n"));
	kapp->quit();
    }

    levels = new Levels();
    score  = new Score;
    menu();
    checkMenuItems();

    View *view = new View(this);
    rattler = view->rattler;
    rattler->setFocus();

    connect(rattler, SIGNAL(setPoints(int)), view->lcd, SLOT(display(int)));
    connect(rattler, SIGNAL(setTrys(int)), view->trys, SLOT(set(int)));
    connect(rattler, SIGNAL(rewind()), view->pg, SLOT(rewind()));
    connect(rattler, SIGNAL(advance()), view->pg, SLOT(advance()));
    connect(view->pg, SIGNAL(restart()), rattler, SLOT(restartTimer()));

    connect(rattler, SIGNAL(togglePaused()), this, SLOT(togglePaused()));
    connect(rattler, SIGNAL(setScore(int)), score, SLOT(setScore(int)));

    menubar->show();
    setMenu(menubar);
    view->show();
    setView(view);
}


void Game::menu()
{

    game = new QPopupMenu();
    CHECK_PTR( game );
    game->insertItem( klocale->translate("New"), this, SLOT(newGame()),Key_F2);
    pauseID = game->insertItem( klocale->translate("Pause"), this , SLOT(pauseGame()), Key_F3);
    game->insertItem( klocale->translate("High Scores..."), this, SLOT(showHighScores()));
    game->insertSeparator();
    game->insertItem( klocale->translate("&Quit"),  this, SLOT(quitGame()), CTRL+Key_Q );
    game->setCheckable( TRUE );

    balls = new QPopupMenu;
    CHECK_PTR( balls );
    ballsID[0] = balls->insertItem( klocale->translate("0"));
    ballsID[1] = balls->insertItem( klocale->translate("1"));
    ballsID[2] = balls->insertItem( klocale->translate("2"));
    ballsID[3] = balls->insertItem( klocale->translate("3"));
    balls->setCheckable( TRUE );
    connect(balls, SIGNAL(activated(int)), this, SLOT ( ballsChecked(int) ));

    snakes = new QPopupMenu;
    CHECK_PTR( snakes );
    snakesID[0] = snakes->insertItem( klocale->translate("0"));
    snakesID[1] = snakes->insertItem( klocale->translate("1"));
    snakesID[2] = snakes->insertItem( klocale->translate("2"));
    snakesID[3] = snakes->insertItem( klocale->translate("3"));
    snakes->setCheckable( TRUE );
    connect(snakes, SIGNAL(activated(int)), this,
	    SLOT ( snakesChecked(int) ));


    pix = new QPopupMenu;
    lookupBackgroundPixmaps();
    pixID.resize(backgroundPixmaps.count());

    if(backgroundPixmaps.count() == 0)
	pix->insertItem(klocale->translate("none"));
    else
	for(unsigned i = 0; i < backgroundPixmaps.count(); i++) {
	    // since the filename may contain underscore, they
	    // are replaced with spaces in the menu entry
	    QString s(backgroundPixmaps.at(i)->baseName());
	    s = s.replace(QRegExp("_"), " ");
	    pixID[i] = pix->insertItem((const char *)s);
	}
    pix->setCheckable( TRUE );
    connect(pix, SIGNAL(activated(int)), this,
	    SLOT ( pixChecked(int) ));

    options = new QPopupMenu();
    CHECK_PTR( options );
    skillID[0] = options->insertItem( klocale->translate("Beginner"));
    skillID[1] = options->insertItem( klocale->translate("Intermediate"));
    skillID[2] = options->insertItem( klocale->translate("Advanced"));
    skillID[3] = options->insertItem( klocale->translate("Expert"));
    options->insertSeparator();
    options->insertItem(klocale->translate("Balls"), balls);
    options->insertItem(klocale->translate("Computer Snakes"), snakes);
    options->insertSeparator();
    options->insertItem(klocale->translate("Select background color..."), this, SLOT(backgroundColor()));
    options->insertItem(klocale->translate("Select background pixmap"), pix);
    options->insertSeparator();
    options->insertItem(klocale->translate("Change keys..."),this, SLOT(confKeys()));
    options->insertSeparator();
    options->insertItem(klocale->translate("Starting Room..."), this, SLOT(startingRoom()));

    options->setCheckable( TRUE );
    connect(options, SIGNAL(activated(int)), this, SLOT ( skillChecked(int) ));

    QPopupMenu *help = kapp->getHelpMenu(true, QString(klocale->translate("Snake Race"))
                                         + " " + KSNAKE_VERSION
                                         + klocale->translate("\n\nby Michel Filippi"
                                         " (mfilippi@sade.rhein-main.de)")); 
    menubar = new KMenuBar( this );
    CHECK_PTR( menubar );
    menubar->insertItem( klocale->translate("&Game"), game );
    menubar->insertItem( klocale->translate("&Options"), options );
    menubar->insertSeparator();
    menubar->insertItem( klocale->translate("&Help"), help);
}

void Game::ballsChecked(int id)
{
    for ( int x = 0; x < 4; x++)
	if (ballsID[x] != id)
	    balls->setItemChecked( ballsID[x], FALSE );
	else { balls->setItemChecked( ballsID[x], TRUE );
	conf->writeEntry("Balls", x);
	rattler->setBalls(x);
	}
}

void Game::snakesChecked(int id)
{
    for ( int x = 0; x < 4; x++)
	if (snakesID[x] != id)
	    snakes->setItemChecked( snakesID[x], FALSE );
	else { snakes->setItemChecked( snakesID[x], TRUE );
	conf->writeEntry("ComputerSnakes", x);
	rattler->setCompuSnakes(x);
	}
}

void Game::skillChecked(int id)
{
    if (options->indexOf(id) > 3)
	return;

    for ( int x = 0; x < 4; x++)
	if (skillID[x] != id)
	    options->setItemChecked( skillID[x], FALSE );
	else { options->setItemChecked( skillID[x], TRUE );
	conf->writeEntry("Skill", x);
	rattler->setSkill(x);
	}
}

void Game::pixChecked(int id)
{
    for ( unsigned int x = 0; x < backgroundPixmaps.count(); x++)
	pix->setItemChecked( pixID[x] , pixID[x] == id );

	conf->writeEntry("Background", 2);
	conf->writeEntry("BackgroundPixmap",
				      backgroundPixmaps.at(id)->filePath());

	rattler->reloadRoomPixmap();
}

void Game::checkMenuItems()
{
    balls->setItemChecked( ballsID[conf->readNumEntry("Balls", 1)], TRUE );
    snakes->setItemChecked( snakesID[conf->readNumEntry("ComputerSnakes", 1)], TRUE );
    options->setItemChecked( skillID[conf->readNumEntry("Skill", 1)], TRUE );

    QString path = conf->readEntry("BackgroundPixmap");
    for ( unsigned int x = 0; x < backgroundPixmaps.count(); x++) {
	if (path == backgroundPixmaps.at(x)->filePath()) {
	    pix->setItemChecked( x , TRUE );
	    break;
	}
    }
}

void Game::quitGame()
{
    kapp->quit();
}

void Game::showHighScores()
{
    score->display(-1);  // Magic number because bug in moc doesn't let us
                         // default 2 parameters.
}

void Game::newGame()
{
    rattler->restart();
}

void Game::pauseGame()
{
    rattler->pause();
}

void Game::togglePaused()
{
    static bool checked = FALSE;
    checked = !checked;
    game->setItemChecked( pauseID, checked );
}

void Game::startingRoom()
{
    int r = 0;
    StartRoom *sr = new StartRoom(conf->readNumEntry("StartingRoom", 1), &r);
    sr->exec();
    delete sr;

    if (r != 0) {
	conf->writeEntry("StartingRoom", r);
	rattler->setRoom(r);
    }
}

void Game::confKeys()
{
    Keys *keys = new Keys();
    if (keys->exec() == QDialog::Accepted)
	rattler->initKeys();
    delete keys;
}


//taken from KReversi
void Game::backgroundColor()
{
    QString s;
    QColor c;
      if(KColorDialog::getColor(c)) {
	conf->writeEntry("Background", 1);
	s.sprintf("%d %d %d", c.red(), c.green(), c.blue());
	conf->writeEntry("BackgroundColor", (const char *)s);
	rattler->reloadRoomPixmap();
      }
}

void Game::lookupBackgroundPixmaps()
{
    QString pixDir;
    pixDir.setStr(KApplication::kde_datadir().copy());
    pixDir.append("/ksnake/backgrounds");
    QDir dir(pixDir, "*.xpm");

    if(!dir.exists())
	return;

    const QFileInfoList *fl = dir.entryInfoList();

    // sanity check, maybe the directory is unreadable
    if(fl == NULL)
	return;

    QFileInfoListIterator it( *fl );
    QFileInfo *fi;

    while((fi = it.current())) {
	backgroundPixmaps.append(new QFileInfo(*fi));
	++it;
    }
}






/************************** main ******************************/





int main( int argc, char **argv )
{
  KApplication a( argc, argv, "ksnake" );

  Game g;
  a.setMainWidget( &g );
  g.show();
  return a.exec();
}
