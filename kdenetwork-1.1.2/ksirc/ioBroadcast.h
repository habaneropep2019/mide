#ifndef KIOBROADCAST_H
#define KIOBROADCAST_H

#include <qobject.h>
#include <qstring.h>
#include <qdict.h>

#include "messageReceiver.h"
#include "ksircprocess.h"

class KSircIOBroadcast : public KSircMessageReceiver
{
public:
  KSircIOBroadcast(KSircProcess *_proc) : KSircMessageReceiver(_proc)
    {
      proc = _proc;
      setBroadcast(FALSE);
    }
  virtual ~KSircIOBroadcast();

  virtual void sirc_receive(QString str);
  virtual void control_message(int, QString);

  virtual filterRuleList *defaultRules();

private:
  KSircProcess *proc;
};

#endif
