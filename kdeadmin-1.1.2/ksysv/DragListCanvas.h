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

#ifndef KSV_DRAGLIST_CANVAS_H
#define KSV_DRAGLIST_CANVAS_H

#include <qcolor.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpopmenu.h>
#include <qtimer.h>
#include <qcolor.h>
#include "mylist.h"
#include "Data.h"
#include <kiconloader.h>
#include <drag.h>

class KSVDragList;

class KSVDragListCanvas : public KDNDWidget {
  Q_OBJECT

public:
  KSVDragListCanvas(QWidget* parent = 0, const char* name = 0);
  virtual ~KSVDragListCanvas();

protected:
  friend class KSVDragList;

  /* overridden */
  virtual void dndMouseMoveEvent ( QMouseEvent* _mouse );
  virtual void dndMouseReleaseEvent ( QMouseEvent* e );
  virtual void dragEndEvent();
  virtual void mousePressEvent ( QMouseEvent* e);
  virtual void mouseReleaseEvent ( QMouseEvent* e );
  virtual void paintEvent ( QPaintEvent* e );
  virtual void resizeEvent ( QResizeEvent* e );
  virtual void keyPressEvent ( QKeyEvent* e );
  virtual void keyReleaseEvent ( QKeyEvent* e );
  virtual void mouseDoubleClickEvent ( QMouseEvent* e );
  virtual void mouseMoveEvent ( QMouseEvent* e );
  virtual void focusInEvent ( QFocusEvent* e );
  virtual void focusOutEvent ( QFocusEvent* e );

  /* new */
  void updateTooltips();
  void adjustSize();
  void draw();
  void draw(const int index);
  void draw(int start, int end);
  void redrawFocus();
  KSVDragData* getObject(int _y, bool* _upper);
  KSVDragData* getObject(int _y);
  QPixmap getDefaultIcon();
  bool withinLimits( int x, int y ) const;

  int focus;
  bool noFocus;
  int oldFocus;
  bool pressed;
  bool ro;
  int press_x, press_y;
  QTimer* timer;
  QPoint _lastPos;
  KIconLoader* kil;
  KDNDDropZone* dz;
  QPixmap defaultIcon;
  QPixmap selIcon;
  QPixmap multiIcon;
  QBitmap monoIcon;
  QString runlevel;
  QPixmap buffer;
  KSVList list;	
  bool origin;
  QPopupMenu* popup;

 public slots:
  void slotDropEvent( KDNDDropZone* _zone );
  void setDefaultIcon( const char* _icon );
  void setRunlevel( QString _run);
  void insertItem( int _index, KSVDragData* _item, const bool genNr = FALSE );
  void removeItem( KSVDragData* _item );
  void setReadOnly(bool _value);
  void clear();
  void setOrigin(bool val = TRUE);
  void selectItem( int val );
  void setChangedColor( const QColor& color );
  void setNewColor( const QColor& color );

  /**
    * add popup menu for entries
    */
  virtual void addPMenu( QPopupMenu* menu );

protected slots:
  void autoScrollTimer();

signals:
  void sigDropOccured( KDNDDropZone* _zone, KSVDragListCanvas* _w, bool _pos);
  void sigOrigin( bool val );
  void sigDroppedFrom(QString _par, KSVDragData* _w);
  void sigChanged();
  void sigRightPressed(KSVDragListCanvas* _list, KSVDragData* _data, QPoint _pos);
  void sigResize(int _w, int _h);
  void sigScrollDown(int _focus);
  void sigScrollUp(int _focus);
  void sigNotRemovable();
  void sigDoubleClick(KSVDragListCanvas* _list, KSVDragData* _data, QPoint _pos);
  void mouseMove( int _x, int _y );
  void cannotGenerateNumber();

  /**
   * cut & copy only allowed if some entry is selected
   */
  void selected( const KSVDragData* );

public:
  virtual int count() const;
  virtual int find( const KSVDragData* o ) const;
  const QString getRunlevel() const;
  virtual KSVDragData* at(int _index);
  bool isOrigin() const;

  /**
   * May we insert the item here?
   */
  bool insertOK( KSVDragData* _data, const int where );

  int focusItem() const;

  /**
   * interpolate a sorting number using the numbers
   * above and below index.
   * Returns -1 if no number could be generated
   * (i.e. delta(nr(index-1),nr(index+1)) <= 1)
   */
  int generateNumber( const int index );

public slots:
  void reSort();
  
  
protected:
  QColor _newColor;
  QColor _changedColor;


private:
  QColor hiCol;
  QColor blackCol;
  QColor whiteCol;
  QColor hiText;
  QColor midCol;
  QColor paint_back;
  QColor paint_text;
  QFont italic;
};

#endif
