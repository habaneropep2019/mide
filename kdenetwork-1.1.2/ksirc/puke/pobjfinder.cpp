#include "pobjfinder.h"
#include "pobjfinder-cmd.h"
#include <qstrlist.h>

PObject *createWidget(CreateArgs &ca)
{
  PObjFinder *pw = new("PObjFinder") PObjFinder(ca.parent);
  pw->setWidget(0x0);
  pw->setWidgetId(ca.pwI);
  pw->setPukeController(ca.pc);
  return pw;
}


PObjFinder::PObjFinder(PObject *parent)
  : PObject(parent)
{
  // We don't actually encase a widget since all the ObjFinder interface
  // is static
  setWidget(0x0);
  connect(objFind, SIGNAL(inserted(QObject *)),
          this, SLOT(newObject(QObject *)));
}

PObjFinder::~PObjFinder()
{
}

void PObjFinder::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_OBJFINDER_ALLOBJECTS:
    {
      QString qscArg;
      QStrList allObj = objFinder::allObjects();
      for(uint i = 0; i <= allObj.count(); i++){
        qscArg += allObj.at(i);
        qscArg += "\n";
      }

      pmRet.iCommand = - pm->iCommand;
      pmRet.iWinId = pm->iWinId;
      pmRet.iArg = 0;
      pmRet.cArg = qstrdup(qscArg);
      pmRet.iTextSize = qscArg.length();
      emit outputMessage(fd, &pmRet);
      delete pmRet.cArg;
      break;
    }
  default:
    PObject::messageHandler(fd, pm);
  }
}

void PObjFinder::setWidget(QObject *_obj)
{
  PObject::setWidget(_obj);
}


objFinder *PObjFinder::widget()
{
  return 0x0;
}


void PObjFinder::newObject(QObject *name){
  QString info;

  info = name->className();
  info += "::";
  info += name->name("unnamed");

  PukeMessage pmRet;
  pmRet.iCommand = - PUKE_OBJFINDER_NEWOBJECT_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.cArg = qstrdup(info);
  pmRet.iTextSize = info.length();
  emit outputMessage(widgetIden().fd, &pmRet);
  delete pmRet.cArg;
}