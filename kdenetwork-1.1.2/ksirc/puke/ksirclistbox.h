#ifndef PKSIRCLISTBOX_H
#define PKSIRCLISTBOX_H

class PKSircListBox;

#include "../irclistbox.h"
#include "pmessage.h"
#include "plistbox.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PKSircListBox : public PListBox
{
  Q_OBJECT
 public:
  PKSircListBox ( PObject * parent );
  virtual ~PKSircListBox ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_lb);
  virtual KSircListBox *widget();

  public slots:

  signals:
    void outputMessage(int fd, PukeMessage *pm);

protected:
  bool checkWidget();

private:
  KSircListBox *lb;
   
};

#endif
