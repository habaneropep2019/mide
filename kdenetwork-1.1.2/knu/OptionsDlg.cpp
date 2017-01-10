/*
 *   OptionsDlg.cpp - Options window (widget)
 * 
 *   part of knu: KDE network utilities
 *
 *   Copyright (C) 1997  Bertrand Leconte
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * $Id: OptionsDlg.cpp,v 1.4 1998/09/23 16:24:11 bieker Exp $
 *
 * $Log: OptionsDlg.cpp,v $
 * Revision 1.4  1998/09/23 16:24:11  bieker
 * Use i18n() instead of _().
 *
 * Revision 1.3  1997/12/07 23:44:19  leconte
 * - handle the binary's name modification dynamicaly (problem reported
 *   by Conrad Sanderson)
 * - added browse button to the option dialog (for binary selection)
 * - code clean-up
 * - better fallback to"nslookup" if "host" is not found
 *
 * Revision 1.2  1997/11/23 22:28:10  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */

#include <kapp.h>

#include "knu.h"
#include "OptionsDlg.h"
#include "OptionsDlg.moc"

// This is the unit used to separate widgets
#define SEPARATION 10

// This is the caption string of the main application
extern QString CaptionString;

/**
 * Constructor
 */
OptionsDlg::OptionsDlg(CommandCfgDlg **cp, int pn,
		       QWidget* parent, const char* name)
  	: QDialog(parent, name, TRUE)
{
  int i;
  
  configPages = cp;
  pagesNumber = pn;

  // Get the caption of the main program and add configuration to it
  QString cs;
  cs += CaptionString;
  cs += i18n(" - configuration");
  setCaption(cs);

  // Layout
  layout1 = new QBoxLayout(this, QBoxLayout::TopToBottom, SEPARATION);
  CHECK_PTR(layout1);
  

  // Create the tabctrl widget
  cfgTabCtrl = new KTabCtl(this, "cfgTabCtrl");
  CHECK_PTR(cfgTabCtrl);

  for (i=0; i<pagesNumber; i++) {
    cfgTabCtrl->addTab(configPages[i]->makeWidget(cfgTabCtrl),
		       configPages[i]->tabName());
  }
  layout1->addWidget(cfgTabCtrl, 10);

  /*
   * buttons
   */
  
  // widgets
  bBtnOk = new QPushButton(i18n("&OK"), this, "button_ok");
  CHECK_PTR(bBtnOk);
  connect(bBtnOk, SIGNAL(clicked()), SLOT(slotOkBtn()));
  bBtnOk->setFixedSize(70, 30);
  bBtnOk->setDefault(TRUE);

  bBtnHelp = new QPushButton(i18n("&Help"), this, "button_help");
  CHECK_PTR(bBtnHelp);
  connect(bBtnHelp, SIGNAL(clicked()), SLOT(slotHelpBtn()));
  bBtnHelp->setFixedSize(70, 30);

  bBtnCancel = new QPushButton(i18n("&Cancel"), this, "button_cancel");
  CHECK_PTR(bBtnCancel);
  connect(bBtnCancel, SIGNAL(clicked()), SLOT(slotCancelBtn()));
  bBtnCancel->setFixedSize(70, 30);

  // layouts
  layoutB = new QBoxLayout(QBoxLayout::LeftToRight);
  CHECK_PTR(layoutB);
  layout1->addLayout(layoutB);
  layoutB->addWidget(bBtnOk);
  layoutB->addStretch(10);
  layoutB->addWidget(bBtnHelp);
  layoutB->addWidget(bBtnCancel);

  adjustSize();
  layout1->activate();
  adjustSize();
}

/**
 * Destructor
 */
OptionsDlg::~OptionsDlg()
{
  delete layout1;
  delete layoutB;
  for (int i=0; i<pagesNumber; i++) {
    configPages[i]->deleteWidget();
    delete configPages[i];
  }
  delete cfgTabCtrl;
  delete bBtnOk;
  delete bBtnHelp;
  delete bBtnCancel;
}


/**
 * callback for "ok" button
 */
void
OptionsDlg::slotOkBtn()
{
  for (int i=0; i<pagesNumber; i++) {
    configPages[i]->commitChanges();
  }

  emit accept();
}

/**
 * callback for "cancel" button
 */
void
OptionsDlg::slotCancelBtn()
{
  for (int i=0; i<pagesNumber; i++) {
    configPages[i]->cancelChanges();
  }
  emit reject();
}

/**
 * callback for "help" button
 */
void
OptionsDlg::slotHelpBtn()
{
  kapp->invokeHTMLHelp("", "config");
}

/**
 * Called when the window manager wants to close the window
 */
void 
OptionsDlg::closeEvent (QCloseEvent*)
{
  for (int i=0; i<pagesNumber; i++) {
    configPages[i]->cancelChanges();
  }
  emit reject();
}

