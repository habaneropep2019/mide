/* -*- c++ -*- */
/***************************************************************************
 *                                  KAMenu.h                               *
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

#ifndef KAMENU_H
#define KAMENU_H

#include "KASettingsTypes.h"

#include <kmenubar.h> 

#define FILE_OPEN_ID    0
#define FILE_OPENDIR_ID 1
#define FILE_GET_ID     2
#define FILE_LOAD_ID    4
#define FILE_STORE_ID   5
#define FILE_WRITE_ID   6


class KConfig;

class KAMenu: public KMenuBar
{
Q_OBJECT
public:
KAMenu( QWidget *parent, const char *name );
~KAMenu();

  //  enum settings{ hostname, searchtype, sorttype, nicelevel, all, save };

  inline void setFileOpenEnable( bool enable );
  inline void setFileOpenDirEnable( bool enable );
  inline void setFileGetEnable( bool enable );
  inline void setFileLoadListEnable( bool enable );
  inline void setFileStoreListEnable( bool enable );
  inline void setFileWriteListEnable( bool enable );
  inline void setQueryEnable( bool enable );
  inline void setSortEnable( bool enable );

public slots:
  void slotHostlistChanged(); // reads the hostlist, emits sigArchiehost()
  void slotConfigChanged(); // reads the configuration without emitting sigs.
  void slotQueryRunning(bool); // changes the menu when the query runs:Stop enabled, queries disabled
  inline void slotQueryBegin();
  inline void slotQueryEnd();

signals:
  void sigFileOpen();
  void sigFileOpenDir();
  void sigFileGet();
  void sigFileLoadList();
  void sigFileStoreList();
  void sigFileWriteList();
  void sigSettingsShowFileDiscription( bool );
  void sigArchieHost(const char *);
  void sigSearchmode(SearchMode::mode);
  void sigNicelevel(NiceLevel::mode);
  void sigSettingsAll();
//  void sigSettings( enum settings );
  //  void sigSettingsChanged();
  void sigQueryFile();
  void sigQueryPath();
  void sigQueryHost();
  void sigQueryStop();
  void sigSortHostname();
  void sigSortDomain();
  void sigSortDate();
  void sigSortFilesize();

private:
  static void changeCheckMenu(QPopupMenu *, int old_id, int new_id);
  static int searchmode2menu(SearchMode::mode);
  static SearchMode::mode menu2searchmode(int);
  static int nicelevel2menu(NiceLevel::mode);
  static NiceLevel::mode menu2nicelevel(int);

  QPopupMenu *file, *settings, *host, *searchmode, *nicelevel,
    *query, *sort, *help;
  int host_id, nice_id, searchmode_id;
  KConfig *config;

private slots:
  void slotFileOpen();
  void slotFileOpenDir();
  void slotFileGet();
  void slotFileSave();
  void slotFileLoad();
  void slotFileWrite();

  void slotSettings(int);
  void slotSettingsHostname(int);
  void slotSettingsSearchmode(int);
  //  void slotSettingsSorttype();
  void slotSettingsNicelevel(int);
  void slotSettingsShowFile();
  void slotSettingsAll();
  void slotSettingsSave();

  void slotQuery(int);

  void slotSort(int);

  //  void slotHelpUsingKA();
  //  void slotHelpAbout();

};

inline void
KAMenu::setFileOpenEnable( bool /*enable*/ )
    { file->setItemEnabled( file->idAt(FILE_OPEN_ID), /*enable*/FALSE); }

inline void 
KAMenu::setFileOpenDirEnable( bool enable )
    { file->setItemEnabled( file->idAt(FILE_OPENDIR_ID), enable); }

inline void 
KAMenu::setFileGetEnable( bool enable )
    { file->setItemEnabled( file->idAt(FILE_GET_ID), enable); }

inline void 
KAMenu::setFileLoadListEnable( bool enable )
    { file->setItemEnabled( file->idAt(FILE_LOAD_ID), enable); }

inline void 
KAMenu::setFileStoreListEnable( bool enable )
    { file->setItemEnabled( file->idAt(FILE_STORE_ID), enable); }

inline void 
KAMenu::setFileWriteListEnable( bool enable )
    { file->setItemEnabled( file->idAt(FILE_WRITE_ID), enable); }

inline void 
KAMenu::setQueryEnable( bool enable )
    { setItemEnabled( 2, enable ); }

inline void 
KAMenu::setSortEnable( bool enable )
    { setItemEnabled( 3, enable ); }

inline void 
KAMenu::slotQueryBegin()
{ slotQueryRunning(TRUE); }

inline void 
KAMenu::slotQueryEnd()
{ slotQueryRunning(FALSE); }

#endif
