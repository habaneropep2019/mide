#ifndef KBASERULES_H
#define KBASERULES_H

#include <qstring.h>
#include <qlist.h>

#include <messageReceiver.h>

class KSMBaseRules : public KSircMessageReceiver
{
public:
  KSMBaseRules(KSircProcess *proc)
    : KSircMessageReceiver(proc)
    {
      broadcast = FALSE;
    }
  virtual ~KSMBaseRules()
    {
    }

  virtual void sirc_receive(QString str);

  virtual void control_message(int, QString);

  filterRuleList *defaultRules();

private:
  bool broadcast;
  char to[101];
  char match_us[101];
  char to_us[101];
};

#endif
