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

#ifndef KSV_CLIPBOARD_H
#define KSV_CLIPBOARD_H

#include <qobject.h>
#include "Data.h"
#include "mylist.h"

class KSVClipBoard : public QObject
{
  Q_OBJECT

public:
  KSVClipBoard( QObject* parent = 0, const char* name = 0 );
  ~KSVClipBoard();

protected:
  KSVDragData* _current;

public:
  KSVDragData* getClipboard();

public slots:
  void clear();
  void setClipboard( KSVDragData* data );
  
signals:
  void filled( const KSVDragData* );

};

#endif
