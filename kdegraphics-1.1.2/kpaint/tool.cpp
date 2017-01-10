// $Id: tool.cpp,v 1.8 1998/10/18 20:00:07 habenich Exp $

#include <kdebug.h>
#include <stdio.h>
#include <assert.h>
#include <qpixmap.h>
#include "tool.h"

Tool::Tool()
{
  active= false;
  canvas= 0;
  props= 0;
  tooltip= 0;
}

Tool::Tool(const char *toolname)
  : name(toolname)
{
  active= false;
  canvas= 0;
  props= 0;
  tooltip= 0;
}


int Tool::getPages()
{
  return props;
}

void Tool::activate(Canvas *c)
{
  assert(!c->isActive());
  canvas= c;
  active= true;
  activating();
}

void Tool::deactivate()
{
  assert(active);
  deactivating();
  active= false;
  canvas= 0;
}

bool Tool::isActive()
{
  return active;
}

void Tool::setLeftPen(QPen &p)
{
  KDEBUG(KDEBUG_INFO, 3000, "Tool::setLeftPen()\n");
  leftpen= p;
}

void Tool::setLeftBrush(QBrush &b)
{
  KDEBUG(KDEBUG_INFO, 3000, "Tool::setLeftBrush()\n");
  leftbrush= b;
}

void Tool::setRightPen(QPen &p)
{
  KDEBUG(KDEBUG_INFO, 3000, "Tool::setRightPen()\n");
  rightpen= p;
}

void Tool::setRightBrush(QBrush &b)
{
  KDEBUG(KDEBUG_INFO, 3000, "Tool::setRightBrush()\n");
  rightbrush= b;
}

QPixmap *Tool::pixmap()
{
  return 0;
}

#include "tool.moc"
