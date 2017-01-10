#include "plistbox.h"

PObject *createWidget(CreateArgs &ca) /*FOLD00*/
{
  PListBox *plb = new("PListBox") PListBox(ca.parent);
  QListBox *lb;
  if(ca.fetchedObj != 0 && ca.fetchedObj->inherits("QListBox") == TRUE){
    lb = (QListBox *) ca.fetchedObj;
    plb->setDeleteAble(FALSE);
  }
  else if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    lb = new("QListBox") QListBox((QWidget *) ca.parent->widget());
  else
    lb = new("QListBox") QListBox();
  plb->setWidget(lb);
  plb->setWidgetId(ca.pwI);
  return plb;
}


PListBox::PListBox(PObject *parent) /*FOLD00*/
  : PTableView(parent)
{
  //  debug("PListBox PListBox called");
  lb = 0;
  setWidget(lb);
}

PListBox::~PListBox() /*FOLD00*/
{
  //  debug("PListBox: in destructor");
  /*
  delete widget();     // Delete the frame
  lb=0;          // Set it to 0
  setWidget(lb); // Now set all widget() calls to 0.
  */
}

void PListBox::messageHandler(int fd, PukeMessage *pm) /*FOLD00*/
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LISTBOX_INSERT:
    if(!checkWidget())
      return;
    
    widget()->insertItem(pm->cArg, pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->count();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_INSERT_SORT:
    if(!checkWidget())
      return;
    
    widget()->inSort(pm->cArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->count();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_INSERT_PIXMAP:
    if(!checkWidget())
      return;
    
    widget()->insertItem(QPixmap(pm->cArg), pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->count();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_HIGHLIGHT:
    if(!checkWidget())
      return;
    
    widget()->setCurrentItem(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->currentItem();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_REMOVE:
    if(!checkWidget())
      return;
    
    widget()->removeItem(pm->iArg);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_GETTEXT:
    if(!checkWidget())
       return;
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    if(widget()->text(pm->iArg) != 0x0){
      pmRet.iArg = 1;
      pmRet.iTextSize = strlen(widget()->text(pm->iArg));
      pmRet.cArg = new char[strlen(widget()->text(pm->iArg))+1];
      strcpy(pmRet.cArg, widget()->text(pm->iArg));
      emit outputMessage(fd, &pmRet);
      delete[] pmRet.cArg;
    }
    else{
      pmRet.iArg = 0;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
    }
    break;
  case PUKE_LISTBOX_SET_SCROLLBAR:
    widget()->setScrollBar((bool) pm->iArg);
    pmRet.iCommand = PUKE_LISTBOX_SET_SCROLLBAR_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_SET_AUTO_SCROLLBAR:
    widget()->setAutoScrollBar((bool) pm->iArg);
    pmRet.iCommand = PUKE_LISTBOX_SET_AUTO_SCROLLBAR_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LISTBOX_CLEAR:
    widget()->clear();
    pmRet.iCommand = PUKE_LISTBOX_CLEAR_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  default:
    PTableView::messageHandler(fd, pm);
  }
}

void PListBox::setWidget(QObject *_lb) /*FOLD00*/
{
  if(_lb != 0 && _lb->inherits("QListBox") == FALSE)
    throw(errorInvalidSet(_lb, className()));

  lb = (QListBox *) _lb;
  if(lb != 0){
    connect(lb, SIGNAL(highlighted(int)),
            this, SLOT(highlighted(int)));
    connect(lb, SIGNAL(selected(int)),
            this, SLOT(selected(int)));
  }
  PTableView::setWidget(lb);

}


QListBox *PListBox::widget() /*FOLD00*/
{
  return lb;
}


void PListBox::highlighted(int index) { /*FOLD00*/
  PukeMessage pmRet;

  debug("Got highlight");
  pmRet.iCommand = PUKE_LISTBOX_HIGHLIGHTED_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = index;
  if(widget()->text(index) != 0){
    pmRet.iTextSize = strlen(widget()->text(index));
    pmRet.cArg = new char[strlen(widget()->text(index)) + 1];
    strcpy(pmRet.cArg, widget()->text(index));
  }
  else
    pmRet.cArg = 0;
  emit outputMessage(widgetIden().fd, &pmRet);
  delete[] pmRet.cArg;
}

void PListBox::selected(int index) { /*FOLD00*/
  PukeMessage pmRet;

  debug("Got selected");
  pmRet.iCommand = PUKE_LISTBOX_SELECTED_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = index;
  if(widget()->text(index) != 0){
    pmRet.iTextSize = strlen(widget()->text(index));
    pmRet.cArg = new char[strlen(widget()->text(index)) + 1];
    strcpy(pmRet.cArg, widget()->text(index));
  }
  else
    pmRet.cArg = 0;
  emit outputMessage(widgetIden().fd, &pmRet);
  delete[] pmRet.cArg;
}

bool PListBox::checkWidget(){ /*FOLD00*/
  if(widget() == 0){
    debug("PListBox: No Widget set");
    return FALSE;
  }
  return TRUE;
}

#include "plistbox.moc"

