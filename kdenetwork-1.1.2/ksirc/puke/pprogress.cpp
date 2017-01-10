#include "pprogress.h"
#include <stdio.h>

PObject *createWidget(CreateArgs &ca) /*FOLD00*/
{
  PProgress *pw = new("PProgress") PProgress(ca.parent);
  KSProgress *ksp;
  if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    ksp = new("KSProgress") KSProgress((QWidget *) ca.parent->widget());
  else
    ksp = new("KSProgress") KSProgress();
  pw->setWidget(ksp);
  pw->setWidgetId(ca.pwI);
  return pw;
}


PProgress::PProgress( PObject *parent) /*FOLD00*/
  : PWidget(parent)
{
  //  debug("PProgress PProgress called");
  ksp = 0;
}

PProgress::~PProgress() /*FOLD00*/
{
  //  debug("PProgress: in destructor");
  /*
  delete widget(); // Delete the frame
  ksp=0;      // Set it to 0
  setWidget(ksp); // Now set all widget() calls to 0.
  */
}

void PProgress::messageHandler(int fd, PukeMessage *pm) /*FOLD00*/
{
  //  debug("PProgress handler called");
  PukeMessage pmRet;
  if(widget() == 0){
    warning("Null widget");
    return;
  }
  switch(pm->iCommand){
  case PUKE_KSPROGRESS_SET_RANGE:
    {
      int start=0, stop=1;
      int found = sscanf(pm->cArg, "%d\t%d", &start, &stop);
      if(found != 2)
	throw(errorCommandFailed(PUKE_INVALID,13));
      if(start >= stop){
          stop = start+1;
          start = 0;
      }
      
      widget()->setRange(start, stop);
      pmRet.iCommand = PUKE_KSPROGRESS_SET_RANGE_ACK;
      pmRet.iWinId = pm->iWinId;
      pmRet.iArg = 0;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      break;
    }
  case PUKE_KSPROGRESS_SET_TOPTEXT:
    widget()->setTopText(QString(pm->cArg));
    pmRet.iCommand = PUKE_KSPROGRESS_SET_TOPTEXT;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_KSPROGRESS_SET_BOTTEXT:
    widget()->setBotText(QString(pm->cArg));
    pmRet.iCommand = PUKE_KSPROGRESS_SET_BOTTEXT;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_KSPROGRESS_SET_VALUE:
    widget()->setValue(pm->iArg);
    pmRet.iCommand = PUKE_KSPROGRESS_SET_VALUE_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;        
  default:
    PWidget::messageHandler(fd, pm);
  }
}

void PProgress::cancelPressed(){ /*FOLD00*/
  debug("Cancel Pressed");
  PukeMessage pmRet;
  pmRet.iCommand = PUKE_KSPROGRESS_CANCEL_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.cArg = 0;
  emit outputMessage(widgetIden().fd, &pmRet);
}

void PProgress::setWidget(QObject *_f) /*FOLD00*/
{
  //  debug("PProgress setWidget called");
  if(_f != 0 && _f->inherits("KSProgress") == FALSE)
    throw(errorInvalidSet(_f, className()));

  ksp = (KSProgress *) _f;
  if(widget() != 0){
    connect(widget(), SIGNAL(cancel()),
            this, SLOT(cancelPressed()));
  }
  PWidget::setWidget(ksp);

}


KSProgress *PProgress::widget() /*fold00*/
{
  //  debug("PProgress widget called");
  return ksp;
}

#include "pprogress.moc"

