#ifndef KIODCCC_H
#define KIODCCC_H

#include <qobject.h>
#include <qstring.h>
#include <qdict.h>
#include <qprogdlg.h>

#include "KSProgress/ksprogress.h"

#include "messageReceiver.h"
#include "ksircprocess.h"
#include "dccDialog.h"

struct DCCInfo {
  QString nick;
  QString cancelMessage;
  int LastSize;
  int PercentSize;
  bool inProgress;
};

class KSircIODCC : public QObject,
		   public KSircMessageReceiver
{
  Q_OBJECT
public:
  KSircIODCC(KSircProcess *_proc) : QObject(),
    KSircMessageReceiver(_proc)
    {
      proc = _proc;
      pending = new dccDialog();
      connect(pending, SIGNAL(getFile()),
	      this, SLOT(getFile()));
      connect(pending, SIGNAL(forgetFile()),
	      this, SLOT(forgetFile()));
      setBroadcast(FALSE);
    }
  virtual ~KSircIODCC();

  virtual void sirc_receive(QString);
  virtual void control_message(int, QString);
  virtual filterRuleList *defaultRules();

protected slots:
  void cancelTransfer(QString);
  void getFile();
  void forgetFile();

signals:
  void outputLine(QString);

private:
//  QDict<QProgressDialog> DlgList;
  QDict<KSProgress> DlgList;
  QDict<DCCInfo> DCCStatus;
  
  dccDialog *pending;

  KSircProcess *proc;
};

#endif
