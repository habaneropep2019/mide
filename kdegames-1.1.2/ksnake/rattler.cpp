#include <qdatetm.h>
#include <stdlib.h>
#include <qtimer.h>
#include <qlist.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qkeycode.h>
#include <qmsgbox.h>
#include <qbitarry.h>

#include <kapp.h>
#include <kaccel.h>

#include "rattler.h"
#include "board.h"
#include "level.h"
#include "basket.h"
#include "ball.h"
#include "snake.h"
#include "pixServer.h"


QBitArray gameState(5);
QLabel *label = 0;
int speed[4] = { 85, 75, 55, 40 };

Rattler::Rattler( QWidget *parent, const char *name )
    : QWidget( parent, name )
{

    setFocusPolicy(QWidget::StrongFocus);

    KConfig *conf = kapp->getConfig();
    numBalls = conf->readNumEntry("Balls", 1);
    numSnakes = conf->readNumEntry("ComputerSnakes", 1);
    skill = conf->readNumEntry("Skill", 1);
    room = conf->readNumEntry("StartingRoom", 1);


    initKeys();

    board = new Board(35*35);
    level = new Level(board);

    pix = new PixServer(board, this);
    setFixedSize(pix->levelPix().size());

    basket = new Basket(board, pix);
    samy = new SamySnake(board, pix);

    computerSnakes = new QList<CompuSnake>;
    computerSnakes->setAutoDelete( TRUE );

    balls = new QList<Ball>;
    balls->setAutoDelete( TRUE );

    connect( samy, SIGNAL(closeGate(int)), this, SLOT(closeGate(int)));
    connect( samy, SIGNAL(score(bool, int)), this, SLOT(scoring(bool,int)));
    connect( samy, SIGNAL(goingOut()), this, SLOT(speedUp()));
    connect( basket, SIGNAL(openGate()), this, SLOT(openGate()));

    QTime midnight( 0, 0, 0 );
    srandom( midnight.secsTo(QTime::currentTime()) );

    gameState.fill(FALSE);
    gameState.setBit(Demo);

    timerCount = 0;
    QTimer::singleShot( 2000, this, SLOT(demo()) );
}

void Rattler::paintEvent( QPaintEvent *e)
{
    QRect rect = e->rect();
    bool dirty = FALSE;

    if (!rect.isEmpty()) {
	dirty = TRUE;
	QPixmap levelPix = pix->levelPix();
	bitBlt(this, rect.x(), rect.y(),
	       &levelPix, rect.x(), rect.y(), rect.width(), rect.height());
    }

    if (!gameState.testBit(Init) && !gameState.testBit(Over)) {

	basket->repaint( dirty );

	if(!gameState.testBit(Demo))samy->repaint( dirty );

	for (CompuSnake *as = computerSnakes->first(); as != 0;
	     as = computerSnakes->next())
	    if (as) as->repaint( dirty );

	for (Ball *b = balls->first(); b != 0;
	     b = balls->next())
	    if (b) b->repaint();
    }
}

void Rattler::timerEvent( QTimerEvent * )
{
    timerCount++;

    if ( !leaving )		// advance progressBar unless Samy
	emit advance();		// is going out

    for (CompuSnake *c = computerSnakes->first(); c != 0;
	 c = computerSnakes->next())
	if(c) c->nextMove();

    for (Ball *b = balls->first(); b != 0;
	 b = balls->next())
	if (b) b->nextMove();

    samyState state = ok;

    if(!gameState.testBit(Demo))
	state = samy->nextMove(direction);

    repaint(0,0,0,0, FALSE);

    if (state == ko)
	newTry();
    else if (state == out)
	levelUp();
}

void Rattler::initKeys()
{
    KConfig *conf = kapp->getConfig();

    QString up("Up");
    up = conf->readEntry("upKey", (const char*) up);
    UpKey    = stringToKey(up);

    QString down("Down");
    down = conf->readEntry("downKey", (const char*) down);
    DownKey  = stringToKey(down);

    QString left("Left");
    left = conf->readEntry("leftKey", (const char*) left);
    LeftKey  = stringToKey(left);

    QString right("Right");
    right = conf->readEntry("rightKey", (const char*) right);
    RightKey = stringToKey(right);

}

void Rattler::keyPressEvent( QKeyEvent *k )
{
    if (gameState.testBit(Paused))
	return;

    uint key = k->key();
    if (key == UpKey)
	direction = N;
    else if (key == DownKey)
	direction = S;
    else if (key == RightKey)
	direction = E;
    else if (key == LeftKey)
	direction = W;
    else {
	k->ignore();
	return;
    }
    k->accept();

    /*
    switch ( k->key()) {
    case (const int)UpKey:
	direction = N;
	break;
    case  DownKey:
	direction = S;
	break;
    case RightKey:
	direction = E;
	break;
    case LeftKey:
	direction = W;
	break;
    default:
	k->ignore();
	return;
    }
    k->accept();
    */





}

void Rattler::closeGate(int i)
{
    board->set(i, brick);
    pix->restore(i);
}

void Rattler::openGate()
{
    board->set(NORTH_GATE, empty);
    pix->erase(NORTH_GATE);
}

void Rattler::scoring(bool win, int i)
{
    Fruits fruit  = basket->eaten(i);

    if (gameState.testBit(Demo))
	win = TRUE;

    int p = 0;

    switch (fruit) {

    case Red:
	if (win) {
	    if (!timerHasRunOut)
		p = 1 + skill*2;
	    else p = 1;
	}
	else p = -2;
	break;

    case Golden:
	if (win) {
	    if (!timerHasRunOut)
		p = 2 + (skill*2) + (numSnakes*2) + (numBalls+2);
	    else p = 2;
	}
	else p = -5;
	break;

    default:
	break;

    }

    score(p);
}

void Rattler::score(int p)
{
    points += p;
    points = (points < 0 ? 0 : points);
    emit setPoints(points);

	while (points > check*50) {
	    check++;
	    if (trys < 7 && !gameState.testBit(Demo))
		emit setTrys(++trys);
	}
}

void Rattler::killedComputerSnake()
{
    if (!gameState.testBit(Demo))
	score(20);
}

void Rattler::pause()
{
    if (gameState.testBit(Init))
	return;

    if (gameState.testBit(Playing)) {

	gameState.toggleBit(Playing);
	gameState.setBit(Paused);
	stop();

	label = new QLabel(this);
	label->setFont( QFont( "Times", 16, QFont::Bold ) );
	label->setText(klocale->translate("Game Paused\n Press F3 to resume\n"));
	label->setAlignment( AlignCenter );
	label->setFrameStyle( QFrame::Panel | QFrame::Raised );
	label->setGeometry(182, 206, 198, 80);
	label->show();
	emit togglePaused();
    }
    else if (gameState.testBit(Paused)) {
	gameState.toggleBit(Paused);
	gameState.setBit(Playing);
	start();
	cleanLabel();
	emit togglePaused();
    }
}

void Rattler::cleanLabel()
{
    if (label) {
	delete label;
	label = 0;
    }
}

void Rattler::restartDemo()
{
    if (!gameState.testBit(Demo))
	return;

    int r = 50000+ (random() % 30000);
    QTimer::singleShot( r, this, SLOT(restartDemo()) );

    stop();
    level->create(Intro);
    init(FALSE);
    repaint();
    start();
}

void Rattler::demo()
{
    static  bool first_time = TRUE;

    if(gameState.testBit(Init) || gameState.testBit(Playing))
	return;

    stop();

    QTimer::singleShot( 60000, this, SLOT(restartDemo()) );
    gameState.fill(FALSE);
    gameState.setBit(Init);
    gameState.setBit(Demo);
    resetFlags();

    if(!first_time) {
	level->create(Intro);
	pix->initRoomPixmap();
    }
    repaint(rect(), FALSE);
    init(FALSE);
    run();
    first_time = FALSE;
}

void Rattler::restart()
{
    if (gameState.testBit(Init))
	return;
    stop();

    if (gameState.testBit(Paused) || gameState.testBit(Playing)) {

	switch( QMessageBox::information( this, klocale->translate("Snake Race"),
					  klocale->translate("A game is already started\n"
					  "Start a new one ?\n"),
					  klocale->translate("&Yes"), klocale->translate("&No"),
					  0, 1 ) ) {
	case 0:
	    if ( gameState.testBit(Paused))
		emit togglePaused();
	    break;
	case 1:
	    if ( !gameState.testBit(Paused))
		start();
	    return;
	}
    }

    gameState.fill(FALSE);
    gameState.setBit(Init);
    gameState.setBit(Playing);

    resetFlags();

    level->set(room);
    level->create(Banner);
    pix->initRoomPixmap();

    cleanLabel();

    repaint();
    QTimer::singleShot( 2000, this, SLOT(showRoom()) );
}

void Rattler::newTry()
{
    stop();

    if(trys==0) {
	gameState.fill(FALSE);
	gameState.setBit(Over);
	level->create(GameOver);
	pix->initRoomPixmap();
	repaint();
	QTimer::singleShot( 5000, this, SLOT(demo()) );
	emit setScore(points);
	return;
    }
    --trys;
    gameState.fill(FALSE);
    gameState.setBit(Init);
    gameState.setBit(Playing);

    level->create(Room);
    pix->initRoomPixmap();
    init(TRUE);
    repaint();
    QTimer::singleShot( 1000, this, SLOT(run()) );
}

void Rattler::levelUp()
{
    stop();

    gameState.fill(FALSE);
    gameState.setBit(Init);
    gameState.setBit(Playing);

    score (2*(level->get())+(2*numSnakes)+(2*numBalls)+(2*skill));

    level->up();
    level->create(Banner);
    pix->initRoomPixmap();
    repaint();

    QTimer::singleShot( 2000, this, SLOT(showRoom()) );
}

/* this slot is called by the progressBar  when value() == 0
or by a compuSnake wich manages to exit */
void Rattler::restartTimer()
{
    timerHasRunOut = TRUE;
    timerCount = 0;
    emit rewind();

    if ( board->isEmpty(NORTH_GATE) )
	closeGate(NORTH_GATE);
    basket->newApples();
}

void Rattler::speedUp()
{
    leaving = TRUE;
    stop();
    start( 30 );
}

void Rattler::resetFlags()
{
    trys = 2;
    check  = 1;
    points = 0;
}

void Rattler::showRoom()
{
    level->create(Room);
    pix->initRoomPixmap();
    init(TRUE);
    repaint();
    QTimer::singleShot( 1000, this, SLOT(run()) );
}

void Rattler::init(bool play)
{
    leaving = FALSE;
    timerHasRunOut = FALSE;
    timerCount = 0;
    emit rewind();

    emit setTrys(trys);
    emit setPoints(points);

    basket->clear();
    basket->newApples();
    restartBalls(play);
    restartComputerSnakes(play);
    if(play) samy->init();

}

void Rattler::run()
{
    direction = N;
    gameState.toggleBit(Init);
    start();
}

void Rattler::start()
{
    gameTimer = startTimer( speed [skill] );
}

void Rattler::start(int t)
{
    gameTimer = startTimer(t);
}

void Rattler::stop()
{
    killTimers ();
}

void Rattler::restartComputerSnakes(bool play)
{
    if( !computerSnakes->isEmpty())
	computerSnakes->clear();

    int i  = (play == FALSE && numSnakes == 0 ? 1 : numSnakes);

    for (int x = 0; x < i; x++) {
	CompuSnake *as = new CompuSnake(board, pix);
	connect( as, SIGNAL(closeGate(int)), this, SLOT(closeGate(int)));
	connect( as, SIGNAL(restartTimer()), this, SLOT(restartTimer()));
	connect( as, SIGNAL(score(bool, int)), this, SLOT(scoring(bool,int)));
	connect( as, SIGNAL(killed()), this, SLOT(killedComputerSnake()));
	computerSnakes->append(as);
    }
}

void Rattler::restartBalls(bool play)
{
    if( !balls->isEmpty())
	balls->clear();

    int i = (play == FALSE && numBalls == 0 ? 1 : numBalls);

    for (int x = 0; x < i; x++) {
	Ball *b = new Ball(board, pix);
	balls->append(b);
    }
}

void Rattler::setBalls(int i)
{
    Ball *b;
    numBalls = i;
    int count = balls->count();

    if (gameState.testBit(Playing) || gameState.testBit(Demo)) {
	if ( i > count) {
	    while ( i > count) {
		Ball *b = new Ball(board, pix);
		balls->append(b);
		i--;
	    }
	}
	else if (i < count) {
	    while (i < count) {
		b = balls->getLast();
		b->zero();
		balls->removeLast();
		i++;
	    }
	}
    }
}

void Rattler::setCompuSnakes(int i)
{
    CompuSnake *cs;
    numSnakes = i;
    int count = computerSnakes->count();

    if (gameState.testBit(Playing) || gameState.testBit(Demo)) {
	if ( i > count) {
	    while ( i > count) {
		CompuSnake *as = new CompuSnake(board, pix);
		connect( as, SIGNAL(closeGate(int)), this, SLOT(closeGate(int)));
		connect( as, SIGNAL(restartTimer()), this, SLOT(restartTimer()));
		connect( as, SIGNAL(score(bool, int)), this, SLOT(scoring(bool,int)));
		connect( as, SIGNAL(killed()), this, SLOT(killedComputerSnake()));
		computerSnakes->append(as);
		i--;
	    }
	}
	else if (i < count) {
	    while (i < count) {
		cs = computerSnakes->getLast();
		cs->zero();
		computerSnakes->removeLast();
		i++;
	    }
	}
    }
}

void Rattler::setSkill(int i)
{
    skill = i;
    if (gameState.testBit(Playing) || gameState.testBit(Demo)) {
	stop();
	start();
    }
}

void Rattler::setRoom(int i)
{
    room = i;
}

void Rattler::reloadRoomPixmap()
{
    pix->initbackPixmaps();
    pix->initRoomPixmap();
    demo();
}
