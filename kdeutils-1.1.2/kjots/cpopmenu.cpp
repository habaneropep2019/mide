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


#include "cpopmenu.h"
#include "qapp.h"

/*****************************************************************************
  CPopupMenu member functions
 *****************************************************************************/

CPopupMenu::CPopupMenu( QWidget *parent, const char *name )
    : QPopupMenu( parent, name )
{
    initMetaObject();
    setMouseTracking(TRUE);
}

void CPopupMenu::mousePressEvent( QMouseEvent *e )
{
  if( !rect().contains( e->pos() ) )
    {
      return;
    }
  QPopupMenu::mousePressEvent( e);
  return;
}

void CPopupMenu::mouseReleaseEvent( QMouseEvent *e )
{
  if( QCursor::pos() == old_pos )
    return;
  QPopupMenu::mouseReleaseEvent( e );
}

void CPopupMenu::popup( const QPoint& pos, int indexAtPoint)
{
  old_pos = pos;
  QPopupMenu::popup( pos, indexAtPoint );
}

void CPopupMenu::hide()
{
  releaseMouse();
  QPopupMenu::hide();
}

void CPopupMenu::setActItem( int it)
{
  actItem = it;
}

