#include "pframe.h"

PObject *createWidget(CreateArgs &ca)
{
  PFrame *pw = new("PFrame") PFrame(ca.parent);
  QFrame *tf;
  if(ca.parent != 0 &&
     ca.parent->widget()->isWidgetType() == TRUE)
    tf = new("QFrame") QFrame((QWidget *) ca.parent->widget());
  else
    tf = new("QFrame") QFrame();
  pw->setWidget(tf);
  pw->setWidgetId(ca.pwI);
  pw->setPukeController(ca.pc);  
  return pw;
}


PFrame::PFrame( PObject *parent)
  : PWidget(parent)
{
  //  debug("PFrame PFrame called");
  frame = 0;
}

PFrame::~PFrame()
{
  //  debug("PFrame: in destructor");
  /*
  delete frame; // Delete the frame
  frame=0;      // Set it to 0
  setWidget(frame); // Now set all widget() calls to 0.
  */
}

void PFrame::messageHandler(int fd, PukeMessage *pm)
{
  //  debug("PFrame handler called");
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_QFRAME_SET_FRAME:
    widget()->setFrameStyle(pm->iArg);
    pmRet.iCommand = PUKE_QFRAME_SET_FRAME_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->frameStyle();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_QFRAME_SET_LINEWIDTH:
    widget()->setLineWidth(pm->iArg);
    pmRet.iCommand = PUKE_QFRAME_SET_LINEWIDTH_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->lineWidth();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  default:
    PWidget::messageHandler(fd, pm);
  }
}

void PFrame::setWidget(QObject *w)
{
  if(w != 0 && w->inherits("QFrame") == FALSE)
    throw(errorInvalidSet(w, className()));

  frame = (QFrame *) w;
  PWidget::setWidget((QWidget *) w);
}


QFrame *PFrame::widget()
{
  //  debug("PFrame widget called");
  return frame;
}

#include "pframe.moc"

