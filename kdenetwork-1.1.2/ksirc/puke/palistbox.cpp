#include "palistbox.h"

PObject *createWidget(CreateArgs &ca) /*FOLD00*/
{
  PAListBox *plb = new("PAListBox") PAListBox(ca.parent);
  aListBox *lb;
  if(ca.fetchedObj != 0 && ca.fetchedObj->inherits("aListBox") == TRUE){
    lb = (aListBox *) ca.fetchedObj;
    plb->setDeleteAble(FALSE);
  }
  else if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    lb = new("aListBox") aListBox((QWidget *) ca.parent->widget());
  else
    lb = new("aListBox") aListBox();
  plb->setWidget(lb);
  plb->setWidgetId(ca.pwI);
  return plb;
}


PAListBox::PAListBox(PObject *parent) /*FOLD00*/
  : PListBox(parent)
{
  //  debug("PListBox PListBox called");
  lb = 0;
  setWidget(lb);
}

PAListBox::~PAListBox() /*FOLD00*/
{
  //  debug("PListBox: in destructor"); 
  /*
  delete widget();     // Delete the frame
  lb=0;          // Set it to 0
  setWidget(lb); // Now set all widget() calls to 0.
  */
}

void PAListBox::messageHandler(int fd, PukeMessage *pm) /*FOLD00*/
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LISTBOX_INSERT_SORT:
    if(!checkWidget())
      return;

    widget()->inSort(pm->cArg, (bool) pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->count();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_ALISTBOX_ISTOP:
    if(!checkWidget())
      return;

    pmRet.iArg = widget()->isTop(pm->iArg);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_ALISTBOX_FIND_NICK:
    if(!checkWidget())
      return;

    pmRet.iArg = widget()->findNick(pm->cArg);
    
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_ALISTBOX_SMALL_HIGHLIGHT:
    {
      if(!checkWidget())
	return;

      widget()->setAutoUpdate(FALSE);
      int index = widget()->findNick(pm->cArg);
      nickListItem *item = new("nickListItem") nickListItem();
      *item = *widget()->item(index);
      widget()->removeItem(index);
      item->setVoice(pm->iArg);
      widget()->inSort(item);
      widget()->setAutoUpdate(TRUE);
      widget()->repaint(TRUE);

      pmRet.iCommand = - pm->iCommand;
      pmRet.iWinId = pm->iWinId;
      pmRet.iArg = 0;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      break;
    }
  case PUKE_ALISTBOX_BIG_HIGHLIGHT:
    {
      if(!checkWidget())
	return;

      widget()->setAutoUpdate(FALSE);
      int index = widget()->findNick(pm->cArg);
      nickListItem *item = new("nickListItem") nickListItem();
      *item = *widget()->item(index);
      widget()->removeItem(index);
      item->setOp(pm->iArg);
      widget()->inSort(item);
      widget()->setAutoUpdate(TRUE);
      widget()->repaint(TRUE);

      pmRet.iCommand = - pm->iCommand;
      pmRet.iWinId = pm->iWinId;
      pmRet.iArg = 0;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      break;
    }

  default:
    PListBox::messageHandler(fd, pm);
  }
}

void PAListBox::setWidget(QObject *_lb) /*FOLD00*/
{
  if(_lb != 0 && _lb->inherits("aListBox") == FALSE)
    throw(errorInvalidSet(_lb, className()));

  lb = (aListBox *) _lb;
  if(lb != 0){
  }
  PListBox::setWidget(lb);

}


aListBox *PAListBox::widget() /*FOLD00*/
{
  return lb;
}

bool PAListBox::checkWidget(){ /*FOLD00*/
  if(widget() == 0){
    debug("PAListBox: No Widget set");
    return FALSE;
  }
  return TRUE;
}

#include "palistbox.moc"

