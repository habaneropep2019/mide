// -*- C++ -*-

//
//  klpq
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.imv.de or chris@kde.org
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

#include "queueview.h"
#include "queueview.moc"


MyRow::MyRow(QString ra, QString ow, int jo, QString fi, QString si)
{
  rank.setStr(ra);
  owner.setStr(ow);
  job = jo;
  files.setStr(fi);
  size.setStr(si);
}

void MyRow::paint( QPainter *p, int col, int width )
{
  int fontpos = ( p->fontMetrics().lineSpacing() - p->fontMetrics().lineSpacing())/2;
  QString job_str;
  switch( col )
    {
    case 0: // rank
      {
	p->drawText( 3, fontpos, width, p->fontMetrics().lineSpacing(),
		     AlignLeft,
		     rank );
	break;
      }
    break;
    case 1: // owner
      p->drawText( 3, fontpos, width, p->fontMetrics().lineSpacing(),
		   AlignLeft,
		   owner );
      break;
    case 2: // job id
      job_str.setNum(job);
      p->drawText( 3, fontpos, width, p->fontMetrics().lineSpacing(),
		   AlignLeft,
		   job_str );
      break;
    case 3: // files
      p->drawText( 3, fontpos, width, p->fontMetrics().lineSpacing(),
		   AlignLeft,
		   files );
      break;
    case 4: // size
      p->drawText( 3, fontpos, width, p->fontMetrics().lineSpacing(),
		   AlignLeft,
		   size );
      break;
    }
}

//-------------------------------------------
//  myRowTable
//-------------------------------------------

MyRowTable::MyRowTable( QWidget *parent = NULL, const char *name = NULL )
  : KRowTable( KRowTable::SelectRow, parent, name )
{
  setCellHeight( fontMetrics().lineSpacing() );
  setNumCols( 5 );
  //current = -1;
}

void MyRowTable::insertItem( QString row )
{
  int i, x, y;
  QString rank, owner, job_str, files, size;
  int job;

  row     = row.simplifyWhiteSpace();
  i       = row.find(' ');
  if( i == -1 )
    return;
  rank    = row.left(i);
  row     = row.right(row.length() - i - 1);
  i       = row.find(' ');
  if( i == -1 )
    return;
  owner   = row.left(i);
  row     = row.right(row.length() - i - 1);
  i       = row.find(' ');
  if( i == -1 )
    return;
  job_str = row.left(i);
  job     = job_str.toInt();
  row     = row.right(row.length() - i - 1);
  y       = row.findRev(' ');
  if( y == -1 )
    return;
  x       = row.findRev(' ', y - 1);
  if( x == -1 )
    return;
  size    = row.mid(x + 1, y - x - 1);
  files   = row.left(x);

  MyRow *new_row = new MyRow(rank, owner, job, files, size);
  setNumRows( numRows() + 1 );
  insertRow( new_row, numRows()-1 );
  if( isUpdatesEnabled() )
    repaint();
}

void MyRowTable::clear()
{
  setTopCell( 0 );
  //current = -1;
  setNumRows( 0 );
  if( isUpdatesEnabled() )
    updateScrollBars();
}

void MyRowTable::setCurrentItem(int row)
{
  setCurrentRow( row, -1 );
}

int MyRowTable::jobId(int row)
{
  return ((MyRow *) getRow(row))->jobId();
}
