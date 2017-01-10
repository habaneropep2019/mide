#include "view.h"
#include "QwViewport.h"
#include "sidebar.h"

View::View(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  c= 0;
  s= 0;
}

View::~View() 
{

}

void View::updateRects()
{
  if (c) {
    c->move(0, 0);
    if (width() > s->width())
      c->resize(width() - s->width(), height());
  }
  if (s) {
    s->move(width() - s->width(), 0);
    s->resize(s->width(), height());
  }
}

void View::resizeEvent(QResizeEvent *)
{
  updateRects();
}

#include "view.moc"

