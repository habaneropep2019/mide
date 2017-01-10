#ifndef findArtDlg_included
#define findArtDlg_included

#include <qwidget.h>

class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;

class findArtDlg : public QWidget
{
    Q_OBJECT
        
public:
    
    findArtDlg(QWidget* parent = NULL,const char* name = NULL);
    
    virtual ~findArtDlg();

    QPushButton* b1;
    QPushButton* b2;
    QComboBox* field;
    QLineEdit* expr;
    QCheckBox* casesen;
    QCheckBox* wildmode;

signals:
    void FindThis(const char *,const char *,bool,bool);
    
private slots:
    void close();
    void find();
    
};
#endif // findArtDlg_included
