/**********************************************************************
 
 IO Notify Messanger

 $$Id: ioNotify.cpp,v 1.5.2.1 1999/02/09 14:47:03 kulow Exp $$

**********************************************************************/
 

#include "ioNotify.h"
#include <iostream.h>

KSircIONotify::KSircIONotify(KSircProcess *_proc)
  : QObject(),
    KSircMessageReceiver(_proc)
{
  proc = _proc;
  setBroadcast(FALSE);
}


KSircIONotify::~KSircIONotify()
{
}

void KSircIONotify::sirc_receive(QString str)
{
  if(str.contains("*)*")){
    int s1, s2;
    s1 = str.find("Signon by") + 10;
    s2 = str.find(" ", s1);
    if(s1 < 0 || s2 < 0){
      cerr << "Nick Notify mesage broken: " << str << endl;
      return;
    }
    QString nick = str.mid(s1, s2 - s1);
    emit notify_online(nick);
  }
  else if(str.contains("*(*")){
    int s1, s2;
    s1 = str.find("Signoff by") + 11;
    s2 = str.find(" ", s1);
    if(s1 < 0 || s2 < 0){
      cerr << "Nick Notify mesage broken: " << str << endl;
      return;
    }
    QString nick = str.mid(s1, s2 - s1);
    emit notify_offline(nick);
  }
  else{
    proc->getWindowList()["!default"]->sirc_receive(str);
    cerr << "Nick Notifer got " << str << endl;
  }
}

void KSircIONotify::control_message(int, QString)
{
}


filterRuleList *KSircIONotify::defaultRules()
{
  filterRule *fr;
  filterRuleList *frl = new("filterRuleList") filterRuleList();
  frl->setAutoDelete(TRUE);
  fr = new("filterRule") filterRule();
  fr->desc = "Send Nick Notifies to notifier parser";
  fr->search = "^\\*\\S?[\\(\\)]\\S?\\* ";
  fr->from = "^";
  fr->to = "~!notify~";
  frl->append(fr);
  return frl;
}
#include "ioNotify.moc"
