#ifndef KIOLAG_H
#define KIOLAG_H

#include <qobject.h>
#include <qstring.h>
#include <qdict.h>

#include "messageReceiver.h"
#include "ksircprocess.h"

class KSircIOLAG : public QObject,
		   public KSircMessageReceiver
{
  Q_OBJECT
public:
  KSircIOLAG(KSircProcess *_proc);
  virtual ~KSircIOLAG();

  virtual void sirc_receive(QString);
  virtual void control_message(int, QString);

protected slots:

protected:
 virtual void timerEvent ( QTimerEvent * );

signals:
  void outputLine(QString);

private:
  KSircProcess *proc;
};

#endif
