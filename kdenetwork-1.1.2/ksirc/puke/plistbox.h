#ifndef PLISTBOX_H
#define PLISTBOX_H

class PListBox;

#include <qlistbox.h>
#include "pmessage.h"
#include "ptablevw.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PListBox : public PTableView
{
  Q_OBJECT
 public:
  PListBox ( PObject * parent );
  virtual ~PListBox ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_lb);
  virtual QListBox *widget();

  public slots:
    void highlighted(int);
    void selected(int);

  signals:
    void outputMessage(int fd, PukeMessage *pm);

protected:
  bool checkWidget();

private:
  QListBox *lb;
   
};

#endif
