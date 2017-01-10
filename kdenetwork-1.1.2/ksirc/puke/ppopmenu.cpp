#include "ppopmenu.h"

PObject *createWidget(CreateArgs &ca)
{
  PPopupMenu *pm = new("PPopupMenu") PPopupMenu(ca.parent);
  QPopupMenu *qpm;
  if(ca.fetchedObj != 0 && ca.fetchedObj->inherits("QPopupMenu") == TRUE){
    qpm= (QPopupMenu *) ca.fetchedObj;
    pm->setDeleteAble(FALSE);
  }
  else if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    qpm = new("QPopupMenu") QPopupMenu((QWidget *) ca.parent->widget());
  else
    qpm = new("QPopupMenu") QPopupMenu();
  pm->setWidget(qpm);
  pm->setWidgetId(ca.pwI);
  return pm;
}


PPopupMenu::PPopupMenu(PObject *parent)
  : PTableView(parent)
{
  //  debug("PLineEdit PLineEdit called");
  menu = 0;
  setWidget(menu);
  pmd = new("PMenuData") PMenuData(this);
}

PPopupMenu::~PPopupMenu()
{
  //  debug("PLineEdit: in destructor"); 
/*  delete widget();     // Delete the frame
  menu = 0;          // Set it to 0
  setWidget(menu); // Now set all widget() calls to 0.
*/
  delete pmd;
}

void PPopupMenu::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
    /*
  case PUKE_LINED_SET_MAXLENGTH:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    widget()->setMaxLength(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->maxLength();
    emit outputMessage(fd, &pmRet);
    break;
    */
  case PUKE_POPUPMENU_POPUP_CURRENT:
    if(widget() == 0){
      debug("PPopupMenu: No Widget set");
      return;
    }
    
    widget()->popup(QCursor::pos(), 0);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 1;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;

  default:
    if(pmd->menuMessageHandler(fd, pm) == FALSE) // Call pmd's even filter
      PTableView::messageHandler(fd, pm);
  }
}

void PPopupMenu::setWidget(QObject *_menu)
{
  if(_menu != 0 && _menu->inherits("QPopupMenu") == FALSE)
    throw(errorInvalidSet(_menu, className()));

  menu = (QPopupMenu *) _menu;
  if(menu != 0x0){
    connect(menu, SIGNAL(activated(int)),
            this, SLOT(got_activated(int)));
  }
  PTableView::setWidget(menu);
}


QPopupMenu *PPopupMenu::widget()
{
  return menu;
}

void PPopupMenu::got_activated(int itemId){
  warning("Item got activated: %d", itemId);
  
  widgetId wI;
  PukeMessage pmRet;

  wI = widgetIden();
  pmRet.iCommand = PUKE_POPUPMENU_ACTIVATED_ACK;
  pmRet.iWinId = wI.iWinId;
  pmRet.iArg = itemId;
  pmRet.cArg = 0;
  emit outputMessage(wI.fd, &pmRet);

}

#include "ppopmenu.moc"

