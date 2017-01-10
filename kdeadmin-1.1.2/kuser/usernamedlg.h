#ifndef _KU_USERNAMEDLG_H
#define _KU_USERNAMEDLG_H

#include <qdialog.h>
#include <qpushbt.h>
#include <qlined.h>
#include <qlayout.h>
#include "kuser.h"

class usernamedlg : public QDialog {
  Q_OBJECT
public:
  usernamedlg(KUser*auser, QWidget* parent = NULL, const char* name = NULL);
  ~usernamedlg();

protected slots:
  void ok();
  void cancel();

protected:
  KUser *user;

  QPushButton *pbOk;
  QPushButton *pbCancel;
  QLineEdit   *leusername;
};

#endif // _KU_USERNAMEDLG_H

