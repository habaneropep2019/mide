/*  Yo Emacs, this -*- C++ -*-

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef KSV_PREFS_H
#define KSV_PREFS_H

#include <qtabdlg.h>
#include <qpopmenu.h>
#include <qscrbar.h>
#include <qstring.h>
#include <qpushbt.h>
#include <qlined.h>
#include <kcolorbtn.h>

class KSVPrefDlg : public QTabDialog
{
  Q_OBJECT
public:
  KSVPrefDlg( QWidget* parent = 0, char* name = 0, WFlags f = 0);
  ~KSVPrefDlg();

protected:
  QLineEdit* _scriptPath;
  QLineEdit* _rlPath;
  QPushButton* browseScripts;
  QPushButton* browseRL;
  KColorButton* _newColor;
  KColorButton* _changedColor;

public:
  const QString getRLPath() const;
  const QString getScriptPath() const;
  const QColor getNewColor() const;
  const QColor getChangedColor() const;

public slots:
  void reReadConfig();
    
signals:

protected slots:
  void getScriptDir();
  void getRLDir();
};

#endif
