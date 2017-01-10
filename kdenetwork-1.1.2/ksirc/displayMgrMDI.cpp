#include "displayMgrMDI.h"

#include "KMDIMgr.h"
#include "displayMgr.h"
#include "toplevel.h"

#include <ktopwidget.h>

DisplayMgrMDI::DisplayMgrMDI(KTopLevelWidget *ktw)
  : DisplayMgr()
{
  mgr = new("KMDIMgrBase") KMDIMgrBase(ktw, "MDI Manager");
  mgr->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);
  ktw->setView(mgr, TRUE);
  
}

DisplayMgrMDI::~DisplayMgrMDI(){
  delete mgr;  // Remove the manager
}

void DisplayMgrMDI::newTopLevel(QWidget *w, bool show){
  
  w->recreate(mgr, 0x0, QPoint(0,0));

  KMDIWindow *mdiWnd=mgr->addWindow(w,MDI_SHOW,NULL);

  if(w->inherits("KSircTopLevel") == TRUE){
      QObject::connect(mdiWnd, SIGNAL(selected(KMDIWindow *)),
	    w, SLOT(gotMDIFocus(KMDIWindow *)));
  }

  mdiWnd->setCaption("<No Caption Set>");
  
  if(show == TRUE)
    mdiWnd->show();
  else
    mdiWnd->hide();
}

void DisplayMgrMDI::removeTopLevel(QWidget *w){

  KMDIWindow *km;
  if(w->inherits("KSircTopLevel") == TRUE){
    KSircTopLevel *kstl = (KSircTopLevel *) w;
    km = mgr->getWindowByName(kstl->original_name());
  }
  else{
    km = mgr->getWindowByName(w->name());
  }
  if(km != 0x0){
    mgr->removeWindow(km);
    km->hide();
  }
  else
  warning("Could not find widget in removeTopLevel");
}

void DisplayMgrMDI::show(QWidget *w){

  KMDIWindow *km;
  if(w->inherits("KSircTopLevel") == TRUE){
    KSircTopLevel *kstl = (KSircTopLevel *) w;
    km = mgr->getWindowByName(kstl->original_name());
  }
  else{
    km = mgr->getWindowByName(w->name());
  }
  if(km != 0)
    km->show();
  else
  warning("Could not find widget in show");

}

void DisplayMgrMDI::raise(QWidget *w){

  KMDIWindow *km;
  if(w->inherits("KSircTopLevel") == TRUE){
    KSircTopLevel *kstl = (KSircTopLevel *) w;
    km = mgr->getWindowByName(kstl->original_name());
  }
  else{
    km = mgr->getWindowByName(w->name());
  }
  if(km != 0){
    km->raise();
    km->slotRestore();
  }
  else
  warning("Could not find widget in raise");

}


void DisplayMgrMDI::setCaption(QWidget *w, const char *cap){

  KMDIWindow *km;
  if(w->inherits("KSircTopLevel") == TRUE){
    KSircTopLevel *kstl = (KSircTopLevel *) w;
    km = mgr->getWindowByName(kstl->original_name());
  }
  else{
    km = mgr->getWindowByName(w->name());
  }
  if(km != 0)
    km->setCaption(cap);
  else
  warning("Could not find widget in setCaption");

}
#include "displayMgrMDI.moc"
