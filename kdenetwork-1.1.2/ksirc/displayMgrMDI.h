#ifndef DISPLAYMGRMDI_H
#define DISPLAYMGRMDI_H

#include <ktopwidget.h>

#include "displayMgr.h"
#include "KMDIMgr.h"
#include "toplevel.h"

class DisplayMgrMDI : public DisplayMgr {
public:
  DisplayMgrMDI(KTopLevelWidget *ktw);
  virtual ~DisplayMgrMDI();

  virtual void newTopLevel(QWidget *, bool show = FALSE);
  virtual void removeTopLevel(QWidget *);

  virtual void show(QWidget *);
  virtual void raise(QWidget *);

  virtual void setCaption(QWidget *, const char *);

  const KMDIMgrBase *getMGR(){ return mgr; }

private:
  KMDIMgrBase *mgr;
  
};


#endif

