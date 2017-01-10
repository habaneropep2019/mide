// -*- C++ -*-

//
//  klpq
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

#ifndef queueview_included
#define queueview_included

#include "kheader.h"
#include "krowtable.h"

class MyRow : public KRow
{
public:
  MyRow(QString ra, QString ow, int jo, QString fi, QString si);
  virtual ~MyRow() {}

  int jobId() { return job; }
protected:
  void paint( QPainter *painter, int col, int width );

  QString rank;
  QString owner;
  int     job;
  QString files;
  QString size;
};

class MyRowTable : public KRowTable
{
  Q_OBJECT
public:
  MyRowTable( QWidget *parent = NULL, const char *name = NULL );
  ~MyRowTable() {}

  void insertItem( QString row );
  void clear();
  int  currentItem() { return currentRow(); }
  void setCurrentItem(int row);
  int  jobId(int row);
  int  count() { return numRows(); }

protected:
  int current;
};


#endif /* queueview_included */


