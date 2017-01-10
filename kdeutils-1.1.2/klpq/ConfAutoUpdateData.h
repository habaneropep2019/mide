#ifndef ConfAutoUpdateData_included
#define ConfAutoUpdateData_included

//#include <qlcdnum.h>
#include <qpushbt.h>
#include <qscrbar.h>

class QLCDNumber;

class ConfAutoUpdateData
{
public:

    ConfAutoUpdateData
    (
        QWidget* parent
    );


    QScrollBar* s_freq;
    QLCDNumber* lcd_freq;
    QPushButton* b_ok;
    QPushButton* b_cancel;

};

#endif // ConfAutoUpdateData_included
