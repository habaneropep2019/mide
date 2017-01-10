/*
 *   FingerDlg.cpp - Dialog for the finger command
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
 * $Id: FingerDlg.cpp,v 1.4 1998/09/23 16:24:08 bieker Exp $
 *
 * $Log: FingerDlg.cpp,v $
 * Revision 1.4  1998/09/23 16:24:08  bieker
 * Use i18n() instead of _().
 *
 * Revision 1.3  1998/06/11 19:20:35  leconte
 * - some accelerators added
 * - strings added to i18n
 *
 * Revision 1.2  1998/06/09 21:18:03  leconte
 * Bertrand: correction of bug #745 (reported by Duncan Haldane):
 * 	arguments are now added to the command line
 *
 * Revision 1.1  1998/03/01 19:30:10  leconte
 * - added a finger tab
 * - internal mods
 *
 */

#include "FingerDlg.h"
#include "FingerDlg.moc"


// This is the unit used to separate widgets
#define SEPARATION 10

/*
 * Constructor
 */
FingerDlg::FingerDlg(QString commandName, 
		     QWidget* parent, const char* name)
  : CommandDlg(commandName, parent, name)
{
  KConfig *kc = kapp->getConfig();
  
  layout1 = new QBoxLayout(commandBinOK, 
			   QBoxLayout::TopToBottom, SEPARATION);
  CHECK_PTR(layout1);
  
  layout2 = new QBoxLayout(QBoxLayout::LeftToRight, SEPARATION);
  CHECK_PTR(layout2);
  layout1->addLayout(layout2, 0);
  
  // Frame for options
  frame1 = new QFrame(commandBinOK, "frame_1");
  CHECK_PTR(frame1);
  frame1->setFrameStyle(QFrame::Box | QFrame::Sunken);
  layout1->addWidget(frame1, 0);
  
  layout3 = new QBoxLayout(frame1, QBoxLayout::LeftToRight, SEPARATION/2);
  CHECK_PTR(layout3);
  
  // Make the layout of CommandDlg
  commandLbl1->setText(i18n("&User:"));
  commandLbl1->setFixedSize(commandLbl1->sizeHint());
  layout2->addWidget(commandLbl1);
  layout2->addWidget(commandArgs);
  layout2->addSpacing(2*SEPARATION);
  layout2->addWidget(commandGoBtn);
  layout2->addWidget(commandStopBtn);
  
  // Layout of options
  layout3->addStretch(10);
  
  fingerCb2 = new QComboBox(TRUE, frame1, "cb_1");
  fingerCb2->setMinimumSize(3*fontMetrics().width("----------"), 
			    2*fontMetrics().height());
  fingerCb2->setMaximumSize(QLayout::unlimited, 2*fontMetrics().height());

  fingerCb2->insertItem("localhost");

  fingerLbl2 = new QLabel(fingerCb2, i18n("Ser&ver:"), frame1, "Label_1");
  fingerLbl2->setFixedSize(fingerLbl2->sizeHint());

  layout3->addWidget(fingerLbl2, 0);
  layout3->addWidget(fingerCb2, 10);
  
  layout3->addStretch(10);
  layout3->activate();
  
  layout1->addWidget(commandTextArea, 10);
  layout1->activate();

  /*
   * Look at the command binary to see which widget to display
   */
  kc->setGroup(configGroupName);
  if (!kc->hasKey("path")) {
    // It's the first execution, 
    // so we have to search for the pathname
    kc->writeEntry("path", commandName);
    checkBinaryAndDisplayWidget();
    if (commandFound) {
      // All is OK : we can enable this tab.
      if (!kc->hasKey("enable")) {
	kc->writeEntry("enable", 1);
      }
    }
  } else {
    checkBinaryAndDisplayWidget();
  }
  
  // Commit changes in configfile (if needed)
  kc->sync();
}

/*
 * Destructor
 */
FingerDlg::~FingerDlg()
{
}

/**
 * build the command line from widgets
 */
bool
FingerDlg::buildCommandLine(QString args)
{
  QString s;
  KConfig *kc = kapp->getConfig();
  
  kc->setGroup(configGroupName);
  s = kc->readEntry("path");
  if (s.isEmpty()) {
    return FALSE;
  } else {
    QString as;
    //debug("getExecutable = %s", (const char *)s);
    childProcess.clearArguments();
    childProcess.setExecutable(s);

    // Add arguments
    s = (kc->readEntry("arguments")).simplifyWhiteSpace();

    if (!s.isEmpty()) {
      while (s.contains(' ', FALSE) != 0) {
	int pos = s.find(' ', 0, FALSE);
	childProcess << s.left(pos);
	s = s.remove(0, pos+1);
      }
      childProcess << s;
    }

    as.sprintf("%s@%s", (const char *)args, 
	       (const char *)fingerCb2->currentText());

    childProcess << (const char *)as;
    return TRUE;
  }
}




