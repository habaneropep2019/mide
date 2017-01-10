#ifndef KIODISCARD_H
#define KIODISCARD_H

#include <qobject.h>
#include <qstring.h>
#include <qdict.h>

#include "messageReceiver.h"
#include "ksircprocess.h"

class KSircIODiscard : public KSircMessageReceiver
{
public:
  KSircIODiscard(KSircProcess *_proc) : KSircMessageReceiver(_proc)
    {
    }
  virtual ~KSircIODiscard();

  virtual void sirc_receive(QString);
  virtual void control_message(int, QString);

private:
};

#endif
