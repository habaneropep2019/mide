/* -*- C++ -*-
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: acctselect.cpp,v 1.12 1999/01/17 19:56:20 bieker Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <qdir.h>
#include <qgrpbox.h>
#include <qlayout.h>
#include <qdir.h>
#include <kapp.h>
#include <qregexp.h>
#include <qwmatrix.h>
#include <stdio.h>

#include "macros.h"
#include "acctselect.h"
#include "pppdata.h"

AccountingSelector::AccountingSelector(QWidget *parent, bool _isnewaccount, const char *name)
  : QWidget(parent, name),
    isnewaccount(_isnewaccount)
{
  QGridLayout *l = new QGridLayout(this, 3, 3, 10, 10);
  l->addRowSpacing(0, fontMetrics().lineSpacing() - 10);
  QGroupBox *box = new QGroupBox(this,"box");
  box->setTitle(i18n("Accounting Setup"));
  l->addMultiCellWidget(box, 0, 2, 0, 2);

  QVBoxLayout *l1 = new QVBoxLayout(0);
  l->addLayout(l1, 1, 1);
  l1->addSpacing(10);

  // checkbox for enabling/disabling accounting
  use = new QCheckBox(i18n("Enable accounting"), this);
  use->setChecked(gpppdata.AcctEnabled());
  use->setMinimumSize(use->sizeHint());
  l1->addWidget(use, 0);
  l1->addSpacing(10);
  connect(use, SIGNAL(toggled(bool)), this, SLOT(enableItems(bool)));

  // insert the tree widget
  tl = new KTreeList(this);
  connect(tl, SIGNAL(selected(int)),
	  this, SLOT(slotHighlighted(int)));
  tl->setMinimumSize(220, 200);
  l1->addWidget(tl, 1);

  // label to display the currently selected ruleset
  QHBoxLayout *l11 = new QHBoxLayout;
  l1->addSpacing(10);
  l1->addLayout(l11);
  QLabel *lsel = new QLabel(i18n("Selected:"), this);
  lsel->setMinimumSize(lsel->sizeHint());
  selected = new QLabel(this);
  selected->setFrameStyle(QFrame::Sunken | QFrame::WinPanel);
  selected->setLineWidth(1);
  selected->setFixedHeight(selected->sizeHint().height() + 16);
  l11->addWidget(lsel, 0);
  l11->addSpacing(10);
  l11->addWidget(selected, 1);

  // volume accounting
  l1->addStretch(1);
  QHBoxLayout *l12 = new QHBoxLayout;
  l1->addLayout(l12);
  QLabel *usevol_l = new QLabel(i18n("Volume accounting:"),
				this);
  MIN_SIZE(usevol_l);
  use_vol = new QComboBox(this);
  use_vol->insertItem(i18n("No accounting"), 0);
  use_vol->insertItem(i18n("Bytes in"), 1);
  use_vol->insertItem(i18n("Bytes out"), 2);
  use_vol->insertItem(i18n("Bytes in and out"), 3);
  MIN_SIZE(use_vol);
  use_vol->setCurrentItem(gpppdata.VolAcctEnabled());
  l12->addWidget(usevol_l);
  l12->addWidget(use_vol);

  // load the pmfolder pixmap from KDEdir
  QString fname = KApplication::kde_datadir().copy();
  fname += "/kppp/pics/folder.xpm";
  pmfolder.load(fname.data());

  // scale the pixmap
  if(pmfolder.width() > 0) {
    QWMatrix wm;
    wm.scale(16.0/pmfolder.width(), 16.0/pmfolder.width());
    pmfolder = pmfolder.xForm(wm);
  }

  // load the pmfolder pixmap from KDEdir
  fname = KApplication::kde_datadir().copy();
  fname += "/kppp/pics/phone.xpm";
  pmfile.load(fname.data());

  // scale the pixmap
  if(pmfile.width() > 0) {
    QWMatrix wm;
    wm.scale(16.0/pmfile.width(), 16.0/pmfile.width());
    pmfile = pmfile.xForm(wm);
  }

  choice = -1;

  setupTreeWidget();

  l->setColStretch(0, 0);
  l->setColStretch(2, 0);
  l->setRowStretch(0, 0);
  l->setRowStretch(2, 0);
  l->setRowStretch(1, 1);
  l->setColStretch(1, 1);
  l->activate();
}


QString AccountingSelector::fileNameToName(QString s) {

  s.replace(QRegExp("_"), " ");
  return s;

}


QString AccountingSelector::nameToFileName(QString s) {

  s.replace(QRegExp(" "), "_");
  return s;

}


QString AccountingSelector::indexToFileName(int idx) {

  KTreeListItem *tli = tl->itemAt(idx);
  if(tli == NULL)
    return QString("");
  else {
    QString name = nameToFileName(QString(tli->getText()));
    while(tli->hasParent()) {
      tli = tli->getParent();
      QString s = tli->getText();
      if(s != i18n("Available rules"))
	name = nameToFileName(s) + "/" + name;
    }
    return QString(name+".rst");
  }
}


KTreeListItem *AccountingSelector::findByName(KTreeListItem *start,
					      QString name)
{
  KTreeListItem *ch = start;
  while(ch) {
    if(ch->getText() == name)
      return ch;
    ch = ch->getSibling();
  }
  return NULL;
}


void AccountingSelector::insertDir(QDir d, KTreeListItem *root) {

  // sanity check
  if(!d.exists() || !d.isReadable())
    return;


  // set up filter
  d.setNameFilter("*.rst");
  d.setFilter(QDir::Files);
  d.setSorting(QDir::Name);

  // read the list of files
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;

  // traverse the list and insert into the widget
  while((fi = it.current()) != NULL) {
    ++it;

    QString samename = fi->fileName();
    KTreeListItem *i = findByName(root->getChild(), samename);

    // skip this file if already in tree
    if(i != NULL)
      continue;

    // check if this is the file we should mark
    QString name = fileNameToName(fi->baseName());
    KTreeListItem *tli = new KTreeListItem(name.data(), &pmfile);

    // check if this is the item we are searching for
    // (only in "Edit"-mode, not in "New"-mode
    if(!isnewaccount &&
       (edit_s == QString(fi->filePath()).right(edit_s.length()))) {
      edit_item = tli;
    }
    root->appendChild(tli);
  }

  // set up a filter for the directories
  d.setFilter(QDir::Dirs);
  d.setNameFilter("*");
  const QFileInfoList *dlist = d.entryInfoList();
  QFileInfoListIterator dit(*dlist);
  while((fi = dit.current()) != NULL) {
    // skip "." and ".." directories
    if(fi->fileName().left(1) != ".") {
      // convert to human-readable name
      QString name = fileNameToName(fi->fileName());

      // if the tree already has an item with this name,
      // skip creation and use this one, otherwise
      // create a new entry
      KTreeListItem *i = findByName(root->getChild(), name);
      if(i == NULL) {
	KTreeListItem *item = new KTreeListItem(name.data(), &pmfolder);
	root->appendChild(item);
	insertDir(QDir(fi->filePath()), item);
      } else
	insertDir(QDir(fi->filePath()), i);
    }
    ++dit;
  }
}


/// expands a branch of the treelist
void expandBranch(KTreeList *tl, KTreeListItem *root) {

  if(root != NULL) {
    expandBranch(tl, root->getParent());
    tl->expandItem(tl->itemIndex(root));
  }
}

void AccountingSelector::setupTreeWidget() {
  // search the item
  edit_item = NULL;
  if(!isnewaccount)
    edit_s = gpppdata.accountingFile();
  else
    edit_s = "";

  KTreeListItem *i = new KTreeListItem(i18n("Available rules"), &pmfolder);
  tl->setExpandButtonDrawing(TRUE);
  tl->setTreeDrawing(TRUE);
  tl->insertItem(i, -1, FALSE);


  QString s = QDir::homeDirPath() + "/";
  s += ACCOUNTING_PATH "/Rules/";

  insertDir(QDir(s.data()), i);

  // look in $KDEDIR/lib/kppp/Accounting
  s = KApplication::kde_datadir().copy();
  s += "/kppp/Rules/";
  insertDir(QDir(s.data()), i);

  enableItems(FALSE);

  // when mode is "Edit", then hightlight the
  // appropriate item, otherwise expand
  // one level of the tree
  if(!isnewaccount) {
    expandBranch(tl, edit_item);
    tl->setCurrentItem(tl->itemIndex(edit_item));
    slotHighlighted(tl->itemIndex(edit_item));
  } else
    tl->setExpandLevel(1);
}


void AccountingSelector::enableItems(bool) {

  tl->setEnabled(use->isChecked());

  if(!use->isChecked() || (choice == -1))
    selected->setText(i18n("(none)"));
  else {
    // replace underscores
    QString s = indexToFileName(choice);
    s = s.replace(QRegExp("_"), " ");

    // remove .rst extension
    s = s.left(s.length()-4);
    if( 0 == (s.find('/')))
	s = s.right(s.length() -1);
    selected->setText(s.data());

  }
}


void AccountingSelector::slotHighlighted(int idx) {

  KTreeListItem *tli = tl->itemAt(idx);

  if(tli == NULL)
    return;

  if(tli->hasChild())
    return;

  if(tli->getPixmap()->serialNumber() == pmfolder.serialNumber())
    return;

  choice = idx;
  enableItems(TRUE);

}


bool AccountingSelector::save() {

  if(use->isChecked() && (choice != -1)) {

    QString s = indexToFileName(choice);
    gpppdata.setAccountingFile(s.data());
    gpppdata.setAcctEnabled(TRUE);
  } else {
    gpppdata.setAccountingFile("");
    gpppdata.setAcctEnabled(FALSE);
  }

  gpppdata.setVolAcctEnabled(use_vol->currentItem());

  return TRUE;
}

#include "acctselect.moc"

