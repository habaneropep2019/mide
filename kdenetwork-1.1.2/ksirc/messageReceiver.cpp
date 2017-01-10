#include "messageReceiver.h"
#include "ksircprocess.h"

KSircMessageReceiver::~KSircMessageReceiver()
{
}

void KSircMessageReceiver::sirc_receive(QString)
{
  warning("Pure virtual sirc_receive called!!!!");
}

void KSircMessageReceiver::control_message(int, QString)
{
  warning("Pure virutal control_messaga called for a message_receiver!\n");
}

void KSircMessageReceiver::setBroadcast(bool bd)
{
  broadcast = bd;
}

bool KSircMessageReceiver::getBroadcast()
{
  return broadcast;
}

filterRuleList *KSircMessageReceiver::defaultRules()
{
  return new("filterRuleList") filterRuleList();
}
