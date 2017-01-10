#ifndef fontsDlg_included
#define fontsDlg_included

#include <qdialog.h>
#include <qcombo.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qchkbox.h>

#include <kcolorbtn.h>

class fontsDlg : public QDialog
{
    Q_OBJECT

public:

    fontsDlg
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~fontsDlg();
    QPushButton* b1;
    QPushButton* b2;
    QComboBox* fontSize;
    QLineEdit* samp1;
    QLineEdit* samp2;
    QComboBox* stdFontName;
    QComboBox* fixedFontName;
    KColorButton* bgColor;
    KColorButton* fgColor;
    KColorButton* linkColor;
    KColorButton* followColor;
    QCheckBox* vertsplit;
    QCheckBox* singlewin;
    QComboBox* hdrstyle;
    QComboBox* atmstyle;
    
private slots:
    void save();
    void syncFonts(int);

};
#endif // fontsDlg_included
