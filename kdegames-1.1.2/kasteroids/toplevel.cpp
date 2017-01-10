
#include <kapp.h>
#include <qaccel.h>
#include <qmsgbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <ktopwidget.h>
#include <kmenubar.h>
#include <klocale.h>

// sound support
extern "C" {
#include <mediatool.h>
}
#include <kaudio.h>

#include "kfixedtopwidget.h"
#include "toplevel.h"
#include "version.h"

#include "toplevel.moc"

#include <X11/Xlib.h>

#define SB_SCORE	1
#define SB_LEVEL	2
#define SB_SHIPS	3


struct SLevel
{
    int    nrocks;
    double rockSpeed;
};

#define MAX_LEVELS	16

SLevel levels[MAX_LEVELS] =
{
    { 1, 0.6 },
    { 1, 0.8 },
    { 2, 0.7 },
    { 2, 0.8 },
    { 2, 0.9 },
    { 3, 0.7 },
    { 3, 0.8 },
    { 3, 0.9 },
    { 4, 0.7 },
    { 4, 0.8 },
    { 4, 0.9 },
    { 5, 0.7 },
    { 5, 0.8 },
    { 5, 0.9 },
    { 5, 1.0 }
};

const char *soundEvents[] = 
{
    "ShipDestroyed",
    "RockDestroyed",
    0
};

const char *soundDefaults[] = 
{
    "Explosion.wav",
    "ploop.wav",
    0
};

KAstTopLevel::KAstTopLevel() : KFixedTopWidget()
{
    setCaption( kapp->getCaption() );

    QWidget *mainWin = new QWidget( this );

    view = new KAsteroidsView( mainWin );
    connect( view, SIGNAL( shipKilled() ), SLOT( slotShipKilled() ) );
    connect( view, SIGNAL( rockHit(int) ), SLOT( slotRockHit(int) ) );
    connect( view, SIGNAL( rocksRemoved() ), SLOT( slotRocksRemoved() ) );

    QVBoxLayout *vb = new QVBoxLayout( mainWin );
    QHBoxLayout *hb = new QHBoxLayout;
    vb->addLayout( hb );

    QFont labelFont( "helvetica", 24 );
    QColorGroup grp( darkGreen, black, QColor( 128, 128, 128 ),
	    QColor( 64, 64, 64 ), black, darkGreen, black );
    QPalette pal( grp, grp, grp );

    mainWin->setPalette( pal );

    hb->addSpacing( 10 );

    QLabel *label;
    label = new QLabel( klocale->translate("Score"), mainWin );
    label->setFont( labelFont );
    label->setPalette( pal );
    label->setFixedWidth( label->sizeHint().width() );
    hb->addWidget( label );

    scoreLCD = new QLCDNumber( 6, mainWin );
    scoreLCD->setFrameStyle( QFrame::NoFrame );
    scoreLCD->setSegmentStyle( QLCDNumber::Flat );
    scoreLCD->setFixedWidth( 150 );
    scoreLCD->setPalette( pal );
    hb->addWidget( scoreLCD );
    hb->addStretch( 10 );

    label = new QLabel( klocale->translate("Level"), mainWin );
    label->setFont( labelFont );
    label->setPalette( pal );
    label->setFixedWidth( label->sizeHint().width() );
    hb->addWidget( label );

    levelLCD = new QLCDNumber( 2, mainWin );
    levelLCD->setFrameStyle( QFrame::NoFrame );
    levelLCD->setSegmentStyle( QLCDNumber::Flat );
    levelLCD->setFixedWidth( 70 );
    levelLCD->setPalette( pal );
    hb->addWidget( levelLCD );
    hb->addStretch( 10 );

    label = new QLabel( klocale->translate("Ships"), mainWin );
    label->setFont( labelFont );
    label->setFixedWidth( label->sizeHint().width() );
    label->setPalette( pal );
    hb->addWidget( label );

    shipsLCD = new QLCDNumber( 1, mainWin );
    shipsLCD->setFrameStyle( QFrame::NoFrame );
    shipsLCD->setSegmentStyle( QLCDNumber::Flat );
    shipsLCD->setFixedWidth( 40 );
    shipsLCD->setPalette( pal );
    hb->addWidget( shipsLCD );

    hb->addStrut( 30 );

    QFrame *sep = new QFrame( mainWin );
    sep->setMaximumHeight( 5 );
    sep->setFrameStyle( QFrame::HLine | QFrame::Raised );
    sep->setPalette( pal );

    vb->addWidget( sep );

    vb->addWidget( view, 10 );
    vb->freeze( 640, 480 );

    createMenuBar();

    menu->show();
    setView( mainWin );
    setMenu( menu );

    setFocusPolicy( StrongFocus );
    setFocus();

    readSettings();

    kas = new KAudio;

    if ( kas->serverStatus() )
	sound = false;
}

KAstTopLevel::~KAstTopLevel()
{
    delete kas;
}

void KAstTopLevel::createMenuBar()
{
    menu = new KMenuBar( this );
    CHECK_PTR( menu );

    QPopupMenu *fileMenu = new QPopupMenu;
    CHECK_PTR( fileMenu );
    fileMenu->insertItem( klocale->translate( "&New Game" ), this,
	SLOT(slotNewGame()), CTRL+Key_N );
    fileMenu->insertSeparator( );
    fileMenu->insertItem( klocale->translate( "&Quit" ), this, SLOT(slotQuit()),
	CTRL+Key_Q );

    QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Asteroids"))
                                         + " " + KASTEROIDS_VERSION
                                         + i18n("\n\nby Martin R. Jones")
                                         + " (mjones@kde.org)"); 

    menu->insertItem( klocale->translate( "&File" ), fileMenu );
    menu->insertSeparator();
    menu->insertItem( klocale->translate( "&Help" ), help );
}

void KAstTopLevel::readSettings()
{
    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup( "Sounds" );

    QString qs;

    for ( int i = 0; soundEvents[i]; i++ )
    {
	qs = config->readEntry( soundEvents[i] );
	if ( !qs.isEmpty() )
	    soundDict.insert( soundEvents[i], qs );
	else
	    soundDict.insert( soundEvents[i], soundDefaults[i] );
    }

    qs = config->readEntry( "PlaySounds" );
    if ( qs.isEmpty() || qs != "Yes" )
	sound = false;
    else
	sound = true;
}

void KAstTopLevel::playSound( const char *snd )
{
    kas->stop();
    QString filename = kapp->kde_datadir().copy();
    filename += "/kasteroids/";
    filename += soundDict[ snd ];
    kas->play( filename );
}

void KAstTopLevel::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() )
    {
	case Key_Left:
	    view->rotateLeft( true );
	    event->accept();
	    break;

	case Key_Right:
	    view->rotateRight( true );
	    event->accept();
	    break;

	case Key_Up:
	    view->thrust( true );
	    event->accept();
	    break;

	case Key_Space:
	    view->shoot( true );
	    event->accept();
	    break;

	default:
	    event->ignore();
    }
}

void KAstTopLevel::keyReleaseEvent( QKeyEvent *event )
{
    switch ( event->key() )
    {
	case Key_Left:
	    view->rotateLeft( false );
	    event->accept();
	    break;

	case Key_Right:
	    view->rotateRight( false );
	    event->accept();
	    break;

	case Key_Up:
	    view->thrust( false );
	    event->accept();
	    break;

	case Key_Return:
	    if ( waitShip )
	    {
		view->newShip();
		event->accept();
		waitShip = false;
		view->hideText();
	    }
	    break;

	case Key_Space:
	    view->shoot( false );
	    event->accept();
	    break;

	default:
	    event->ignore();
    }
}

void KAstTopLevel::focusInEvent( QFocusEvent * )
{
    XAutoRepeatOff( qt_xdisplay() );
}

void KAstTopLevel::focusOutEvent( QFocusEvent * )
{
    XAutoRepeatOn( qt_xdisplay() );
}

void KAstTopLevel::slotNewGame()
{
    score = 0;
    scoreLCD->display( 0 );
    level = 0;
    levelLCD->display( level );
    shipsRemain = 5;
    shipsLCD->display( shipsRemain-1 );
    view->newGame();
    view->setRockSpeed( levels[0].rockSpeed );
    view->addRocks( levels[0].nrocks );
    view->showText( klocale->translate( "Press Enter to launch." ), yellow );
    waitShip = true;
}

void KAstTopLevel::slotQuit()
{
    kapp->quit();
}

void KAstTopLevel::slotShipKilled()
{
    shipsRemain--;
    shipsLCD->display( shipsRemain-1 );

    playSound( "ShipDestroyed" );

    if ( shipsRemain )
    {
	waitShip = true;
	view->showText( klocale->translate( "Ship Destroyed.  Press Enter to launch."), yellow );
    }
    else
    {
	QMessageBox::message(klocale->translate("KAsteriods"),
	    klocale->translate("Game Over!"));
	view->endGame();
    }
}

void KAstTopLevel::slotRockHit( int size )
{
    switch ( size )
    {
	case 0:
	    score += 10;
	    break;

	case 1:
	    score += 20;
	    break;

	default:
	    score += 40;
    }

    playSound( "RockDestroyed" );

    scoreLCD->display( score );
}

void KAstTopLevel::slotRocksRemoved()
{
    level++;

    if ( level >= MAX_LEVELS )
	level = MAX_LEVELS - 1;

    view->setRockSpeed( levels[level-1].rockSpeed );
    view->addRocks( levels[level-1].nrocks );

    levelLCD->display( level );
}

