/*
 *   MtrDlg.cpp - Dialog for the mtr command
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
 * $Id: MtrDlg.cpp,v 1.3 1999/01/22 20:31:08 leconte Exp $
 *
 * $Log: MtrDlg.cpp,v $
 * Revision 1.3  1999/01/22 20:31:08  leconte
 * Preparing kde1.1
 * Knu should now be able to handle long responses from nslookup
 *
 * Revision 1.2  1998/10/29 21:26:28  leconte
 * Bertrand:
 * . renamed Matt s traceroute to mtr as asked by the new mtr maintainer
 * . mtr-0.25 has the diffs included. No more patch here
 * . updated knu for mtr-0.25
 *
 * Revision 1.1  1998/10/14 19:32:59  leconte
 * Bertrand: Added mtr support (with a patch to mtr-0.21)
 *
 */

#include <stdlib.h>
#include "MtrDlg.h"
#include "MtrDlg.moc"


// This is the unit used to separate widgets
#define SEPARATION 10

/*
 * Constructor
 */
MtrDlg::MtrDlg(QString commandName,
			     QWidget* parent, const char* name)
  : CommandDlg(commandName, parent, name)
{
  KConfig *kc = kapp->getConfig();

  layout1 = new QBoxLayout(commandBinOK, QBoxLayout::TopToBottom, SEPARATION);
  CHECK_PTR(layout1);
    
  layout2 = new QBoxLayout(QBoxLayout::LeftToRight, SEPARATION);
  CHECK_PTR(layout2);
  layout1->addLayout(layout2, 0);
    
#if 0
  // Frame for options
  frame1 = new QFrame(commandBinOK, "frame_1");
  CHECK_PTR(frame1);
  frame1->setFrameStyle(QFrame::Box | QFrame::Sunken);
  frame1->setMinimumSize(0, 2*fontMetrics().height());
  layout1->addWidget(frame1, 0);

  layout3 = new QBoxLayout(frame1, QBoxLayout::LeftToRight, SEPARATION/2);
  CHECK_PTR(layout3);
#endif

  // Create QListView
  commandTextArea->hide();	// Hide TextArea (not needed)
  lv = new QListView(commandBinOK, "qlistview");
  CHECK_PTR(lv);
  if (style() == WindowsStyle) {
    lv->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  } else {
    lv->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  }
  //  lv->addColumn(i18n("#"), fontMetrics().width("000 "));
  lv->addColumn(i18n("#"), 0);
  lv->addColumn(i18n("Hostname"), 10*fontMetrics().width("111."));
  lv->addColumn(i18n("Loss"), fontMetrics().width("0000 %"));
  lv->setColumnAlignment(2, AlignRight);
  lv->addColumn(i18n("Rcv"), fontMetrics().width("00000"));
  lv->setColumnAlignment(3, AlignRight);
  lv->addColumn(i18n("Snt"), fontMetrics().width("00000"));
  lv->setColumnAlignment(4, AlignRight);
  lv->addColumn(i18n("Best"), fontMetrics().width("000000"));
  lv->setColumnAlignment(5, AlignRight);
  lv->addColumn(i18n("Avg"), fontMetrics().width("000000"));
  lv->setColumnAlignment(6, AlignRight);
  lv->addColumn(i18n("Worst"), fontMetrics().width("000000"));
  lv->setColumnAlignment(7, AlignRight);
  lv->setAllColumnsShowFocus(TRUE);
  lv->setSorting(0);

  memset(lines, 0, 256 * sizeof(void *));
  numberOfLines = 0;

#if 0
  /*
   * This is for tests
   */
  QListViewItem *lvi;
  (void) new QListViewItem(lv, "1", "arthur1.tlse.club-internet.fr", "0 %",
			   "1", "2", "3", "4", "5");
  lvi =  new QListViewItem(lv, "2", "arthur2.tlse.club-internet.fr", "10 %",
			   "6", "7", "8", "9", "10");
  (void) new QListViewItem(lv, "3", "arthur3.tlse.club-internet.fr", "50 %",
			   "11", "12", "13", "14", "15");
  (void) new QListViewItem(lv, "4", "arthur4.tlse.club-internet.fr", "100 %",
			   "16", "17", "18", "19", "20");

  lvi->setText(5, "4321");
#endif

  QHeader *h = lv->header();
  h->setResizeEnabled(FALSE);
  h->setResizeEnabled(TRUE, 1);
  h->setMovingEnabled(FALSE);
  h->setClickEnabled(FALSE);


  // Re-make the layout of CommandDlg
  layout2->addWidget(commandLbl1);
  layout2->addWidget(commandArgs);
  layout2->addSpacing(2*SEPARATION);
  layout2->addWidget(commandGoBtn);
  layout2->addWidget(commandStopBtn);

#if 0    
  // Layout of options
  layout3->addStretch(10);
  
  mtrCb1 = new QCheckBox(i18n("Make host &name resolution"), 
				frame1, "cb_1");
  mtrCb1->setChecked(TRUE);
  mtrCb1->adjustSize();
  mtrCb1->setFixedSize(mtrCb1->width(), 
			      2*fontMetrics().height());
  layout3->addWidget(mtrCb1, 0);
     
  layout3->addStretch(10);
  layout3->activate();
#endif


  
  layout1->addWidget(lv, 10);
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
MtrDlg::~MtrDlg()
{
}

/**
 * build the command line from widgets
 */
bool
MtrDlg::buildCommandLine(QString args)
{
  int i;
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

#if 0    
    if (!mtrCb1->isChecked()) {
      childProcess << "-n";
    }
#endif
    childProcess << "--split";
    childProcess << (const char *)args;

    // Remove current lines in the QListView if needed
    for (i=numberOfLines-1; i>=0; i--) {
      delete lines[i];
      lines[i] = 0;
    }
    numberOfLines = 0;

    return TRUE;
  }
}



/**
 * Read the output of the command on a socket
 *
 * This is called by the main select loop in Qt (QSocketNotifier)
 */
void
MtrDlg::slotCmdStdout(KProcess *, char *buffer, int buflen)
{
  char *p, *q, *line, *pp;
  int  hop, i;
  QString *receivedLine;
  //  char *hostname, *loss, *rcv, *snt, *best, *avg, *worst;
  
  if (buflen <= 0) {
    receivedLine = new QString("--- nothing ---\n");
  } else {
    receivedLine = new QString(buffer, buflen+1);
  }
  //  debug("stdout> %s", (char *)*receivedLine);

  // Split incoming data by line
  //  p = malloc(receivedLine.length());
  p = receivedLine->data();
  while ((p != 0) && (*p != 0)) {
    line = p;
    q = p;
    while ((*q != '\n') && (*q != 0)) { q++; }
    if (*q == 0) {
      // This line is not complete.
debug("This line is not complete.");
      // So, what do I do???
    }
    *q = 0;
    q++;
    p = q;

    // Ok, now we have the current line in line.
    hop = atoi(line);
    //debug("HOP = %d; numberOfLines=%d", hop, numberOfLines);
    if (hop < 0) {
      // We have to remove this line
      if (-hop == numberOfLines) {
debug("deleting a line");
	delete lines[-hop-1];
	lines[-hop-1] = 0;
	numberOfLines--;
      } else {
debug("deleting a line not at end");
        // do not remove column #0 : it's the sort key. 
        lines[-hop-1]->setText(1, ""); 
        lines[-hop-1]->setText(2, ""); 
        lines[-hop-1]->setText(3, ""); 
        lines[-hop-1]->setText(4, ""); 
        lines[-hop-1]->setText(5, ""); 
        lines[-hop-1]->setText(6, ""); 
        lines[-hop-1]->setText(7, ""); 
      }
    } else if (hop != 0) {
      if (numberOfLines < hop) {
	// Add lines
	//debug("adding line; numberOfLines=%d, hop=%d", numberOfLines, hop);
	for (i=numberOfLines; i<hop; i++) {
	  char s[10];

	  //debug("- i=%d", i);
	  lines[i] = new QListViewItem(lv);
	  CHECK_PTR(lines[i]);
	  sprintf(s, "%03d", i+1);
	  lines[i]->setText(0, s);
	  lines[i]->setText(1, "???");
	  numberOfLines++;
	}
      }
 
      // Split the line to differents column
      //debug("line = \"%s\"", line);
      pp = strtok(line, " ");
      for (i=0; pp && *pp && i<8; i++) {
	if (i == 0) {
	  char s[10];
	  sprintf(s, "%03d", hop);
	  lines[hop-1]->setText(0, s);
	} else if (i == 2) {
	  char s[10];
	  sprintf(s, "%s %%", pp);
	  lines[hop-1]->setText(2, s);
	} else {
	  lines[hop-1]->setText(i, pp);
	}
	pp = strtok(0L, " ");
      }
      // Do we need to erase last columns? no.

      //      lines[hop-1]->setText(0, line);

    } else {
      // it was not a correct line. error?
      // TBD
      char s[10];
      numberOfLines++;
      lines[numberOfLines-1] = new QListViewItem(lv);
      CHECK_PTR(lines[numberOfLines-1]);
      sprintf(s, "%03d", numberOfLines-1);
      lines[numberOfLines-1]->setText(0, s);
      lines[numberOfLines-1]->setText(1, line);

      //      debug("it was not a correct line. error? = %s", line);
    }
  } // while
  delete receivedLine;
}

/**
 * install wait cursor on the main widget
 */
void
MtrDlg::installWaitCursor()
{
  lv->setCursor(waitCursor);
}

/**
 * reset wait cursor on the main widget
 */
void
MtrDlg::resetWaitCursor()
{
  lv->setCursor(arrowCursor);
}

/**
 * clear the output (slot)
 */
void
MtrDlg::clearOutput()
{
  int i;
  // Remove current lines in the QListView if needed
  for (i=numberOfLines-1; i>=0; i--) {
    delete lines[i];
    lines[i] = 0;
  }
  numberOfLines = 0;
}
