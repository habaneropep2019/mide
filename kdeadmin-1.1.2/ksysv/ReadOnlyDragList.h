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

#ifndef KSV_RO_DRAGLIST_H
#define KSV_RO_DRAGLIST_H

#include <qframe.h>
#include <qpopmenu.h>
#include <qscrbar.h>
#include <qstring.h>
#include "mylist.h"
#include "DragList.h"

/*
  ROWidgetList: a scrolled list of draggable items.
*/	

class ROWidgetList : public KSVDragList
{
  Q_OBJECT
 public:
  ROWidgetList( QWidget* parent = 0, char* name = 0);
  virtual ~ROWidgetList();

 public slots:
  void insertItem(char* _title, char* _file);
  void insertItem(KSVDragData* _w, int _pos, const bool genNr);
  void removeItem(KSVDragData* _w);
  void slotRemoveAfterDrop(QString _parent, KSVDragData* _w);

  /**
    * Insert a list of items
    * at the current cursor
    * position (forwarded to 
    * "canvas")
    */
  virtual void insertList( KSVList l );

  /**
    * Remove a list of items
    * (forwarded to canvas)
    */
  virtual void removeList( KSVList l );
            
 protected slots:
  void slotRootDrop(KDNDDropZone* _zone); 

};

#endif
