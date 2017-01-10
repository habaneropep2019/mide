#ifndef DISPLAYMGRSDI_H
#define DISPLAYMGRSDI_H

#include "displayMgr.h"

#include "qwidget.h"

class DisplayMgrSDI : public DisplayMgr {
public:
  DisplayMgrSDI();
  virtual ~DisplayMgrSDI();

  virtual void newTopLevel(QWidget *, bool show = FALSE);
  virtual void removeTopLevel(QWidget *);

  virtual void show(QWidget *);
  virtual void raise(QWidget *);

  virtual void setCaption(QWidget *, const char *);

private:
};


#endif

