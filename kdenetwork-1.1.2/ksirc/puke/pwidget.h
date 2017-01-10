#ifndef PWIDGET_H
#define PWIDGET_H

class PWidget;

#include <qobject.h>
#include <qwidget.h>
#include "pobject.h"
#include "pmessage.h"

class PWidget : public PObject
{
  Q_OBJECT
 public:
  PWidget(PObject *parent = 0);
  virtual ~PWidget();

  // Init and setup code
  static PObject *createWidget(CreateArgs &ca);

  virtual void messageHandler(int fd, PukeMessage *pm);

  virtual void setWidget(QObject *w = 0x0);
  virtual QWidget *widget();

 signals:
  void outputMessage(int fd, PukeMessage *pm);
  void widgetDestroyed(widgetId wI);

 protected:
  bool eventFilter(QObject *o, QEvent *e);

 private:
  QWidget *w;

  void (PWidget::*eventList[20])(QObject *, QEvent *);

  void eventNone(QObject *, QEvent *);
  void eventTimer(QObject *, QEvent *);
  void eventMouse(QObject *, QEvent *);
  void eventKey(QObject *, QEvent *);
  void eventFocus(QObject *, QEvent *);
  void eventPaint(QObject *, QEvent *);
  void eventMove(QObject *, QEvent *);
  void eventResize(QObject *, QEvent *);

};

#endif
