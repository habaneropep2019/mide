#include "displayMgr.h"

#include <qobject.h>


DisplayMgr::DisplayMgr()
{
}

DisplayMgr::~DisplayMgr()
{
}

void DisplayMgr::newTopLevel(QWidget *, bool show)
{
  warning("Display Manger: newTopLevel called");
}

void DisplayMgr::removeTopLevel(QWidget *)
{
  warning("Display Manger: removeTopLevel called");
}

void DisplayMgr::show(QWidget *)
{
  warning("Display Manger: show called");
}

void DisplayMgr::raise(QWidget *)
{
  warning("Display Manger: raise called");
}

void DisplayMgr::setCaption(QWidget *w, const char *)
{
  warning("Display Manger: setCaption called");
}

#include "displayMgr.moc"
