
#ifndef KMESSAGERECEIVER_H
#define KMESSAGERECEIVER_H

#include <qstring.h>
#include <qlist.h>

class KSircProcess;

struct filterRule {
  char *desc;
  char *search;
  char *from;
  char *to;
};

typedef QList<filterRule> filterRuleList;

class KSircMessageReceiver
{
public:
  KSircMessageReceiver(KSircProcess *_proc)
    {
      proc = _proc;
      broadcast = TRUE;
    }
  virtual ~KSircMessageReceiver();

  virtual void sirc_receive(QString str);

  virtual void control_message(int, QString);

  bool getBroadcast();
  void setBroadcast(bool bd);

  virtual filterRuleList *defaultRules();

private:
  KSircProcess *proc;
  bool broadcast;

};

#endif
