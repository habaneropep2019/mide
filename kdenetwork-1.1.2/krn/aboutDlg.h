#ifndef aboutDlg_included
#define aboutDlg_included

#include <qdialog.h>

class aboutDlg : public QDialog
{
    Q_OBJECT

public:

    aboutDlg
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~aboutDlg();

};
#endif // aboutDlg_included
