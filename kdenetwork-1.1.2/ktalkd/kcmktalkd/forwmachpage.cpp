/*
 * forwmachpage.cpp - Forwarding machine settings for KTalkd
 *
 * Copyright (C) 1998 David Faure, faure@kde.org
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "forwmachpage.h"

#include <ksimpleconfig.h>
#include <kapp.h>

extern KSimpleConfig *config;

KForwmachPageConfig::KForwmachPageConfig( QWidget *parent, const char* name ) 
    : KConfigWidget (parent, name)
{
    forwmach_cb = new QCheckBox(i18n("Activate &forward"), this);
    forwmach_cb->adjustSize();
    address_edit = new QLineEdit(this);
    address_edit->adjustSize();
    address_edit->setMinimumWidth(150);
    address_label = new QLabel(address_edit,i18n("&Destination (user or user@host)"),this);
    address_label->adjustSize();
    address_label->setAlignment( ShowPrefix | AlignVCenter );

    method_combo = new QComboBox(this);
    method_combo->insertItem("FWA");
    method_combo->insertItem("FWR");
    method_combo->insertItem("FWT");
    method_combo->adjustSize();
    method_combo->setMinimumWidth(80);
    method_label = new QLabel(method_combo, i18n("Forward &method"),this);
    method_label->adjustSize();
    method_label->setAlignment( ShowPrefix | AlignVCenter );

    expl_label = new QLabel(i18n(
"FWA : Forward announcement only. Direct connection. Not recommended.\n\
FWR : Forward all requests, changing info when necessary. Direct connection.\n\
FWT : Forward all requests and take the talk. No direct connection.\n\
\n\
Recommended use : FWT it you want to use it behind a firewall (and if ktalkd\n\
can access both networks), and FWR otherwise.\n\
\n\
See Help for further explanations.\n\
"),this);
    expl_label->adjustSize();

    int h = 10 + forwmach_cb->height() + address_edit->height() +
        method_combo->height()+expl_label->height()+30;
    setMinimumSize(400, h); // 400 : otherwise, buttons may overlap

    loadSettings();

    connect(forwmach_cb, SIGNAL(clicked()), this, SLOT(forwmachOnOff()));
}

KForwmachPageConfig::~KForwmachPageConfig( ) {
    /* I've been told that this is not necessary as 
       they will be deleted by Qt. But, well... */
    delete forwmach_cb;
    delete address_label;
    delete address_edit;
    delete method_label;
    delete method_combo;
    delete expl_label;
}

void KForwmachPageConfig::resizeEvent(QResizeEvent *e) {
    KConfigWidget::resizeEvent(e);
    int h_txt = forwmach_cb->height(); // taken for the general label height
    int h_edt = address_edit->height();   // taken for the general QLineEdit height
    int spc = h_txt / 3;
    int w = rect().width();

    int h = 10 + spc*2;
    forwmach_cb->move(10, h);
    h += h_txt+spc;
    address_label->setFixedHeight(h_edt); // same size -> vertical center aligned
    address_label->move(10, h);
    int w_label = address_label->width()+20;
    address_edit->setGeometry(w_label, h, w-w_label-10, h_edt);
    h += h_edt+spc;

    method_label->setFixedHeight(h_edt); // same size -> vertical center aligned
    method_label->move(10, h);
    method_combo->move(w_label, h);
    h += h_edt+spc;

    expl_label->move(10,h);
    
}

void KForwmachPageConfig::forwmachOnOff() {
    bool b = forwmach_cb->isChecked();
    address_label->setEnabled(b);
    address_edit->setEnabled(b);
    method_label->setEnabled(b);
    method_combo->setEnabled(b);
    expl_label->setEnabled(b);
}

void KForwmachPageConfig::defaultSettings() {

    forwmach_cb->setChecked(false);
    method_combo->setCurrentItem(1);
    address_edit->setText("");

    // Activate things according to configuration
    forwmachOnOff();

}

void KForwmachPageConfig::loadSettings() {
   
    config->setGroup("ktalkd");

    QString forward = config->readEntry("Forward","unset");
    forwmach_cb->setChecked(forward!="unset");
    address_edit->setText(forward);

    QString forwardMethod = config->readEntry("ForwardMethod","");
    for (int i=0; i<method_combo->count(); i++)
        if (!strcmp(forwardMethod, method_combo->text(i)))
            method_combo->setCurrentItem(i);

    // Activate things according to configuration
    forwmachOnOff();
}

void KForwmachPageConfig::saveSettings() {

    config->setGroup("ktalkd");

    if (forwmach_cb->isChecked())
    {
        config->writeEntry("Forward",address_edit->text());
    } else
        config->deleteEntry("Forward", false /*non localized*/);
    config->writeEntry("ForwardMethod",method_combo->currentText());
    
    config->sync();
}

void KForwmachPageConfig::applySettings() {
    saveSettings();
}
