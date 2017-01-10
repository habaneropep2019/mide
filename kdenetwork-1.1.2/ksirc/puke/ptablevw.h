#ifndef PTABLEVIEW_H
#define PTABLEVIEW_H

class PTableView;

#include <qtablevw.h>
#include "pmessage.h"
#include "pframe.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

class PTableView : public PFrame
{
  Q_OBJECT
 public:
  PTableView ( PObject * parent );
  virtual ~PTableView ();
  
  virtual void messageHandler(int fd, PukeMessage *pm);
  
  virtual void setWidget(QObject *_tbl);
  virtual QTableView *widget();

  public slots:

 signals:
  void outputMessage(int fd, PukeMessage *pm);

 private:
  QTableView *tbl;
};

#endif
