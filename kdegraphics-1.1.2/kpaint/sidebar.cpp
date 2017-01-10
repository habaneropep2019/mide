#include <qwmatrix.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <kcolorbtn.h>
#include "canvas.h"
#include "sidebar.h"

SideBar::SideBar(Canvas *c, QWidget *parent, const char *name) 
  : QFrame(parent, name)
{
  if (style() == MotifStyle) {        
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);
  }
  else {
    setFrameStyle(QFrame::WinPanel | QFrame::Raised);
  }

  QVBoxLayout *top= new QVBoxLayout (this, 4);

  nav= new NavWidget(this, "navwidget");

  nav->setPixmap(c->pixmap());

  connect(c, SIGNAL(pixmapChanged(QPixmap *)),
	  nav, SLOT(setPixmap(QPixmap *)));

  top->addWidget(nav, 1);
  
  // The colour thingies
  KColorButton *lmbColourBtn= new KColorButton(red, this, "lmbColour");
  KColorButton *rmbColourBtn= new KColorButton(green, this, "rmbColour");

  lmbColourBtn->setFixedSize(30, 30);
  rmbColourBtn->setFixedSize(30, 30);
  
  connect(lmbColourBtn, SIGNAL(changed(const QColor &)),
	  this, SIGNAL(lmbColourChanged(const QColor &)));
  connect(rmbColourBtn, SIGNAL(changed(const QColor &)),
	  this, SIGNAL(rmbColourChanged(const QColor &)));

  QGridLayout *colourGrid= new QGridLayout(2, 2);
  top->addLayout(colourGrid);
  colourGrid->addWidget(lmbColourBtn, 0, 0);
  colourGrid->addWidget(rmbColourBtn, 1, 1);

  // The palette thingy
  top->addStretch(2);

  // Activate the layout managers
  top->activate();
}

SideBar::~SideBar() 
{

}

void SideBar::pixmapChanged()
{
  nav->pixmapChanged();
}


#include "sidebar.moc"
