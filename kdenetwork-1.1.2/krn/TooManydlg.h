#ifndef TOO_MANY_DLG_H
#define TOO_MANY_DLG_H
#include <qdialog.h>

class QPushButton;
class QRadioButton;
class QCheckBox;
class KIntLineEdit;
class QComboBox;

class TooManyDlg : public QDialog
{
    Q_OBJECT

public:
    TooManyDlg (QWidget* parent = NULL,const char* name = NULL);
    virtual ~TooManyDlg();
    int exec();
    void TooManyDlg::keyPressEvent( QKeyEvent *e );
    int howmany;
    bool newthres;
    bool frombottom;
    KIntLineEdit *number;
    QCheckBox *ntcb;
    QRadioButton *fbrb;
};
#endif