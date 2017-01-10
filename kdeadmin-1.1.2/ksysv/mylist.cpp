/*

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

#include <qstring.h>
#include <mylist.h>

KSVList::~KSVList() {
// nothing todo either
}

int KSVList::compareItems(GCI item1, GCI item2) {
  KSVDragData* o1= (KSVDragData*)item1;
  KSVDragData* o2= (KSVDragData*)item2;
  int result = 0;

  if (o1->currentName().data() == o2->currentName())
    result = 0;
  else if (o1->number() > o2->number())
    result = 1;
  else if (o1->number() < o2->number())
    result = -1;
  else
    result = o1->currentName().data() < o2->currentName() ? -1 : 1;

  return result;
}

void KSVList::reSort() {
  KSVList tmp = *this;
  this->clear();

  const int max = tmp.count();
  
  for (int i = 0; i < max; ++i) {
    this->inSort(tmp.at(i));
  }
}
