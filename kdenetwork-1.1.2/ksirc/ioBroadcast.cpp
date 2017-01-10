/**********************************************************************
 
 The IO Broadcaster

 $$Id: ioBroadcast.cpp,v 1.16 1998/12/15 04:57:54 asj Exp $$

 The IO broadcaster bassed on the ksircmessage receiver takes a
 message and send it all ksircmessage receivers, except of course it
 self.

 It does the same with control_messages.

 Implementation: 

  Make a QDictIterator, iterate over the windows sedning to each
  broadcaster that's not itself.  

  *** NOTE! don't have 2 broadcasters or else they'll broadcast forever!

**********************************************************************/
 

#include "ioBroadcast.h"
#include <iostream.h>

KSircIOBroadcast::~KSircIOBroadcast()
{
}

void KSircIOBroadcast::sirc_receive(QString str)
{

  QDictIterator<KSircMessageReceiver> it(proc->getWindowList());

  KSircMessageReceiver *dflt = (proc->getWindowList())["!default"];
  if(dflt->getBroadcast() == TRUE)
    dflt->sirc_receive(str);

  it.toFirst();

  while(it.current()){
    if((it.current()->getBroadcast() == TRUE) && (it.current() != dflt))
      it.current()->sirc_receive(str);
    ++it;
  }

}

void KSircIOBroadcast::control_message(int command, QString str)
{

  QDictIterator<KSircMessageReceiver> it(proc->getWindowList());

  it.toFirst();

  while(it.current()){
    if(it.current() != this)
      it.current()->control_message(command, str);
    ++it;
  }
}


filterRuleList *KSircIOBroadcast::defaultRules()
{
  filterRule *fr;
  filterRuleList *frl = new("filterRuleList") filterRuleList();
  frl->setAutoDelete(TRUE);
  fr = new("filterRule") filterRule();
  fr->desc = "Inverse to KSIRC inverse";
  fr->search = ".*";
  fr->from = "(?g)\\x16";
  fr->to = "~r";
  frl->append(fr);
  fr = new("filterRule") filterRule();
  fr->desc = "Underline to KSIRC underline";
  fr->search = ".*";
  fr->from = "(?g)\\x1f";
  fr->to = "~u";
  frl->append(fr);
  fr = new("filterRule") filterRule();
  fr->desc = "Bold to KSIRC bold";
  fr->search = ".*";
  fr->from = "(?g)\\x02";
  fr->to = "~b";
  frl->append(fr);
  fr = new("filterRule") filterRule();
  fr->desc = "Ordinary to KSIRC ordinary";
  fr->search = ".*";
  fr->from = "(?g)\\x0f";
  fr->to = "~c";
  frl->append(fr);
  return frl;

}
