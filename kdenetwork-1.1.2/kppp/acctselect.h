/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: acctselect.h,v 1.4 1998/08/25 21:13:40 mario Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
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

#ifndef __ACCTSELECT__H__
#define __ACCTSELECT__H__

#include <qdir.h>
#include <qwidget.h>
#include <qlabel.h>
#include <ktreelist.h>
#include <qchkbox.h>
#include <qpixmap.h>
#include <qlist.h>
#include <qcombo.h>

class AccountingSelector : public QWidget {
  Q_OBJECT
public:
  AccountingSelector(QWidget *parent = 0, bool _isnewaccount = false, const char *name = 0);
  ~AccountingSelector() {}

  bool save();

private:
  KTreeListItem *findByName(KTreeListItem *start, QString name);
  void setupTreeWidget();
  void insertDir(QDir, KTreeListItem * = 0);
  QString fileNameToName(QString);
  QString nameToFileName(QString);
  QString indexToFileName(int);

private slots:
  void enableItems(bool);
  void slotHighlighted(int);

private:
  KTreeList *tl;
  QCheckBox *use;
  QComboBox *use_vol;
  QPixmap pmfolder, pmfile;
  QLabel *selected;
  int choice;

  KTreeListItem *edit_item;
  QString edit_s;
  bool isnewaccount;
};

#endif

