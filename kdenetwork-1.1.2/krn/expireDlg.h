#ifndef EXPIRE_DLG
#define EXPIRE_DLG

#include <qdialog.h>
#include <keditcl.h>
#include <qlined.h>

class ExpireDlg: public QDialog
{
    Q_OBJECT
public:
    ExpireDlg();
    ~ExpireDlg();
public slots:
    void save();
private:
    KIntLineEdit *bodies;
    KIntLineEdit *headers;
    KIntLineEdit *bodies2;
    KIntLineEdit *headers2;
};

#endif