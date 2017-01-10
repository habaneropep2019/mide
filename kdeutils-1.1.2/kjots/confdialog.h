// -*- C++ -*-

//
//  kjots
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef confdialog_included
#define confdialog_included

#include <qwidget.h>

class KTabCtl;
class QPushButton;
class QLineEdit;
class QLabel;

class ConfDialog : public QWidget
{
  Q_OBJECT
public:
  ConfDialog ();
  ~ConfDialog () {}

signals:
  void commitChanges();
  void confHide();

protected slots:
  void ok();
  void apply();
  void cancel();
  void selectEdFont();

protected:
  void readConfig();
  void writeConfig();
  void closeEvent(QCloseEvent *e);
  
  KTabCtl     *tabbar;
  QPushButton *b_ok;
  QPushButton *b_apply;
  QPushButton *b_cancel;
  QLineEdit   *i_http;
  QLineEdit   *i_ftp;
  QLabel      *l_ed_font;
  QPushButton *b_sel_ed_font;

};



#endif //confdialog_included
