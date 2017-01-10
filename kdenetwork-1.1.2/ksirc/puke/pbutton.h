#ifndef PBUTTON_H
#define PBUTTON_H

class PButton;

#include <qbutton.h>
#include "pmessage.h"
#include "pwidget.h"
#include "pobject.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PButton : public PWidget
{
  Q_OBJECT
 public:
  PButton ( PObject * parent );
  virtual ~PButton ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_qb = 0x0);
  virtual QButton *widget();

 public slots:
  void buttonPressed();
  void buttonReleased();
  void buttonClicked();
  void buttonToggled(bool);
  
 signals:
  void outputMessage(int fd, PukeMessage *pm);

 private:
  QButton *button;

  void buttonMessage(int iCommand);
  bool checkWidget();
};

#endif
