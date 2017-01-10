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

#ifndef SubjList_included
#define SubjList_included

#include <qlistbox.h>
#include <qlayout.h>
#include <qpushbt.h>

#include "KJotsMain.h"

class SubjList : public QWidget
{
  Q_OBJECT
public:
  SubjList( QWidget* parent = NULL, const char* name = NULL );
  virtual ~SubjList() {}

signals:
  void hidden();
  void entryMoved(int);

public slots:
  void rebuildList( QList<TextEntry> *);
  void entryChanged(const char *);
  void select( int );

protected slots:
  void hideMyself() { hide(); emit hidden(); }
  void highlighted( int index ) { current = index; emit entryMoved(index); }

protected:
  QListBox         *lb_subj;
  QGridLayout      *top2bottom;
  QPushButton      *b_ok;
  QList<TextEntry> *entrylist;
  int               current;
};

#endif // SubjList_included



