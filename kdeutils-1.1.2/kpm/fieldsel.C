// fieldsel.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <qpushbt.h>
#include <qlabel.h>
#include <qaccel.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <kapp.h> // for i18n
#include "fieldsel.h"

FieldSelect::FieldSelect(Procview *pv, Proc *proc)
            : QDialog(0, "select fields"),
	      nbuttons(proc->cats.size()),
	      disp_fields(nbuttons),
	      procview(pv)
{
    QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

    updating = FALSE;
    setCaption("qps: select fields");
    buts = new QCheckBox*[nbuttons];

    QGridLayout *l1 = new QGridLayout((nbuttons + 1) / 2, 5, 0);
    tl->addLayout(l1, 1);
    l1->addColSpacing(2, 15);

    int half = (nbuttons + 1) / 2;
    for(int i = 0; i < nbuttons; i++) {
      QCheckBox *but = new QCheckBox(proc->cats[i]->name, this);
	QLabel *desc = new QLabel(proc->cats[i]->help, this);
	but->setMinimumSize(but->sizeHint());
	desc->setMinimumSize(desc->sizeHint());

	if(i < half) {
	  l1->addWidget(but, i, 0);
	  l1->addWidget(desc, i, 1);
	} else {
	  l1->addWidget(but, i-half, 3);
	  l1->addWidget(desc, i-half, 4);
	}
	buts[i] = but;
	connect(but, SIGNAL(toggled(bool)), this, SLOT(field_toggled(bool)));
    }
    update_boxes();

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    QPushButton *closebut = bbox->addButton(i18n("Close"));
    closebut->setDefault(TRUE);
    closebut->setFocus();
    closebut->setFixedSize(closebut->sizeHint());
    bbox->layout();
    tl->addWidget(bbox);

    connect(closebut, SIGNAL(clicked()), SLOT(closed()));

    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(CTRL + Key_W),
		     this, SLOT(closed()));
    tl->freeze();
}

// one of the fields was toggled (we don't know which one yet)
void FieldSelect::field_toggled(bool)
{
    if(updating) return;
    set_disp_fields();
    for(int i = 0; i < nbuttons; i++)
	if(buts[i]->isChecked() != disp_fields.testBit(i))
	    if(buts[i]->isChecked())
		emit added_field(i);
	    else
		emit removed_field(i);
}

void FieldSelect::closed()
{
    hide();
}

void FieldSelect::closeEvent(QCloseEvent *)
{
    closed();
}

void FieldSelect::set_disp_fields()
{
    disp_fields.fill(FALSE);
    int n = procview->cats.size();
    for(int i = 0; i < n; i++)
	disp_fields.setBit(procview->cats[i]->index);
}

void FieldSelect::update_boxes()
{
    set_disp_fields();
    updating = TRUE;
    for(int i = 0; i < nbuttons; i++)
	buts[i]->setChecked(disp_fields.testBit(i));
    updating = FALSE;
}
