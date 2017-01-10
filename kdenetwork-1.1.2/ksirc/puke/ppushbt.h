#ifndef PPUSHBUTTON_H
#define PPUSHBUTTON_H

class PPushButton;

#include <qpushbt.h>
#include "pmessage.h"
#include "pbutton.h"
#include "pobject.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PPushButton : public PButton
{
  Q_OBJECT
 public:
  PPushButton ( PObject * parent );
  virtual ~PPushButton ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_b = 0);
  virtual QPushButton *widget();

 public slots:
  
 signals:
  void outputMessage(int fd, PukeMessage *pm);

 private:
  QPushButton *button;

};

#endif
