#include <qmovie.h> 
#include "plabel.h"

PObject *createWidget(CreateArgs &ca)
{
  PLabel *pw = new("PLabel") PLabel(ca.parent);
  QLabel *le;
  if(ca.fetchedObj != 0 && ca.fetchedObj->inherits("QLabel") == TRUE){
    le = (QLabel *) ca.fetchedObj;
    pw->setDeleteAble(FALSE);
  }
  else if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    le = new("QLabel") QLabel((QWidget *) ca.parent->widget());
  else
    le = new("QLabel") QLabel();
  pw->setWidget(le);
  pw->setWidgetId(ca.pwI);
  return pw;
}


PLabel::PLabel(PObject *parent)
  : PFrame(parent)
{
  //  debug("PLabel PLabel called");
  label = 0;
  setWidget(label);
}

PLabel::~PLabel()
{
  //  debug("PLabel: in destructor");
  /*
  delete widget();     // Delete the frame
  label=0;          // Set it to 0
  setWidget(label); // Now set all widget() calls to 0.
  */
}

void PLabel::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LABEL_SETTEXT:
    if(!checkWidget())
      return;

    widget()->setText(pm->cArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LABEL_SETPIXMAP:
    if(!checkWidget())
       return;

    widget()->setPixmap(QPixmap(pm->cArg));
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LABEL_SETMOVIE:
    if(!checkWidget())
      return;

    widget()->setMovie(QMovie(pm->cArg));
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LABEL_SETALIGNMENT:
    if(!checkWidget())
      return;

    widget()->setAlignment(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  default:
    PFrame::messageHandler(fd, pm);
  }
}

void PLabel::setWidget(QObject *_l)
{
  if(_l != 0 && _l->inherits("QLabel") == FALSE)
    throw(errorInvalidSet(_l, className()));

  label = (QLabel *) _l;
  PWidget::setWidget(_l);

}


QLabel *PLabel::widget()
{
  return label;
}

bool PLabel::checkWidget(){
  if(widget() == 0){
    debug("PLabel: No Widget set");
    return FALSE;
  }
  return TRUE;
}


#include "plabel.moc"

