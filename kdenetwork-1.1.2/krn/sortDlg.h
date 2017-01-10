#ifndef SORT_DLG_H
#define SORT_DLG_H

#include <qdialog.h>
#include "typelayout.h"

class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;

class sortDlg : public QDialog
{
    Q_OBJECT
        
public:
    
    sortDlg();
    
    virtual ~sortDlg();
    KTypeLayout *l;

public slots:
    void save();

};


#endif