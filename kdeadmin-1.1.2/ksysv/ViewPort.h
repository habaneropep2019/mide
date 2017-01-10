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

#ifndef KSV_VIEWPORT_H
#define KSV_VIEWPORT_H

#include <qwidget.h>
#include <qpopmenu.h>

/*
  KSVViewPort
*/

class KSVViewPort : public QWidget 
{
  Q_OBJECT
 
 public:
  KSVViewPort( QWidget* parent = 0 );
  ~KSVViewPort();

 public slots:
  void addPMenu( QPopupMenu* menu );

 signals:
  void select( int val );

 protected:
  QPopupMenu* popup;
  virtual void mousePressEvent( QMouseEvent* e );

};

#endif
