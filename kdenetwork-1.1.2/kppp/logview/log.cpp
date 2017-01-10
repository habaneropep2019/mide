/*
 * kPPPlogview: a accounting log system for kPPP
 *
 *            Copyright (C) 1998 Mario Weilguni <mweilguni@kde.org>
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
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <kapp.h>
#include <qfileinf.h>
#include <qprogdlg.h>

QList<LogInfo> logList;
QProgressDialog *dlg;

int loadLogs() {
  int logsize = 0;
  logList.setAutoDelete(true);

  QString logdirname = kapp->localkdedir() + "/share/apps/kppp/Log/";
  QDir logdir(logdirname, "*.log");

  // get log file size
  const QFileInfoList *list = logdir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;

  while ((fi = it.current()) != 0) {
    logsize += fi->size();
    ++it;
  }

  dlg = new QProgressDialog(i18n("Loading log files"),
			     0,
			     logsize,
			     0);
  dlg->setProgress(0);

  // load logs
  list = logdir.entryInfoList();
  QFileInfoListIterator it1( *list );

  int retval = 0;
  while ((fi = it1.current()) != 0) {
    retval += loadLog(fi->absFilePath());
    ++it1;
  }

  delete dlg; 
  return retval;
}

int loadLog(QString fname) {
  FILE *f;
  int warning=0;

  f = fopen(fname.data(), "r");
  if(f == NULL)
    return 1;

  char buffer[2048+1];
  int lineno = 0;
  while(fgets(buffer, sizeof(buffer), f) != NULL) {
    ++lineno;
    buffer[sizeof(buffer) - 1] = 0;

    int slen = strlen(buffer);

    // skip blank lines
    if(slen < 10)
      continue;
    
    dlg->setProgress(dlg->progress() + slen);
    kapp->processEvents();

    LogInfo *li = new LogInfo(buffer);    
    if(li->error()) {

      // check if the connection has been terminated abnormally
      if(li->error() != 3) {
	warning++;    
	printf("ERROR IN FILE %s LINE %d \"%s\" (%d)\n", 
	       fname.data(), lineno, buffer, li->error());
	delete li;
      } else
	logList.append(li);
    } else
      logList.append(li);    
  }

  fclose(f);

  if(warning)
    return 2;
  else
    return 0;
}

int QLogList::compareItems(GCI a, GCI b) {
  LogInfo *la = (LogInfo *)a;
  LogInfo *lb = (LogInfo *)b;

  if(la->from() < lb->from())
    return -1;
  else if(la->from() > lb->from())
    return 1;
  else
    return 0;
}
