/**********************************************************************
 
 The IO LAG Controller

 $$Id: ioLAG.cpp,v 1.5.2.1 1999/02/09 14:47:02 kulow Exp $$

**********************************************************************/
 

#include "ioLAG.h"
#include "control_message.h"
#include <iostream.h>

KSircIOLAG::KSircIOLAG(KSircProcess *_proc)
  : QObject(),
    KSircMessageReceiver(_proc)
{
  proc = _proc;
  setBroadcast(FALSE);
  startTimer(30000);
//  startTimer(5000);
//(proc->getWindowList())["!all"]->control_message(SET_LAG, "99");
}


KSircIOLAG::~KSircIOLAG()
{
  killTimers();
}

void KSircIOLAG::sirc_receive(QString str)
{

  if(str.contains("*L*")){
    int s1, s2;
    s1 = str.find("*L* ") + 4;
    s2 = str.length();
    if(s1 < 0 || s2 < 0){
      cerr << "Lag mesage broken: " << str << endl;
      return;
    }
    QString lag = str.mid(s1, s2 - s1);
    //    cerr << "Lag: " << str << endl;
    //    cerr << "Setting lag to: " << lag << endl;
    (proc->getWindowList())["!all"]->control_message(SET_LAG, lag);
  }

}

void KSircIOLAG::control_message(int, QString)
{
}

void KSircIOLAG::timerEvent ( QTimerEvent * )
{
  QString cmd = "/lag\n";
  emit outputLine(cmd);
}

#include "ioLAG.moc"
