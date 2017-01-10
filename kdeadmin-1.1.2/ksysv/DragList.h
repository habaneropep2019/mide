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

#ifndef KSV_DRAGLIST_H
#define KSV_DRAGLIST_H

#include <qframe.h>
#include <qpopmenu.h>
#include <qscrbar.h>
#include <qstring.h>
#include <qcolor.h>
#include "mylist.h"
#include "DragListCanvas.h"
#include "Data.h"
#include "moving.h"
#include "ViewPort.h"

/*
  KSVDragList: a scrolled list of draggable items.
*/	

class KSVDragList : public QFrame
{
  Q_OBJECT
public:
  KSVDragList( QWidget* parent = 0, char* name = 0);
  virtual ~KSVDragList();

protected:
  int maxVisible;
  int curIndex;
  KSVViewPort* viewPort;
  QScrollBar* bar;
  KSVDragListCanvas* canvas;
  KSVMovingPart* movingPart;
  int offset;
  KSVList deletedItems;

  /* new members: */
  void adjustSize();

  /* overidden members: */
  virtual void resizeEvent( QResizeEvent* _e );

public:
  virtual void clear();
  virtual const int count() const;
  const bool isOrigin() const;
  virtual KSVDragData* at( const int index );
  virtual int find( const KSVDragData* ) const;
  virtual QSize sizeHint() const;
  virtual KSVDragData* selected();
  virtual int focusItem() const;

  /**
   * Returns a list of all that items that were
   * removed from the widget
   */
  KSVList getDeletedItems();

public slots:
 /**
  * Just for initialization...
  */
  virtual void initItem(const char* _file, const char* _path,
			const char* _name, int _nr);

  virtual void insertItem(KSVDragData* _w, int _pos, const bool genNr = FALSE);

  virtual void removeItem(KSVDragData* _w);
  void slotRemoveAfterDrop(QString _parent, KSVDragData* _w);
  void setDefaultIcon(QString _name);
  void setOrigin( bool val = TRUE );
  void setNewColor( const QColor& color );
  void setChangedColor( const QColor& color );

  /**
   * re-sorts entries after sorting number was changed
   * from outside
   */
  void reSort();
  
  /**
   * Clears the "deleted items" list (with autodeletion enabled)
   */
  void clearDeletedItems();

  /**
    * add a popup menu for entries
    */
  virtual void addItemPMenu( QPopupMenu* menu = 0 );

  /**
    * add popup menu for the widget itself
    * (if the list is not "full")
    */
  virtual void addPMenu( QPopupMenu* menu = 0 );

  /**
   * redraw some items
   */
  void redraw( int start, int end );

signals:
  void sigDroppedFrom(QString _par, KSVDragData* _w);
  void sigChanged();
  void sigNotRemovable();
  void sigDoubleClick(KSVDragData* _data, QPoint _pos);
  void mouseMove( int _x, int _y );
  void sigOrigin( bool val );
  void cannotGenerateNumber();
  void selected( const KSVDragData* );

protected slots:
  virtual void slotRootDrop(KDNDDropZone* _zone);
  void slotValueChanged(int _newValue);
  void updateBar();
  void keyboardDown(int _focus);
  void keyboardUp(int _focus);
  void slotDroppedFrom(QString, KSVDragData*);
  void doubleClickOnEntry(KSVDragListCanvas* _list, KSVDragData* _data, QPoint _pos);
  void autoScroll( int _x, int _y );
  void slotMouseMove( int _x, int _y );
  void slotOrigin( bool val );
  void fwdChanged();
  void fwdCannotGenerateNumber();
  void fwdSelected( const KSVDragData* );

private:
  int oldSliderValue;
};

#endif
