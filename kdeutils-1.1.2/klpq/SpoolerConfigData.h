/**********************************************************************

	--- Qt Architect generated file ---

	File: SpoolerConfigData.h
	Last generated: Sat Feb 7 11:59:44 1998

 *********************************************************************/

#ifndef SpoolerConfigData_included
#define SpoolerConfigData_included

#include <qdialog.h>
#include <qpushbt.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qbttngrp.h>

class SpoolerConfigData : public QDialog
{
    Q_OBJECT

public:

    SpoolerConfigData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~SpoolerConfigData();

public slots:


protected slots:


protected:
    QRadioButton* rb_bsd;
    QRadioButton* rb_lprng;
    QRadioButton* rb_ppr;
    QLineEdit* i_lpq;
    QLineEdit* i_lpc;
    QLineEdit* i_lprm;
    QPushButton* b_ok;
    QPushButton* b_cancel;
    QButtonGroup* bg_spooler;
};

#endif // SpoolerConfigData_included
