#include <qpainter.h>
#include <qlistbox.h>

#include "linelistitem.h"


void lineListItem::paint( QPainter *p ){
  QPen pen = p->pen();
  p->setPen(colour);
  p->drawLine(0,1,lb->width(),1);
  p->setPen(pen);
}
