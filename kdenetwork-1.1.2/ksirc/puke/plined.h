#ifndef PLINEEDIT_H
#define PLINEEDIT_H

class PLineEdit;

#include <qlined.h>
#include "pmessage.h"
#include "pwidget.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PLineEdit : public PWidget
{
  Q_OBJECT
 public:
  PLineEdit ( PObject * parent );
  virtual ~PLineEdit ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_f);
  virtual QLineEdit *widget();

 public slots:
  void updateText(const char *);
  void returnPress();
  

 signals:
  void outputMessage(int fd, PukeMessage *pm);

 private:
  QLineEdit *lineedit;
};

#endif
