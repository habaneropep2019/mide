/* -*- c++ -*- */
/***************************************************************************
 *                                 KArchie.h                               *
 *                            -------------------                          *
 *                         Header file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef KARCHIE_H
#define KARCHIE_H

#include <qlist.h>
#include <kapp.h>
#include <ktopwidget.h>
#include <kurl.h>
#include <kprocess.h>
//#include "KAMenu.h"
#include "KAQuery.h"
#include "KAQueryFile.h"

class KAMenu;
class KAStatusBar;
class KAView;
//class KConfig;
class KAQueryFile;


class TmpOpenFile : public QObject 
{
public:
  //TmpOpenFile(){};
TmpOpenFile(const QString &_tmpFileName, const KAQueryFile &_queryFile, KProcess &_process)
  : tmpFileName(&_tmpFileName), queryFile(&_queryFile), process(&_process) {};
~TmpOpenFile();

  const QString     *tmpFileName;
  const KAQueryFile *queryFile;
        KProcess    *process;

};

/*
class TmpOpenFileList : public QList<TmpOpenFile>
{
  //Q_OBJECT
  //public:
  //~TmpOpenFileList();

};
*/

class KArchie: public KTopLevelWidget
{
Q_OBJECT
public:
KArchie(const char *name);
~KArchie();


public slots:
  void slotChangeSettings();

  // slots for searching
  void slotSearchFile();
  void slotSearchPath();
  void slotSearchHost();

  void slotQuerySearch(char type);

  // slots for getting
  void slotOpenFileSelected(); // open the downloaded file in editor
  void slotOpenDirSelected();
  void slotGetfileSelected();
  void slotOpenFileSelected(const KAQueryFile *);
  void slotOpenDirSelected(const KAQueryFile *);
  void slotGetfileSelected(const KAQueryFile *);
  void slotLoadfilelistSelected();
  void slotStorefilelistSelected();
  void slotWritefilelistSelected();
  
  void slotSortListHostname();
  void slotSortListDomain();
  void slotSortListDate();
  void slotSortListFilesize();

protected:
  void timerEvent( QTimerEvent * );

private:
  void changeSettings();
  void getFile(const KURL &url);
  void openFile(const KURL &url);
  void openDir(const KURL &url);
  bool checkFtpFile(KURL &url, const KAQueryFile *file);
  bool checkFtpPath(KURL &url, const KAQueryFile *file);
  bool checkFtpHost(KURL &url, const KAQueryFile *file);
  bool checkUrl(const KURL &url); // emits a warning window, if wrong
  bool checkSaveFile(QString filename); // checks for overwriting an existing file. Displays a window, returns FALSE, if the file is not to be overwritten.
  const KAQueryFile *checkViewSelection(); // returns the selected file from view or error window, if not.

  KAMenu      *menu;
  KAStatusBar *statbar;
  KAView      *view;
  KAQuery     *query;

  KConfig         *config;
  KAQueryList     *queryResult;
  //  TmpOpenFileList *downloadTmpFileList;
  int              queryTimer;

private slots:
  void slotReturnQuery();
};


#endif
