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

#include "ReadOnlyDragList.h"
#include <kapp.h>

// include meta-object code
#include "ReadOnlyDragList.moc"

#include "Constants.h"
#define VISIBILITY_OFFSET 0

ROWidgetList::ROWidgetList( QWidget* parent, char* name ) : KSVDragList(parent, name) {
  canvas->setReadOnly(TRUE);
}

ROWidgetList::~ROWidgetList() {
  
  // nothing yet
}


void ROWidgetList::insertItem(char*, char* ) {
  
}

void ROWidgetList::insertItem(KSVDragData*, int, const bool) {
  
}

void ROWidgetList::removeItem(KSVDragData* ) {
  
  emit sigNotRemovable();
}

void ROWidgetList::slotRootDrop( KDNDDropZone* ) {
  
}


void ROWidgetList::slotRemoveAfterDrop(QString _parent, KSVDragData* data ) {  
  if ( _parent == name() ) {
    emit sigNotRemovable();

    // delete the KSVDragData object since this is the expected behavior
    // otherwise we´d have a memory leak
    delete data;
  }
}


void ROWidgetList::insertList( KSVList ) {

}

void ROWidgetList::removeList( KSVList ) {

}
