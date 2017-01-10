// scheddlg.h
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef SCHEDDLG_H
#define SCHEDDLG_H

#include <qdialog.h>
#include <qradiobt.h>
#include <qlabel.h>
#include <qlined.h>
#include <qbttngrp.h>

class SchedDialog : public QDialog {
    Q_OBJECT
public:
    SchedDialog(int policy, int prio);

    int out_prio;
    int out_policy;

protected slots:
    void done_dialog();
    void button_clicked(int id);

private:
    QButtonGroup *bgrp;
    QRadioButton *rb_other, *rb_fifo, *rb_rr;
    QLabel *lbl;
    QLineEdit *lined;
};

#endif	// SCHEDDLG_H
