/*
    kiconedit - a small graphics drawing program for creating KDE icons
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef __KCOLORGRID_H__
#define __KCOLORGRID_H__

#include "utils.h"
#include <qtablevw.h>
#include <qdrawutl.h>
#include <qbrush.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qbuffer.h>
#include <qclipbrd.h>
#include <qscrbar.h>
#include <qfileinf.h>
#include <kfiledialog.h> 
#include <qcursor.h>
#include <qpntarry.h>
#include <kapp.h>
//#include <kmsgbox.h>

#if defined(USE_TEMPLATECLASS)
#define KArrayUint QArrayT<uint>
#else
Q_DECLARE(QArrayM,char);
#define KArrayUint QArrayM(uint)
#endif

class KColorArray : public KArrayUint
{
public:
  void remove(int idx);
  void append(uint);
  uint closestMatch(uint);
};

class KColorGrid : public QWidget
{
private:
    Q_OBJECT
public:
  KColorGrid( QWidget * parent = 0, const char *name = 0, int spacing = 0);
  virtual ~KColorGrid() {};

  enum GridState { Plain, Shaded };

  void setSpacing(int space) { s = space; update(); }
  void setGridState(GridState state) { gridstate = state;}
  GridState gridState() { return gridstate; }
  void setGrid(bool state) { hasgrid = state; }
  int spacing() { return s; }
  bool hasGrid() { return hasgrid; }
  uint numColors() { return numcolors.size(); }
  uint colors( uint *c ) { c = numcolors.data(); return numcolors.size(); }
  bool contains(uint c) { return numcolors.contains(c); }
  uint colorAt(int idx) { return gridcolors.at(idx); }
  uint *data() {return numcolors.data(); }
  void fill(uint color);
  int numCols() { return cols; }
  int numRows() { return rows; }

  virtual void setCellSize( int s );
  virtual int cellSize() { return cellsize; }
  virtual QSize sizeHint() const;

  void setSize(const QSize s) { setNumRows(s.height()); setNumCols(s.width()); }

public slots:
  //virtual void show();

signals:
  void addingcolor(uint);
  void colorschanged(uint, uint*);

protected:
  void updateCell(int row, int col, bool f);
  virtual void paintCell( QPainter *p, int row, int col) = 0;
  virtual void paintEvent(QPaintEvent *);
  //virtual void resizeEvent(QResizeEvent *);
  void setColor(int colNum, uint, bool update = true );
  void updateColors();
  virtual void setNumRows(int);
  virtual void setNumCols(int);
  virtual int totalWidth() { return totalwidth; }
  virtual int totalHeight() { return totalheight; }
  int findRow(int y) { return getY(y); }
  int getY(int y);
  int findCol(int x) { return getX(x); }
  int getX(int x);
  const QRect viewRect();

//private:
  int s, cellsize, totalwidth, totalheight;
  int rows, cols, ypos, xpos;
  GridState gridstate;
  bool hasgrid;
  KColorArray gridcolors, numcolors;
};



#endif //__KCOLORGRID_H__



