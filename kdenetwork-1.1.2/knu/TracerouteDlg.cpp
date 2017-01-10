/*
 *   TracerouteDlg.cpp - Dialog for the traceroute command
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
 * $Id: TracerouteDlg.cpp,v 1.9 1998/09/23 16:24:14 bieker Exp $
 *
 * $Log: TracerouteDlg.cpp,v $
 * Revision 1.9  1998/09/23 16:24:14  bieker
 * Use i18n() instead of _().
 *
 * Revision 1.8  1998/06/28 13:15:21  kalle
 * Fixing...
 * Improved RPM spec file
 * bumped package version number to 1.0pre
 *
 * Revision 1.7  1998/06/09 21:18:00  leconte
 * Bertrand: correction of bug #745 (reported by Duncan Haldane):
 * 	arguments are now added to the command line
 *
 * Revision 1.6  1998/03/01 19:30:20  leconte
 * - added a finger tab
 * - internal mods
 *
 * Revision 1.5  1998/01/03 16:40:06  kulow
 * corrected typos
 *
 * Revision 1.3  1997/12/01 21:11:03  leconte
 * Patches by <neal@ctd.comsat.com>
 *
 * Revision 1.2  1997/11/23 22:28:15  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */

#include <stdlib.h>
#include "TracerouteDlg.h"
#include "TracerouteDlg.moc"


// This is the unit used to separate widgets
#define SEPARATION 10

/*
 * Constructor
 */
TracerouteDlg::TracerouteDlg(QString commandName,
			     QWidget* parent, const char* name)
  : CommandDlg(commandName, parent, name)
{
  KConfig *kc = kapp->getConfig();

  layout1 = new QBoxLayout(commandBinOK, QBoxLayout::TopToBottom, SEPARATION);
  CHECK_PTR(layout1);
    
  layout2 = new QBoxLayout(QBoxLayout::LeftToRight, SEPARATION);
  CHECK_PTR(layout2);
  layout1->addLayout(layout2, 0);
    
  // Frame for options
  frame1 = new QFrame(commandBinOK, "frame_1");
  CHECK_PTR(frame1);
  frame1->setFrameStyle(QFrame::Box | QFrame::Sunken);
  frame1->setMinimumSize(0, 2*fontMetrics().height());
  layout1->addWidget(frame1, 0);
  
  layout3 = new QBoxLayout(frame1, QBoxLayout::LeftToRight, SEPARATION/2);
  CHECK_PTR(layout3);
  
  // Re-make the layout of CommandDlg
  layout2->addWidget(commandLbl1);
  layout2->addWidget(commandArgs);
  layout2->addSpacing(2*SEPARATION);
  layout2->addWidget(commandGoBtn);
  layout2->addWidget(commandStopBtn);
    
  // Layout of options
  layout3->addStretch(10);
  
  tracerouteCb1 = new QCheckBox(i18n("Make host &name resolution"), 
				frame1, "cb_1");
  tracerouteCb1->setChecked(TRUE);
  tracerouteCb1->adjustSize();
  tracerouteCb1->setFixedSize(tracerouteCb1->width(), 
			      2*fontMetrics().height());
  layout3->addWidget(tracerouteCb1, 0);
  
  layout3->addStretch(10);
  
  
  tracerouteLe2 = new QLineEdit(frame1, "LineEdit_2");
  tracerouteLe2->setMaxLength(3);
  tracerouteLe2->setText("30");
  tracerouteLe2->setFixedSize(fontMetrics().width("0000"), 
			      (int)(1.75*fontMetrics().height()));
  
  tracerouteLbl2 = new QLabel(tracerouteLe2, i18n("&Maximum number of hops:"), 
			      frame1, "Label_2"); 
  tracerouteLbl2->adjustSize();
  tracerouteLbl2->setFixedSize(tracerouteLbl2->size());
  
  layout3->addWidget(tracerouteLbl2, 0);
  layout3->addWidget(tracerouteLe2, 0);
  
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
TracerouteDlg::~TracerouteDlg()
{
}

/**
 * build the command line from widgets
 */
bool
TracerouteDlg::buildCommandLine(QString args)
{
  int     iMaxHops;
  QString sMaxHops;
  QString s, ss;
  KConfig *kc = kapp->getConfig();
  
  kc->setGroup(configGroupName);
  s = kc->readEntry("path");
  if (s.isEmpty()) {
    return FALSE;
  } else {
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
    
    // Check the Maximum hop count
    iMaxHops = atoi(tracerouteLe2->text());
    if (iMaxHops <=0) {
      iMaxHops = 30;
    }
    sMaxHops.sprintf("%d", iMaxHops);
    tracerouteLe2->setText(sMaxHops);
    
    if (!tracerouteCb1->isChecked()) {
      childProcess << "-n";
    }
    childProcess << "-m" << sMaxHops;
    childProcess << (const char *)args;
    return TRUE;
  }
}




