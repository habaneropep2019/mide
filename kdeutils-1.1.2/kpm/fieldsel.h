// fieldsel.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef FIELDSEL_H
#define FIELDSEL_H

#include <qdialog.h>
#include <qbitarry.h>
#include <qchkbox.h>
#include "proc.h"

class FieldSelect : public QDialog {
    Q_OBJECT
public:
    FieldSelect(Procview *pv, Proc *proc);

    void update_boxes();

public slots:
    void field_toggled(bool);
    void closed();

signals:
    void added_field(int);
    void removed_field(int);

protected:
    QCheckBox **buts;
    int nbuttons;
    QBitArray disp_fields;
    bool updating;
    Procview *procview;

    void set_disp_fields();
    void closeEvent(QCloseEvent *);
};

#endif	// FIELDSEL_H
