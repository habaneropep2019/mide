/*
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: ppplog.cpp,v 1.13.2.1 1999/04/10 16:11:23 porten Exp $
 *
 * (c) 1998 Mario Weilguni <mweilguni@kde.org>
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
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
#include <qglobal.h>
#include <qstring.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <qstrlist.h>
#include <qdialog.h>
#include <kbuttonbox.h>

#include <qglobal.h>
#if QT_VERSION < 140
#include <qmlined.h>
#else
#include <qmultilinedit.h>
#endif

#include <qlayout.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <qregexp.h>

#include "macros.h"
#include "pppdata.h"
#include "requester.h"


int PPPL_MakeLog(QStrList &list) {
  int pid = -1, newpid;
  char buffer[1024], *p;
  const char *pidp;
  int fd;

  fd = Requester::rq->openSysLog();
  if(fd < 0) {
    list.append("Cannot open logfile!");
    return 1;
  }

  FILE *f = fdopen(fd, "r");
  while(fgets(buffer, sizeof(buffer), f) != 0) { 
    // pppd line ?
    p = (char *)strstr(buffer, "pppd[");
    if(p == 0)
      continue;
    pidp = p += strlen("pppd[");
    while(*p && isdigit(*p))
      p++;
    if(*p != ']')
      continue;

    /* find out pid of pppd */
    sscanf(pidp, "%d", &newpid);
    if(newpid != pid) {
      pid = newpid;
      list.clear();
    }
    if(buffer[strlen(buffer)-1] == '\n')
      buffer[strlen(buffer)-1] = '\0';
    list.append(buffer);
  }
  close(fd);

  if(list.isEmpty())
    return 2;

  /* clear security related info */
  list.first();

  char *keyword[] = {"name = \"",
		    "user=\"",
		    "password=\"",
		    0};

  while(list.current() != 0) {
    for(int j = 0; keyword[j] != 0; j++) {
      char *p;

      if( (p = (char *)strstr(list.current(), keyword[j])) != 0) {
	p += strlen(keyword[j]);
	while(*p && *p != '"')
	  *p++ = 'X';
      }
    }

    list.next();
  }

  return 0;
}


void PPPL_AnalyseLog(QStrList &list, QStrList &result) {
  QString msg;
  const char *rmsg = "Remote message: ";

  result.clear();

  // setup the analysis database
  struct {
    const char *regexp;
    const char *answer;
  } hints[] = {
    {"Receive serial link is not 8-bit clean",
     i18n("You have launched pppd before the remote server " \
	  "was ready to establish a PPP connection.\n"
	  "Please use the terminal-based login to verify") },

    {"Serial line is looped back", 
     i18n("You haven't started the PPP software on the peer system.") },
    
    {"AP authentication failed",
     i18n("Check that you supplied the correct username and password!")} ,
    
    {"is locked by pid",
     i18n("You shouldn't pass 'lock' as an argument to pppd. "
	  "Check /etc/ppp/options and ~/.ppprc") },
    
    {"CP: timeout sending",
     i18n("The remote system does not seem to answer to\n"
	  "configuration request! Contact your provider!") },

    {"unrecognized option",
     i18n("You have passed an invalid option to pppd. See `man pppd' "
          "for a complete list of valid arguments.") },

    // terminator
    {0,0}
  };
    

  // scan the log for keywords and try to offer any help
  for(char *line = list.first(); line; line = list.next()) {
    // look for remote message      
    char *p = strstr(line, rmsg);
    if(p) {
      p += strlen(rmsg);
      if(strlen(p)) {
        msg.sprintf(i18n("Notice that the remote system has sent the following"
                         " message:\n\"%s\"\nThis may give you a hint why the"
                         " the connection has failed."), p);
        result.append(msg.data());
      }
    }

    // check in the hint database
    for(uint k = 0; hints[k].regexp != 0; k++) {
      QRegExp rx(hints[k].regexp);
      QString l(line);
      if(l.contains(rx)) {
	result.append(hints[k].answer);
	break;
      }
    }
  }

  if (result.isEmpty())
    result.append(i18n("Sorry. Can't help you here."));
}


void PPPL_ShowLog() {
  QStrList sl, result;

  PPPL_MakeLog(sl);

  bool foundLCP = gpppdata.getPPPDebug();
  for(uint i = 0; !foundLCP && i < sl.count(); i++)
    if(strstr(sl.at(i), "[LCP") != 0)
      foundLCP = TRUE;

  if(!foundLCP) {
    int result = KMsgBox::yesNo(0,
				i18n("Warning"),
				i18n("KPPP could not prepare a PPP log. It´s very likely\n"
				     "that pppd was started without the \"debug\" option.\n\n"
				     "Without this option it´s difficult to find out PPP\n"
				     "problems, so you should turn on the debug option.\n\n"
				     "Shall I turn it on now?"),
				KMsgBox::QUESTION);

    if(result == 1) {
      gpppdata.setPPPDebug(TRUE);
      KMsgBox::message(0,
		       i18n("Information"),
		       i18n("The \"debug\" option has been added. You\n"
			    "should now try to reconnect. If that fails\n"
			    "again, you will get a PPP log that may help\n"
			    "you to track down the connection problem."),
		       KMsgBox::INFORMATION);
      //      return;
    }
    
    //    return;
  }

  PPPL_AnalyseLog(sl, result);

  QDialog *dlg = new QDialog(0, "", TRUE);

  dlg->setCaption(i18n("PPP log"));
  QVBoxLayout *tl = new QVBoxLayout(dlg, 10, 10);
  QMultiLineEdit *edit = new QMultiLineEdit(dlg);
  edit->setReadOnly(TRUE);
  QLabel *label = new QLabel(i18n("kppp's diagnosis (just guessing):"), dlg);
  QMultiLineEdit *diagnosis = new QMultiLineEdit(dlg);
  diagnosis->setReadOnly(TRUE);
  KButtonBox *bbox = new KButtonBox(dlg);
  bbox->addStretch(1);
  QPushButton *write = bbox->addButton(i18n("Write to file"));
  QPushButton *close = bbox->addButton(i18n("Close"));
  bbox->layout();
  edit->setMinimumSize(600, 250);
  label->setMinimumSize(600, 15);
  diagnosis->setMinimumSize(600, 60);

  tl->addWidget(edit, 1);
  tl->addWidget(label);
  tl->addWidget(diagnosis, 1);
  tl->addWidget(bbox);
  tl->freeze();

  for(uint i = 0; i < sl.count(); i++)
    edit->append(sl.at(i));
  for(uint i = 0; i < result.count(); i++)
    diagnosis->append(result.at(i));
  
  dlg->connect(close, SIGNAL(clicked()),
	       dlg, SLOT(reject()));
  dlg->connect(write, SIGNAL(clicked()),
	       dlg, SLOT(accept()));

  if(dlg->exec()) {
    QDir d = QDir::home();
    QString s = d.absPath() + "/PPP-logfile";
    int old_umask = umask(0077);
    
    FILE *f = fopen(s.data(), "w");
    for(uint i = 0; i < sl.count(); i++)
      fprintf(f, "%s\n", sl.at(i));
    fclose(f);
    umask(old_umask);
    
    QString msg;
    msg.sprintf("The PPP log has been saved\nas \"%s\"!\n\nIf you want to send a bug report or have\nproblems connecting to the internet, please\nattach this file. It will help the maintainers\nto find the bug and to improve KPPP", s.data());
    KMsgBox::message(0,
		     i18n("Information"),
		     msg.data(),
		     KMsgBox::INFORMATION);
  }
  delete dlg;
}
