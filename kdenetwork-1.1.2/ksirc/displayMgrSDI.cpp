#include "displayMgrSDI.h"

#include "displayMgr.h"
#include "qwidget.h"

DisplayMgrSDI::DisplayMgrSDI()
  : DisplayMgr()
{
}

DisplayMgrSDI::~DisplayMgrSDI(){
}

void DisplayMgrSDI::newTopLevel(QWidget *w, bool show){

  if(show == TRUE)
    w->show();
}

void DisplayMgrSDI::removeTopLevel(QWidget *w){
}

void DisplayMgrSDI::show(QWidget *w){
  w->show();
}

void DisplayMgrSDI::raise(QWidget *w){
  w->show();
  w->raise();
}


void DisplayMgrSDI::setCaption(QWidget *w, const char *cap){
  w->setCaption(cap);
}

#include "displayMgrSDI.moc"
