#ifndef POBJFINDER_H
#define POBJFINDER_H

#include "pobject.h"
#include "../objFinder.h"
#include "pmessage.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PObjFinder : public PObject
{
  Q_OBJECT
 public:
  PObjFinder ( PObject * parent );
  virtual ~PObjFinder ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_obj = 0);
  virtual objFinder *widget();

public slots:
  virtual void newObject(QObject *obj);

 signals:
   void outputMessage(int fd, PukeMessage *pm);

protected:
  bool checkWidget();

 private:
};

#endif
