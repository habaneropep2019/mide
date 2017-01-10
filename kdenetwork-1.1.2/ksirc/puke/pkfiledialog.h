#ifndef PKFILEDIALOG_H
#define PKFILEDIALOG_H

class PKFileDialog;

#include <kfiledialog.h>
#include "pmessage.h"
#include "pwidget.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PKFileDialog : public PWidget
{
  Q_OBJECT
 public:
  PKFileDialog ( PObject * parent );
  virtual ~PKFileDialog ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_f = 0x0);
  virtual KFileDialog *widget();

 public slots:

 signals:
  void outputMessage(int fd, PukeMessage *pm);

 private:
  KFileDialog *kfbd;
};

#endif
