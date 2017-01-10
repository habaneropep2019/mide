#ifndef PALISTBOX_H
#define PALISTBOX_H

class PAListBox;

#include "../alistbox.h"
#include "pmessage.h"
#include "plistbox.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PAListBox : public PListBox
{
  Q_OBJECT
 public:
  PAListBox ( PObject * parent );
  virtual ~PAListBox ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_lb = 0x0);
  virtual aListBox *widget();

  public slots:

  signals:
    void outputMessage(int fd, PukeMessage *pm);

protected:
  bool checkWidget();

private:
  aListBox *lb;
   
};

#endif
