#include <stdio.h>

#include "playout.h"
#include "commands.h"

PLayout::PLayout(QObject *pobject)
  : PObject(pobject)
{
  // Connect slots as needed
  setWidget();
}

PLayout::~PLayout()
{
  //  debug("PObject: in destructor");
  /*
  delete widget();
  layout = 0;
  setWidget();
  */
}

PObject *PLayout::createWidget(CreateArgs &ca)
{
  PLayout *pw = new("PLayout") PLayout(ca.parent);
  QBoxLayout *qbl;
  int direction, border, iType, iParent;
  // Retreive the border and direction information out of the
  // carg string
  if(sscanf(ca.pm->cArg, "%d\t%d\t%d\t%d", &iParent, &iType, &direction, &border) < 4)
    throw(errorCommandFailed(-ca.pm->iCommand, -1));
  
  if((ca.parent != 0) &&
     (ca.parent->widget()->isWidgetType() == TRUE)){
    qbl = new("QBoxLayout") QBoxLayout((QWidget *) ca.parent->widget(), (QBoxLayout::Direction) direction, border);
    //      debug("Creating layout with parent: %d", parent.iWinId);

  }
  else{
    qbl = new("QBoxLayout") QBoxLayout((QBoxLayout::Direction) direction, border);
    //      debug("Creating layout NO PARENT", parent.iWinId);
  }
  pw->setWidget(qbl);
  pw->setWidgetId(ca.pwI);
  pw->setPukeController(ca.pc);
  return pw;
}

void PLayout::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;

//  debug("In PLayout: %d", pm->iCommand);
  
  if(pm->iCommand == PUKE_LAYOUT_ADDWIDGET){
    if(pm->iTextSize != 2*sizeof(char)){
      warning("PLayout/addwidget: incorrent cArg size, bailing out.  Needed: %u wanted: %d\n", sizeof(int), pm->iTextSize);
      pmRet.iCommand = PUKE_LAYOUT_ADDWIDGET_ACK; // ack the add widget
      pmRet.iWinId = pm->iWinId;
      pmRet.iArg = 1;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      return;
    }
    widgetId wiWidget;
    wiWidget.fd = fd;
    wiWidget.iWinId = pm->iArg;
    PWidget *pw = controller()->id2pwidget(&wiWidget);
    //    debug("Adding widget with stretch: %d and align: %d", (int) pm->cArg[0],
    //	  (int) pm->cArg[1]);
    widget()->addWidget(pw->widget(), pm->cArg[0], pm->cArg[1]);

    pmRet.iCommand = PUKE_LAYOUT_ADDWIDGET_ACK; // ack the add widget
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
  }
  else if(pm->iCommand == PUKE_LAYOUT_ADDLAYOUT){
    if(pm->iTextSize != sizeof(char)){
      warning("PLayout: incorrent cArg size, bailing out.  Needed: %u wanted: %d\n", sizeof(int), pm->iTextSize);
      pmRet.iCommand = PUKE_LAYOUT_ADDLAYOUT_ACK; // ack the add widget
      pmRet.iWinId = pm->iWinId;
      pmRet.iArg = 1;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      return;
    }
    PObject *pld = controller()->id2pobject(fd, pm->iWinId);
    PObject *pls = controller()->id2pobject(fd, pm->iArg);
    if( (pld->widget()->inherits("QBoxLayout") == FALSE) || (pls->widget()->inherits("QBoxLayout") == FALSE))
      throw(errorCommandFailed(PUKE_LAYOUT_ADDLAYOUT_ACK, 1));
    PLayout *plbd = (PLayout *) pld;
    PLayout *plbs = (PLayout *) pls;
    plbd->widget()->addLayout(plbs->widget(), pm->cArg[0]);

    pmRet.iCommand = PUKE_LAYOUT_ADDLAYOUT_ACK; // ack the add widget
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
  }
  else if(pm->iCommand == PUKE_LAYOUT_ADDSTRUT){
    PObject *po = controller()->id2pobject(fd, pm->iWinId);
    if(po->widget()->inherits("PBoxLayout") != TRUE)
      throw(errorCommandFailed(PUKE_LAYOUT_ADDSTRUT_ACK, 1));
    PLayout *pl = (PLayout *) po;
    
    pl->widget()->addStrut(pm->iArg);

    pmRet.iCommand = PUKE_LAYOUT_ADDSTRUT_ACK; // ack the add widget
    pmRet.iWinId = pm->iWinId;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
  }
  else if(pm->iCommand == PUKE_LAYOUT_ACTIVATE){
    PObject *po = controller()->id2pobject(fd, pm->iWinId);
    if(po->widget()->inherits("PBoxLayout") != TRUE)
      throw(errorCommandFailed(PUKE_LAYOUT_ACTIVATE_ACK, 1));
    PLayout *pl = (PLayout *) po;

    pmRet.iArg = 0; // setup failure case
    pl->widget()->activate();

    pmRet.iCommand = PUKE_LAYOUT_ACTIVATE_ACK; // ack the add widget
    pmRet.iWinId = pm->iWinId;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
  }
  else {
    PObject::messageHandler(fd, pm);
  }

}

void PLayout::setWidget(QObject *_layout)
{
  //  debug("PObject setwidget called");
  if(_layout != 0 && _layout->inherits("QBoxLayout") == FALSE)
      throw(errorInvalidSet(_layout, className()));

  layout = (QBoxLayout *) _layout;
  PObject::setWidget(_layout);
  
}

QBoxLayout *PLayout::widget()
{
  return layout;
}


#include "playout.moc"

