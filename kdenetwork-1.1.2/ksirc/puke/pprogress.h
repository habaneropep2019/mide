#ifndef PPROGRESS_H
#define PPROGRESS_H

class PProgress;

#include "../KSProgress/ksprogress.h"
#include "pmessage.h"
//#include "pwidget.h"
#include "pobject.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PProgress : public PWidget {
  Q_OBJECT
public:
  PProgress ( PObject * parent = 0);
  virtual ~PProgress ();

  virtual void messageHandler(int fd, PukeMessage *pm);

  virtual void setWidget(QObject *_f);
  virtual KSProgress *widget();

signals:
  void outputMessage(int fd, PukeMessage *pm);

protected slots:
  void cancelPressed();

private:
  KSProgress *ksp;
};

#endif
