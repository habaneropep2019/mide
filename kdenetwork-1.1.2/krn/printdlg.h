#ifndef PRINT_DLG_H
#define PRINT_DLG_H

#include <qdialog.h>

class PrintDlg: public QDialog
{
    Q_OBJECT
public:
    PrintDlg();
    ~PrintDlg();
public slots:
    void save();
private:
};


#endif