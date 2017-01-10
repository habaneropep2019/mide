//---------------------------------------------------------------------------
//
//             kPPP: A pppd front end for the KDE project
//
//---------------------------------------------------------------------------
//
// (c) 1997-1998 Bernd Johannes Wuebben <wuebben@kde.org>
// (c) 1997-1999 Mario Weilguni <mweilguni@kde.org>
// (c) 1998-1999 Harri Porten <porten@kde.org>
//
// derived from Jay Painters "ezppp"
//
//---------------------------------------------------------------------------
//  
//  $Id: modemdb.cpp,v 1.3.2.1 1999/06/13 17:56:51 porten Exp $
//
//---------------------------------------------------------------------------
// 
//  This program is free software; you can redistribute it and-or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
// 
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the Free
//  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//---------------------------------------------------------------------------

#include <qlabel.h>
#include <qlistbox.h>
#include <kbuttonbox.h>
#include <qlayout.h>
#include <kapp.h>
#include "modemdb.h"
#include "log.h"
#include "newwidget.h"

ModemSelector::ModemSelector(QWidget *parent) : QDialog(parent, 0, true) {
  // set up widgets and such
  setCaption(i18n("Select modem type..."));
  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
  QLabel *l1 = new QLabel(i18n("To setup your modem, first choose its vendor in the "
			       "list to the left, and then select the model from the "
			       "right list. If you don´t know which modem you have, "
			       "you can try out one of the \"Generic\" modems."),
			this);
  l1->setAlignment(AlignLeft | WordBreak);
  l1->setFixedWidth(400);
  l1->setMinimumHeight(50);
  tl->addWidget(l1, 0);

  tl->addSpacing(10);

  QHBoxLayout *tl1 = new QHBoxLayout(10);
  tl->addLayout(tl1, 1);
  vendor = new QListBox(this);
  model  = new QListBox(this);
  vendor->setMinimumSize(200, 130);
  model->setMinimumSize(200, 130);
  tl1->addWidget(vendor, 2);
  tl1->addWidget(model, 3);

  KButtonBox *bbox = new KButtonBox(this);
  bbox->addStretch(1);
  ok = bbox->addButton(i18n("OK"));
  ok->setDefault(true);
  ok->setEnabled(false);
  cancel =   bbox->addButton(i18n("Cancel"));
  bbox->layout();
  tl->addWidget(bbox);
  tl->freeze();

  // setup modem database
  db = new ModemDatabase();

  // set up signal/slots
  connect(ok, SIGNAL(clicked()),
	  this, SLOT(reject()));
  connect(cancel, SIGNAL(clicked()),
	  this, SLOT(reject()));
  connect(vendor, SIGNAL(highlighted(int)),
	  this, SLOT(vendorSelected(int)));
  connect(model, SIGNAL(highlighted(int)),
	  this, SLOT(modelSelected(int)));
  connect(model, SIGNAL(selected(int)),
	  this, SLOT(selected(int)));

  // fill vendor list with life
  const QStrList *lvendor = db->vendors();
  for(uint i = 0; i < lvendor->count(); i++)
    vendor->insertItem(((QStrList *)lvendor)->at(i));

  vendor->setCurrentItem(0);
}


ModemSelector::~ModemSelector() {
  delete db;
}


void ModemSelector::vendorSelected(int idx) {
  ok->setEnabled(false);

  QString name = vendor->text(idx);
  QStrList *models = db->models(name);
  model->clear();
  for(uint i = 0; i < models->count(); i++)
    model->insertItem(models->at(i));
  
  // FIXME: work around Qt bug
  if(models->count() == 0)
    model->update();
  delete models;
}


void ModemSelector::modelSelected(int) {
  ok->setEnabled(true);
}

void ModemSelector::selected(int) {
  accept();
}


ModemDatabase::ModemDatabase() {  
  load();
}


ModemDatabase::~ModemDatabase() {
  delete lvendors;
  delete modemDB;
}


const QStrList *ModemDatabase::vendors() {
  return lvendors;
}


QStrList *ModemDatabase::models(QString vendor) {
  QStrList *sl = new QStrList;  
  QString s = i18n("<Generic>");
  if(vendor == s)
    vendor = i18n("<Generic>");

  for(uint i = 0; i < modems.count(); i++) {
    CharDict *dict = modems.at(i);
    if(dict->find("Vendor") != 0) {
      if(vendor == (*dict)["Vendor"] && (*dict)["Name"][0] != '!')
	sl->inSort((*dict)["Name"]);
    }
  }

  return sl;
}


void ModemDatabase::loadModem(const char *key, CharDict &dict) {
  KEntryIterator *it = modemDB->entryIterator(key);
  KEntryDictEntry *e;

  // remove parent attribute
  dict.setAutoDelete(true);
  dict.remove("Parent");
  
  e = it->current();
  while((e = it->current()) != 0) {
    if(dict.find(it->currentKey()) == 0) {
      char *value = new char[strlen(e->aValue.data())+1];
      strcpy(value, e->aValue.data());
      dict.insert(it->currentKey(), value);
    }
    ++(*it);
  }

  // check name attribute
  if(dict["Name"] == 0 || key[0]=='!') {
    char *name = new char[strlen(key)+1];
    strcpy(name, key);
    dict.replace("Name", name);
  }

  // check parent attribute
  if(dict["Parent"] != 0)
    loadModem(dict["Parent"], dict);
  else 
    // inherit common at last
    if(strcmp(key, "Common") != 0)
      loadModem("Common", dict);

  delete it;
}


void ModemDatabase::load() {
  modemDB = new KConfig("DB/modemDB.rc", 0);
  lvendors = new QStrList;
  lvendors->setAutoDelete(true);
  modems.setAutoDelete(true);

  KGroupIterator *it = modemDB->groupIterator();
  while(it->current()) {
    modemDB->setGroup(it->currentKey());
    CharDict *c = new CharDict;
    c->setAutoDelete(true);
    loadModem(it->currentKey(), *c);

    if(strcmp(it->currentKey(), "Common") == 0) {
      QString s = i18n("Hayes(tm) compatible modem");
      char *name = new char[s.length()+1];
      strcpy(name, s.data());
      c->replace("Name", name);

      s = i18n("<Generic>");
      char *vend = new char[s.length()+1];
      strcpy(vend, s.data());
      c->replace("Vendor", vend);
    }
    modems.append(c);

    if(modemDB->hasKey("Vendor")) {
      QString vendor = modemDB->readEntry("Vendor");
      if(lvendors->find(vendor.data()) == -1)
	lvendors->inSort(vendor.data());
    }
    ++(*it);
  }

  lvendors->insert(0, i18n("<Generic>"));

  delete it;
}


void ModemDatabase::save(KConfig *) {
}

#include "modemdb.moc"
