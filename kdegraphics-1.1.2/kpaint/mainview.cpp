

#include "mainview.moc"

#include <qlayout.h>

#include "view.h"
#include "QwViewport.h"
#include "canvas.h"
#include "sidebar.h"
#include "colorbar.h"

MainView::MainView(QWidget *parent, const char *name)
  : QDialog(parent,name)
{
  int w, h; 
  w= 300;
  h= 200;

  lm = new QBoxLayout(this, QBoxLayout::TopToBottom);

  view= new View(this, "view container");
  v= new QwViewport(view);
  c= new Canvas(w, h, v->portHole());

  lm->addWidget(view);

  side= new SideBar(c, view, "sidebar");
  connect(c, SIGNAL(modified()), side, SLOT(pixmapChanged()));

  view->c= v;
  view->s= side;

  dcb = new ColorBar(this);
  lm->addWidget(dcb);

}
