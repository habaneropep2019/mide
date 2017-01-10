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

#ifndef MYLIST_H
#define MYLIST_H

#include <qlist.h>
#include <Data.h>

class KSVList : public QList<KSVDragData> {
public:
  ~KSVList();
protected:
  int compareItems(GCI item1, GCI item2);
  
public:
  void reSort();
};

#endif
