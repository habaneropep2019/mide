#ifndef IdentDlg_included
#define IdentDlg_included

#include <qdialog.h>

class QLineEdit;
class QPushButton;

class IdentDlg : public QDialog
{
    Q_OBJECT

public:

    IdentDlg
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~IdentDlg();

    QLineEdit* realname;
    QLineEdit* address;
    QLineEdit* organization;
    QPushButton* b1;
    QPushButton* b2;

};
#endif // IdentDlg_included
