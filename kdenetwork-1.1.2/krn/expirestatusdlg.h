#ifndef EXPIRESTATUSDLG_H
#define EXPIRESTATUSDLG_H

#include <qdialog.h>

class ExpireStatusDlg: public QDialog
{
    Q_OBJECT
public:
    ExpireStatusDlg();
    ~ExpireStatusDlg();
private slots:
    void doExpire();
};

#endif
