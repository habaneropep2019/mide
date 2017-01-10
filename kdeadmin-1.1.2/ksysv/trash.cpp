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

// KSVGui Trash Can

#include "trash.h"
#include <qtooltip.h>
#include <kapp.h>
#include <kiconloader.h>
#include <drag.h>

// include meta-object code
#include "trash.moc"


KSVTrash::KSVTrash( QWidget* parent, char* name) : QFrame(parent, name)
{
  setLineWidth(1);
  setMidLineWidth(0);

  if (style() == WindowsStyle)
    setFrameStyle( WinPanel | Sunken );
  else
    setFrameStyle( Panel | Sunken );

  kil = kapp->getIconLoader();
  kil->insertDirectory(3, kapp->kde_icondir());
  
  _pix = kil->loadIcon("kfm_fulltrash.xpm");
  _label = new QLabel(this);
  _label->setPixmap(_pix);
  _label->setGeometry(5, 7, 32, 32);
  
  QToolTip::add(_label, i18n("Drag here to remove services"));
  QToolTip::add(this, i18n("Drag here to remove services"));
  
  KDNDDropZone* _zone = new KDNDDropZone(this, DndText);
  // connect the dropZone with my dropAction  
  connect(_zone, SIGNAL(dropAction(KDNDDropZone* )), this, SLOT(dropHandler( KDNDDropZone* )));    
  
  setMinimumSize(42, 44);
}

KSVTrash::~KSVTrash() {
} 

void KSVTrash::dropHandler( KDNDDropZone* _dz ) {
  QString droppedData = _dz->getData();
  if ( _dz->getAcceptType() == _dz->getDataType() ) {
    KSVDragData* dragged = new KSVDragData();
    dragged->fromString(droppedData);
 
    emit sigDroppedFrom(dragged->runlevel(), dragged);
  } 
}
