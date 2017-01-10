/*  Yo Emacs, this -*-C++-*-

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

#ifndef KSV_TRASH_H
#define KSV_TRASH_H

#include <qframe.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qstring.h>
#include <drag.h>
#include <kiconloader.h>
#include "Data.h"

class KSVTrash : public QFrame
{
  Q_OBJECT
 public:
  KSVTrash( QWidget* parent = 0, char* name = 0);
  ~KSVTrash();

 protected:
  QLabel* _label;
  QPixmap _pix;
  KIconLoader* kil;
              
 public:

 public slots:
            
 signals:
  void sigDroppedFrom( QString _parent, KSVDragData* _w );
  
 protected slots:
  void dropHandler( KDNDDropZone* _dz );
};

#endif
