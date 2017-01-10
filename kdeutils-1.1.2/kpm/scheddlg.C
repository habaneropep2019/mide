// scheddlg.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 199

#include <sched.h>

#include "scheddlg.h"
#include "dialogs.h"

#include <qpushbt.h>
#include <qaccel.h>
#include <qstring.h>
#include <kbuttonbox.h>
#include <qlayout.h>
#include "buttongroup.h"
#include "locale.h"

SchedDialog::SchedDialog(int policy, int prio)
         : QDialog(0, 0, TRUE)
{
    setCaption(i18n("Change scheduling"));

    QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

    int max_x = 0;
    int y_pos = (int)(fontMetrics().lineSpacing() * 1.5);
    bgrp = new ButtonGroup(i18n("Scheduling Policy"), this);
    rb_other = new QRadioButton(i18n("SCHED_OTHER (time-sharing)"),
					      bgrp);
    rb_other->adjustSize();
    rb_other->move(10, y_pos);
    y_pos += rb_other->height();
    max_x = QMAX(max_x, rb_other->width());

    rb_fifo = new QRadioButton(i18n("SCHED_FIFO (real-time)"), bgrp);
    rb_fifo->adjustSize();
    rb_fifo->move(10, y_pos);
    y_pos += rb_fifo->height();
    max_x = QMAX(max_x, rb_fifo->width());

    rb_rr = new QRadioButton(i18n("SCHED_RR (real-time)"), bgrp);
    rb_rr->adjustSize();
    rb_rr->move(10, y_pos);
    y_pos += rb_rr->height();
    max_x = QMAX(max_x, rb_rr->width());

    bgrp->setMinimumSize(max_x + 20, y_pos + 10);
    connect(bgrp, SIGNAL(clicked(int)), SLOT(button_clicked(int)));
    tl->addWidget(bgrp);

    int active = 0;
    switch(policy) {
    case SCHED_OTHER:
	active = 0; break;
    case SCHED_FIFO:
	active = 1; break;
    case SCHED_RR:
	active = 2; break;
    }
    ((QRadioButton *)(bgrp->find(active)))->setChecked(TRUE);
    out_policy = policy;
    out_prio = prio;

    QHBoxLayout *l1 = new QHBoxLayout;
    tl->addLayout(l1);    
    lbl = new QLabel(i18n("Priority (1-99):"), this);
    lbl->setMinimumSize(lbl->sizeHint());
    l1->addWidget(lbl);

    lined = new QLineEdit(this);
    lined->setFixedWidth(lined->sizeHint().width()/2);
    lined->setFixedHeight(lined->sizeHint().height());
    lined->setMaxLength(4);
    QString s(4);
    s.setNum(prio);
    lined->setText(s);
    l1->addWidget(lined);

    button_clicked(active);

    // make sure return and escape work as accelerators
    connect(lined, SIGNAL(returnPressed()), SLOT(done_dialog()));
    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(Key_Escape), this, SLOT(reject()));

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    QPushButton *ok = bbox->addButton(i18n("OK"));
    ok->setDefault(TRUE);
    connect(ok, SIGNAL(clicked()), SLOT(done_dialog()));
    QPushButton *cancel = bbox->addButton(i18n("Cancel"));
    connect(cancel, SIGNAL(clicked()), SLOT(reject()));
    bbox->layout();
    tl->addSpacing(10);
    tl->addWidget(bbox);

    tl->freeze();
}

void SchedDialog::done_dialog()
{
    if(rb_rr->isChecked())
	out_policy = SCHED_RR;
    else if(rb_fifo->isChecked())
	out_policy = SCHED_FIFO;
    else
	out_policy = SCHED_OTHER;
    QString s(lined->text());
    bool ok;
    out_prio = s.toInt(&ok);
    if(out_policy != SCHED_OTHER && (!ok || out_prio < 1 || out_prio > 99)) {
	MessageDialog::message(i18n("Invalid input"),
			       i18n("The priority must be in the range 1..99"),
			       i18n("Cancel"), MessageDialog::warningIcon());
    } else
	accept();
}

void SchedDialog::button_clicked(int id)
{
    if(id == 0) {
	lbl->setEnabled(FALSE);
	lined->setEnabled(FALSE);
    } else {
	QString s(lined->text());
	bool ok;
	int n = s.toInt(&ok);
	if(ok && n == 0)
	    lined->setText("1");
	lbl->setEnabled(TRUE);
	lined->setEnabled(TRUE);

	lined->setFocus();
	lined->selectAll();
    }
}

