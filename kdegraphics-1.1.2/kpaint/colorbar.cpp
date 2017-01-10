

#define COLOR_PATCH_WIDTH 13
#define WIDGET_HEIGHT 19 /* 15 + 2*2 */

#include "colorbar.moc"

#include <qpainter.h>
#include <qpixmap.h>
//#include <qbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>


ColorButton::ColorButton(QWidget *parent, const char *name)
  : QToolButton(parent, name)
{}


void
ColorButton::drawButtonLabel(QPainter *p)
{
  // I want to have the background color
  QColor c = backgroundColor();
  p->fillRect(2, 2, width()-6, height()-6, c);
}


/* ---------- ColorBar ------------- */
ColorBar::ColorBar(QWidget *parent, int fixheight, const char *name)
  : QFrame(parent, name),
    lastid(-1),
    bcount(0)
{
  setFixedHeight( (-1==fixheight)?WIDGET_HEIGHT:fixheight );
  // debug("set height %i", height());
  setFrameStyle( WinPanel | Sunken );
  setLineWidth( 1 );

  //  buttonList = new ButtonList(this);
  buttonList.setAutoDelete( true );

  buttonGroup = new QButtonGroup();
  buttonGroup->setExclusive( true );
  connect(buttonGroup, SIGNAL(clicked(int)),
	  SLOT(slotClicked(int)));

  //  layout = new QHBoxLayout(this);
  //  layout->activate();
}

ColorBar::~ColorBar()
{
  delete buttonGroup;
}

void 
ColorBar::addColor(const QColor &color)
{
  addColor( color.rgb());
  /*  
  QRgb *tmp = new QRgb;
  *tmp = color.rgb();
  rgbList.append(tmp);
  */
}

void 
ColorBar::addColor(const QRgb rgb)
{
  const int bsize = height() - 2*frameWidth();
  int tmp;
  //  debug("button width %i", bsize);

  QButton *b = new ColorButton(this);
  b->setBackgroundColor( QColor( rgb ));
  b->setFixedSize( bsize, bsize );
  //  layout->addWidget( b );
  //  buttonList.append( b );
  tmp = buttonGroup->insert( b );
  lastid = (lastid>tmp)?lastid:tmp;
  bcount++;

  // schiebe den Knopf auf dem Rahmen an die richtige Stelle 
  b->move( (bcount-1)*bsize + frameWidth(), frameWidth());


  /*
  QRgb *tmp = new QRgb;
  *tmp = rgb;
  rgbList.append(tmp);
  */
}

/*
void 
ColorBar::paintEvent(QPaintEvent *)
{
  QPainter paint(this);
  int x;
  QRgb *rgb;
  QPixmap pm((int)COLOR_PATCH_WIDTH, (int)COLOR_PATCH_WIDTH);
  QColor col;

  drawFrame (&paint);
  // paint for each color a rectangular with that color. 
  for ( x=2, rgb = rgbList.first();
	rgb != NULL;
	x+=COLOR_PATCH_WIDTH+2, rgb = rgbList.next() ) {
    col.setRgb( *rgb );
    pm.fill( col );
    paint.drawPixmap(x, 2, pm);
  }

}
*/

/*
void
ColorBar::drawContents(QPainter *p)
{
  int id;
  QButton *b;

  for (id=0; id<=lastid ; id++) {
    b = buttonGroup->find(id);



  }
}
*/

void 
ColorBar::slotClicked(int id)
{
  // debug ("clicked id %i", id);
  ColorButton *b = (ColorButton*)buttonGroup->find(id);
  b->setOn(true);
  emit selected(b->backgroundColor().rgb());
}
