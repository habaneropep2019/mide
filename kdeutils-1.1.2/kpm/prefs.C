// prefs.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997, 1998

#include "prefs.h"
#include "qps.h"

#include <qpushbt.h>
#include <qaccel.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qlayout.h>
#include <kapp.h>
#include "buttongroup.h"

// KDE i18n only works after an instance of 
// KApplication has been created, but xgettext requires a
// keyword for translation. The following hack ensures
// that xgettext recognises the text as locale message,
// but the translation is done in the constructor
#undef TRANS
#define TRANS(x) x

struct Boxvar
{
    char *text;
    bool *variable;
    QCheckBox *cb;
};

  Boxvar statbar_boxes[] = {
    {TRANS("Graphic Load Display"), &Qps::show_load_graph},
    {TRANS("Graphic CPU Display"), &Qps::show_cpu_bar},
    {TRANS("Graphic Memory Display"), &Qps::show_mem_bar},
    {TRANS("Graphic Swap Display"), &Qps::show_swap_bar},
    {TRANS("Load Graph in Icon"), &Qps::load_in_icon},
    {TRANS("Dock into kpanel"), &Qps::panel_docking},
    {0, 0}
};

  Boxvar appear_boxes[] = {
    {TRANS("Green/Black Load Graph"), &Qps::phosphor_graph},
    //    {TRANS("Old Style Tables"), &Qps::old_style_tables},
    {TRANS("Solid filled graph"), &Qps::solid_graph},
    {0, 0}
};

   Boxvar sockinfo_boxes[] = {
    {TRANS("Host Name Lookup"), &Qps::hostname_lookup},
    {0, 0}
};

   Boxvar misc_boxes[] = {
    {TRANS("Auto Save Settings on Exit"), &Qps::auto_save_options},
    //    {TRANS("Set X11 Selection to PIDs"), &Qps::pids_to_selection},
    {0, 0}
};

struct Cbgroup
{
    char *caption;
    Boxvar *boxvar;
};

static Cbgroup groups[] = {
    {TRANS("Status Bar"), statbar_boxes},
    {TRANS("Appearance"), appear_boxes},
    {TRANS("Socket Info Window"), sockinfo_boxes},
    {TRANS("Miscellaneous"), misc_boxes},
    {0, 0}
};

// redefine TRANS
#undef TRANS
#define TRANS klocale->translate

Preferences::Preferences()
           : QDialog(0)
{
    setCaption("Preferences");
    QVBoxLayout *tl = new QVBoxLayout(this, 10);
    
    const int border_x = 10;
    int min_x = 0;

    for(Cbgroup *g = groups; g->caption; g++) {
	ButtonGroup *grp = new ButtonGroup(TRANS(g->caption), this);
	int box_y = (int)(fontMetrics().lineSpacing() * 1.5);
	for(Boxvar *b = g->boxvar; b->text; b++) {
	    b->cb = new QCheckBox(TRANS(b->text), grp);
	    b->cb->adjustSize();
	    b->cb->move(border_x, box_y);
	    min_x = QMAX(min_x, b->cb->sizeHint().width());
	    box_y += b->cb->sizeHint().height();
	}
	connect(grp, SIGNAL(clicked(int)), SLOT(update_reality(int)));
	grp->setMinimumWidth(min_x + 2*border_x);
	grp->setFixedHeight(box_y + 5);
	tl->addWidget(grp);
    }
    
    update_boxes();

    QPushButton *closebut = new QPushButton(TRANS("Close"), this);
    closebut->setDefault(TRUE);
    closebut->adjustSize();
    closebut->setFixedSize(closebut->size());
    connect(closebut, SIGNAL(clicked()), SLOT(closed()));
    tl->addSpacing(10);
    tl->addWidget(closebut);

    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(ALT + Key_W),
		     this, SLOT(closed()));

    tl->freeze();
}

// update check boxes to reflect current status
void Preferences::update_boxes()
{
    for(Cbgroup *g = groups; g->caption; g++)
	for(Boxvar *b = g->boxvar; b->text; b++)
	    b->cb->setChecked(*b->variable);
}

// update flags and repaint to reflect state of check boxes
void Preferences::update_reality(int)
{
    for(Cbgroup *g = groups; g->caption; g++)
	for(Boxvar *b = g->boxvar; b->text; b++)
	    *b->variable = b->cb->isChecked();
    emit prefs_change();
}

void Preferences::closed()
{
    hide();
}
