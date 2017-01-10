#include <stdlib.h>
#include <stdio.h>
#include <qdatetm.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qpopmenu.h>
#include <qmenubar.h>

#include <qapp.h>
#include <qwidget.h>
#include <qkeycode.h>
#include <qpainter.h>

#include "example.h"

#define IMG_BACKGROUND "sprite/bg.ppm"
#define WIDTH 500
#define HEIGHT 400
#define IMG_SPRITE_DATA "sprite/sprite%d.ppm"
#define IMG_SPRITE_MASK "sprite/sprite%d.pbm"
#define NR_ROTS 32
#define BOUNDS_ACTION Bounce

static bool showtext=FALSE;
static bool showlines=FALSE;
static bool btext=TRUE;
static bool dsprite=TRUE;
static bool dpoly=FALSE;
static bool showredraws=FALSE;
static int refresh_delay=33;
static int iterations=0;
static int bouncers=3;
static int speed=20;

MySpriteField::MySpriteField(const char* imagefile, int w, int h, int chunksize, int maxclusters) :
    QwImageSpriteField(imagefile,w,h,chunksize,maxclusters),
    textarea(50,50,w-2*50,h-2*50)
{
}

void MySpriteField::resize(int width, int height)
{
    textarea=QRect(50,50,width-2*50,height-2*50);
    QwImageSpriteField::resize(width,height);
}

void MySpriteField::drawForeground(QPainter& painter, const QRect& r)
{
    if (showredraws) {
	painter.setPen(blue);
	painter.setBrush(NoBrush);
	painter.drawRect(r);
    }
    if (showlines) {
	int x2=width()-1;
	int y2=height()-1;
	painter.setPen(red);
	painter.drawLine(0,0,x2,y2);
	painter.drawLine(0,y2,x2,0);
	painter.setPen(green);
	painter.drawLine(x2/2,0,x2,y2/2);
	painter.drawLine(x2/2,y2,0,y2/2);
	painter.drawLine(x2/2,y2,x2,y2/2);
	painter.drawLine(x2/2,0,0,y2/2);
    }
    if (showtext) {
	static const char* text=
	    "This is an example of the QwSpritefield class set.  "
	    "The cloudy image is drawn as the background, "
	    "and this text is drawn as the foreground.  "
	    "The bouncing things are sprites.\n"
	    "\nOptions:\n"
	    "  -help      Show this text (slower)\n"
	    "  -btext     Disable bouncing text\n"
	    "  -dsprite   Disable drawn sprite\n"
	    "  -dpoly     Show polygon\n"
	    "  -bounce n  Gives n bouncers (default 3)\n"
	    "  -lines     Draws lines on foreground (default off)\n"
	    "  -redraws   Draws rectangles showing redraws (default off)\n"
	    "  -delay n   Changes redraw delay to n milliseconds (default 15)\n"
	    "  -iter n    Exit after n frames\n"
	    "  -sb        Turn on smooth-scrolling scrollbars";
	textarea.moveBy(1,1);
	painter.setFont(QFont("Courier",12));
	painter.setPen(white);
	painter.drawText(textarea,AlignVCenter|WordBreak,text);
	textarea.moveBy(-1,-1);
	painter.setPen(black);
	painter.drawText(textarea,AlignVCenter|WordBreak,text);
    }
}

static int frames=0;
static QTime timer;

Example::Example(bool scrbars, MySpriteField& sfield, QWidget* parent,
	const char* name, WFlags f) :
    QWidget(parent,name?name:"example",f),
    field(sfield),
    view(scrbars ? (QWidget*)new QwScrollingSpriteFieldView(&field,this)
	: (QWidget*)new QwSpriteFieldView(&field,this)
	),
    sprite(0),
    textsprite("QwTextSprite",QFont("Helvetica",18)),
    scrollbars(scrbars),
    count(0)
{
    setMouseTracking(TRUE);

    view->move(0,0);
    textsprite.setColor(red);
    if (!btext) textsprite.hide();
    QPixmap pm(63,63);
    pm.fill(black);

    QPainter p;
    p.begin(&pm);
    for (int i=0; i<50; i++) {
	p.setPen(QColor(rand()%255,rand()%255,rand()%255));
	p.drawLine(rand()%62,rand()%62,rand()%62,rand()%62);
    }
    p.setPen(white);
    p.drawText(0,0,63,63,AlignCenter,"DRAWN\nSPRITE");
    pm.setMask(pm.createHeuristicMask());
    p.end();

    QPoint hs(31,31);
    QList<QPixmap> lpm; lpm.append(&pm);
    QList<QPoint> lhs; lhs.append(&hs);
    drawnsprite = new QwSprite( new QwSpritePixmapSequence(lpm,lhs) );

    // Diamond
    QPointArray pa(4);
    pa[0] = QPoint(0,-150);
    pa[1] = QPoint(+150,0);
    pa[2] = QPoint(0,+150);
    pa[3] = QPoint(-150,0);
    polygon = new QwPolygon();
    polygon->setBrush(DiagCrossPattern);
    polygon->setBrush(QBrush(red,DiagCrossPattern));
    polygon->setPoints(pa);

    images=new QwSpritePixmapSequence(
	IMG_SPRITE_DATA, IMG_SPRITE_MASK, NR_ROTS);

    setCount(bouncers);

    resize(WIDTH,HEIGHT);

    // Since it is virtual, ours won't be called in the constructor
    refresh();
    resizeEvent(0);
    startTimer(refresh_delay);
    timer.start();
}

void Example::setCount(int n)
{
    QwRealMobileSprite** newsprite;
    if ( n ) {
	newsprite = new QwRealMobileSprite*[n];
	int i;

	// copy old
	for (i=0; i<QMIN(count,n); i++) {
	    newsprite[i] = sprite[i];
	}
	// make new
	for ( ; i<n; i++) {
	    newsprite[i]=new QwRealMobileSprite(images);
	    newsprite[i]->setBoundsAction(QwRealMobileSprite::BOUNDS_ACTION);
	    newsprite[i]->frame(rand()%NR_ROTS);
	    newsprite[i]->setVelocity( // broad distribution of speeds
		(double)(rand()%256)*(rand()%256)/10000,
		(double)(rand()%256)*(rand()%256)/10000);
	    newsprite[i]->z(rand()%1024-512);
	}
	// delete old
	for ( ; i<count; i++) {
	    delete sprite[i];
	}
    } else {
	newsprite = 0;
    }
    count = n;
    delete sprite;
    sprite = newsprite;
}

void Example::makeSlave()
{
    QWidget* slave =
	new QwScrollingSpriteFieldView(&field,0,0,WDestructiveClose);
    slave->resize(width()/2,height()/2);
    slave->show();
}

void Example::makePolygon()
{
    // Diamond
    QPointArray pa(4);
    pa[0] = QPoint(0,-50);
    pa[1] = QPoint(+50,0);
    pa[2] = QPoint(0,+50);
    pa[3] = QPoint(-50,0);
    QwPolygon *p= new QwPolygon();
    p->setBrush(blue);
    p->setPoints(pa);
    p->moveTo(rand()%field.width(),
	      rand()%field.height());
}

void Example::makeEllipse()
{
    QwEllipse *e = new QwEllipse();
    e->setBrush(green);
    e->moveTo(rand()%field.width(),
	      rand()%field.height());
}

void Example::makeRectangle()
{
    QwRectangle *r = new QwRectangle();
    r->setBrush(yellow);
    r->moveTo(rand()%field.width(),
	      rand()%field.height());
}

void Example::refresh()
{
    textsprite.visible(!!btext);
    drawnsprite->visible(!!dsprite);
    polygon->visible(!!dpoly);
    if ( count != bouncers )
	setCount(bouncers);
    frames=0;
    timer.start();
    killTimers();
    startTimer(refresh_delay);
    emit refreshSlaves();
    view->repaint(FALSE);
}

void Example::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    view->resize(width(),height());
    if (!scrollbars)
	field.resize(width(),height());
    for (int i=0; i<count; i++) {
	sprite[i]->adoptSpritefieldBounds();
    }
}

void Example::mouseMoveEvent(QMouseEvent* me)
{
    mpos = me->pos();
}

void Example::timerEvent(QTimerEvent*)
{
    if (iterations)
	if (!--iterations)
	    qApp->quit();
    int tx=0,ty=0;
    static int t=0; t++; // tick tick tick
    for (int i=0; i<count; i++) {
#ifdef SCALE_WITH_DELAY
	int rd = (refresh_delay ? refresh_delay : 1);
	sprite[i]->forward(double(speed*rd)/500.0);
	if (((t+count)%(200/rd))==0) // Occasionally
#else
	sprite[i]->forward(speed/20.0);
	if (((t+count)%8)==0) // Occasionally
#endif
	    sprite[i]->frame((sprite[i]->frame()+1)%NR_ROTS);
	tx+=sprite[i]->x();
	ty+=sprite[i]->y();
    }
    QString text;
    text.sprintf("QwTextSprite (%d,%d)",tx/count, ty/count);
    if (btext) {
	textsprite.setText(text);
	textsprite.moveTo(tx/count, ty/count);
    }
    if (dsprite)
	drawnsprite->moveTo(width()-tx/count, height()-ty/count);
    if (dpoly)
	polygon->moveTo(mpos);

    field.update();
    qApp->syncX();

    frames++;
    if ( (frames-10)%32==0 ) {
	int seconds = timer.elapsed();
	if ( seconds ) {
	    QString msg;
	    msg.sprintf("%0.1lf fps",(double)frames*1000/seconds);
	    emit status(msg);
	}
    }
}

MyPopupMenu::MyPopupMenu()
{
    setCheckable(TRUE);
    connect(this, SIGNAL(activated(int)), this, SLOT(checkCheck(int)));
}
MyPopupMenu::~MyPopupMenu()
{
    for (uint i=0; i<count(); i++) {
	bools.remove(idAt( i ));
	ints.remove(idAt( i ));
	values.remove(idAt( i ));
    }
}

int MyPopupMenu::insertCheckItem(const char* text, bool* var, int accel)
{
    int id = insertItem( text, this, SLOT(dummy()), accel );
    if ( var ) {
	setItemChecked( id, *var ); 
	bools.insert(id, var);
    }
    return id;
}

int MyPopupMenu::insertRadioItem(const char* text, int* var, int value, int accel)
{
    int id = insertItem( text, this, SLOT(dummy()), accel );
    setItemChecked( id, *var==value ); 
    ints.insert(id, var);
    int *i = new int; *i = value;
    values.insert(id, i);
    return id;
}

void MyPopupMenu::dummy()
{
}

void MyPopupMenu::checkCheck(int id)
{
    bool* var = bools.find(id);

    if ( var ) {
	bool check = !isItemChecked(id);
	setItemChecked(id, check);
	*var = check;
	emit variableChanged(var);
    } else {
	int* var = ints.find(id);
	int* val = values.find(id);
	if ( var && val ) {
	    *var = *val;
	    int ind=indexOf(id);
	    // go back to start/separator
	    while (idAt(ind)!=-1) {
		ind--;
	    }
	    ind++;
	    // go forward to end/separator
	    int i;
	    while ((i=idAt(ind))!=-1) {
		setItemChecked( i, i==id );
		ind++;
	    }
	    emit variableChanged(var);
	}
    }
}
QIntDict<bool> MyPopupMenu::bools;
QIntDict<int> MyPopupMenu::ints;
QIntDict<int> MyPopupMenu::values;

int main(int argc, char** argv)
{
    bool timeit = FALSE;
    QApplication app(argc,argv);

    bool scrollbars=FALSE;

    for (int arg=1; arg<argc; arg++) {
	if (0==strcmp(argv[arg],"-bounce")) {
	    bouncers=atoi(argv[++arg]);
	} else if (0==strcmp(argv[arg],"-help") ||
		   0==strcmp(argv[arg],"--help")) {
	    showtext=FALSE;
	} else if (0==strcmp(argv[arg],"-redraws")) {
	    showredraws=TRUE;
	} else if (0==strcmp(argv[arg],"-lines")) {
	    showlines=TRUE;
	} else if (0==strcmp(argv[arg],"-btext")) {
	    btext=FALSE;
	} else if (0==strcmp(argv[arg],"-dsprite")) {
	    dsprite=FALSE;
	} else if (0==strcmp(argv[arg],"-dpoly")) {
	    dpoly=!dpoly;
	} else if (0==strcmp(argv[arg],"-delay")) {
	    refresh_delay=atoi(argv[++arg]);
	} else if (0==strcmp(argv[arg],"-sb")) {
	    scrollbars=TRUE;
	} else if (0==strcmp(argv[arg],"-noopt")) {
	    QPixmap::setDefaultOptimization( QPixmap::NoOptim );
	} else if (0==strcmp(argv[arg],"-bestopt")) {
	    QPixmap::setDefaultOptimization( QPixmap::BestOptim );
#ifdef _WS_WIN_
	} else if (0==strcmp(argv[arg],"-bsm")) {
	    extern bool qt_bitblt_bsm;
	    qt_bitblt_bsm=TRUE;
#endif
	} else if (0==strcmp(argv[arg],"-iter")) {
	    iterations=atoi(argv[++arg]);
	    timeit = TRUE;
	} else {
	    warning("Bad param %s", argv[arg]);
	}
    }

    QMainWindow m;
    MySpriteField field(IMG_BACKGROUND,scrollbars ? WIDTH*3 : WIDTH,
		scrollbars ? HEIGHT*3 : HEIGHT);
    Example example(scrollbars,field,&m);

    QMenuBar* menu = m.menuBar();

    QPopupMenu* file = new QPopupMenu;
    file->insertItem("New view", &example, SLOT(makeSlave()), CTRL+Key_N);
    file->insertSeparator();
    file->insertItem("Quit", qApp, SLOT(quit()), CTRL+Key_Q);
    menu->insertItem("&File", file);

    QPopupMenu* edit = new QPopupMenu;
    edit->insertItem("New polygon", &example, SLOT(makePolygon()));
    edit->insertItem("New ellipse", &example, SLOT(makeEllipse()));
    edit->insertItem("New rectangle", &example, SLOT(makeRectangle()));
    menu->insertItem("&Edit", edit);

    MyPopupMenu* options = new MyPopupMenu;
    options->insertCheckItem("Show help text", &showtext, CTRL+Key_H);
    options->insertCheckItem("Show bouncing text", &btext, CTRL+Key_T);
    options->insertCheckItem("Show polygon", &dpoly, CTRL+Key_P);
    options->insertCheckItem("Show drawn sprite", &dsprite, CTRL+Key_D);
    options->insertCheckItem("Show redraw areas", &showredraws, CTRL+Key_A);
    options->insertCheckItem("Show foreground lines", &showlines, CTRL+Key_L);
    options->insertSeparator();
    options->insertRadioItem("1 bouncer", &bouncers, 1);
    options->insertRadioItem("3 bouncers", &bouncers, 3);
    options->insertRadioItem("10 bouncers", &bouncers, 10);
    options->insertRadioItem("30 bouncers", &bouncers, 30);
    options->insertRadioItem("100 bouncers", &bouncers, 100);
    options->insertRadioItem("1000 bouncers", &bouncers, 1000);
    options->insertSeparator();
    options->insertRadioItem("No delay", &refresh_delay, 0);
    options->insertRadioItem("500 fps", &refresh_delay, 2);
    options->insertRadioItem("100 fps", &refresh_delay, 10);
    options->insertRadioItem("72 fps", &refresh_delay, 14);
    options->insertRadioItem("30 fps", &refresh_delay, 33);
    options->insertRadioItem("10 fps", &refresh_delay, 100);
    options->insertRadioItem("5 fps", &refresh_delay, 200);
    options->insertSeparator();
    options->insertRadioItem("1/10 speed", &speed, 2);
    options->insertRadioItem("1/2 speed", &speed, 10);
    options->insertRadioItem("1x speed", &speed, 20);
    options->insertRadioItem("2x speed", &speed, 40);
    options->insertRadioItem("5x speed", &speed, 100);
    menu->insertItem("&Options",options);
    m.statusBar();

    QObject::connect(options, SIGNAL(variableChanged(bool*)), &example, SLOT(refresh()));
    QObject::connect(options, SIGNAL(variableChanged(int*)), &example, SLOT(refresh()));
    QObject::connect(&example, SIGNAL(status(const char*)), m.statusBar(), SLOT(message(const char*)));
    m.setCentralWidget(&example);
    app.setMainWidget(&m);
    m.show();

    QTime t;
    t.start();
    app.exec();
    if ( timeit )
	debug("%dms",t.elapsed());
    return 0;
}


