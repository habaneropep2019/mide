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
//  $Id: modemdb.h,v 1.1 1999/01/13 19:24:58 mario Exp $
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

#ifndef __MODEMDB__H__
#define __MODEMDB__H__

#include <qdialog.h>
#include <qlistbox.h>
#include <qstrlist.h>
#include <kconfig.h>
#include <qdict.h>
#include <qlist.h>

typedef QDict<char> CharDict;

class ModemDatabase {
public:
  ModemDatabase();
  ~ModemDatabase();

  const QStrList *vendors();
  QStrList *models(QString vendor);

  void save(KConfig *);

private:
  void load();
  void loadModem(const char *key, CharDict &dict);
  QList<CharDict> modems;

  QStrList *lvendors;

  KConfig *modemDB;
};


class ModemSelector : public QDialog {
  Q_OBJECT
public:
  ModemSelector(QWidget *parent = 0);  
  ~ModemSelector();

private slots:
  void vendorSelected(int idx);
  void modelSelected(int idx);
  void selected(int idx);

private:
  QPushButton *ok, *cancel;
  QListBox *vendor, *model;
  ModemDatabase *db;
};

#endif
