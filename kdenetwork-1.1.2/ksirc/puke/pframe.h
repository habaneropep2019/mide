#ifndef PFRAME_H
#define PFRAME_H

class PFrame;

#include <qframe.h>
#include "pmessage.h"
#include "pwidget.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PFrame : public PWidget
{
  Q_OBJECT
 public:
  PFrame ( PObject * parent );
  virtual ~PFrame ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);

  virtual void setWidget(QObject *w = 0x0);
  virtual QFrame *widget();

 signals:
  void outputMessage(int fd, PukeMessage *pm);

 private:
  QFrame *frame;
};

#endif
