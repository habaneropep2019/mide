#ifndef DISPLAYMGR_H
#define DISPLAYMGR_H

#include <qwidget.h>

class DisplayMgr {
public:
  DisplayMgr();
  virtual ~DisplayMgr();

  virtual void newTopLevel(QWidget *, bool show = FALSE);
  virtual void removeTopLevel(QWidget *);

  virtual void show(QWidget *);

  virtual void raise(QWidget *);

  virtual void setCaption(QWidget *, const char *);
  
};


#endif

