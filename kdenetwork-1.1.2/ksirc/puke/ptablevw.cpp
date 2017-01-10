#include "ptablevw.h"

PObject *createWidget(CreateArgs &ca)
{
  warning("Table View is abstract class, cannot create an object from it!!!");
  PTableView *pt = new("PTableView") PTableView(ca.parent);
  pt->setWidget(0);
  pt->setWidgetId(ca.pwI);
  return pt;
}


PTableView::PTableView(PObject *parent)
  : PFrame(parent)
{
  //  debug("PTableView PTableView called");
  tbl = 0;
  setWidget(tbl);
}

PTableView::~PTableView()
{
  // don't delete the widget since none acutally exists.
  // delete widget();     // Delete the frame
  //  tbl=0;          // Set it to 0
//  setWidget(tbl); // Now set all widget() calls to 0.
}

void PTableView::messageHandler(int fd, PukeMessage *pm)
{
//  PukeMessage pmRet;
  switch(pm->iCommand){
      /*
       case PUKE_LINED_SET_MAXLENGTH:
       if(widget() == 0){
       debug("PTableView: No Widget set");
       return;
       }
       widget()->setMaxLength(pm->iArg);
       pmRet.iCommand = - pm->iCommand;
       pmRet.iWinId = - pm->iWinId;
       pmRet.iArg = widget()->maxLength();
       emit outputMessage(fd, &pmRet);
       break;
       */
  default:
    PFrame::messageHandler(fd, pm);
  }
}

void PTableView::setWidget(QObject *_tbv)
{
  if(_tbv != 0 && _tbv->inherits("QTableView") == FALSE)
    throw(errorInvalidSet(_tbv, className()));

  tbl = (QTableView *) _tbv;
  if(tbl != 0){
  }
  PFrame::setWidget(_tbv);

}


QTableView *PTableView::widget()
{
  return tbl;
}


#include "ptablevw.moc"

