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

#include "debug.h"
#include "kcolorgrid.h"
#include "utils.h"

void KColorArray::remove(int idx)
{
  int oldsize = size();
  if(idx >= (int)size())
  {
    warning("KColorArray::remove: Index %d out of range", idx);
    return;
  }
  KColorArray tmp(*this);
  tmp.detach();
  resize(size()-1);
  for(int i = idx; i < oldsize-1; i++)
    at(i) = tmp[i+1];
  //debug("KColorArray::remove() %u\t-\tsize: %d", at(idx), size());
}

void KColorArray::append(uint c)
{
  resize(size()+1);
  at(size()-1) = c;
  //debug("KColorArray::append() %u\t-\tsize: %d", c, size());
}

uint KColorArray::closestMatch(uint color)
{
  //debug("KColorArray: %u", c);
  uint c = color & ~OPAQUE_MASK, d = 0xffffff, t;
  //debug("KColorArray: %u", c);
  //debug("KColorArray: %u", c|OPAQUE_MASK);
  uint cb = c;
  for(uint i = 0; i < size(); i++)
  {
    if (at(i) > cb)
      t = at(i) - cb;
    else
      t = cb - at(i);
    if( t < d )
    {
      d = t;
      c = at(i);
    }
  }
  return c|OPAQUE_MASK;
}

KColorGrid::KColorGrid(QWidget *parent, const char *name, int space)
 : QWidget(parent, name)
{
  //debug("KColorGrid - constructor");
  initMetaObject();
  s = space;
  rows = cols = totalwidth = totalheight = 0;
  setCellSize(10);
  setGridState(Plain);
  setGrid(true);
  numcolors.resize(0);
  gridcolors.resize(0);

  //debug("KColorGrid - constructor - done");
}
/*
void KColorGrid::show()
{
  //updateScrollBars();
  QWidget::show();
}
*/
void KColorGrid::paintEvent(QPaintEvent *e)
{
  //debug("KColorGrid::paintEvent");

  //updateScrollBars();
  //QWidget::paintEvent(e);

  const QRect urect = e->rect();

  //debug("Update rect = ( %i, %i, %i, %i )",
    //urect.left(),urect.top(), urect.width(), urect.height() );


  int firstcol = getX(urect.x())-1;
  int firstrow = getY(urect.y())-1;
  int lastcol  = getX(urect.right())+1;
  int lastrow  = getY(urect.bottom())+1;

  QWMatrix matrix;
  QPainter p;
  p.begin( this );

  firstrow = (firstrow < 0) ? 0 : firstrow;
  firstcol = (firstcol < 0) ? 0 : firstcol;
  lastrow = (lastrow >= rows) ? rows : lastrow;
  lastcol = (lastcol >= cols) ? cols : lastcol;
  //debug("%d x %d  -  row: %d x %d", urect.x(), urect.y(), urect.width(), urect.height());
  //debug("col: %d -> %d  -  row: %d -> %d", firstcol, lastcol, firstrow, lastrow);

/*
  if(this->isA("KDrawGrid"))
    debug("KDrawGrid\n   firstcol: %d\n   lastcol: %d\n   firstrow: %d\n   lastrow: %d",
        firstcol, lastcol, firstrow, lastrow);
*/
  for(int i = firstrow; i < lastrow; i++)
  {
    //if(this->isA("KDrawGrid"))
    //  debug("Updating row %d", i);
    for(int j = firstcol; j < lastcol; j++)
    {
      matrix.translate( (j*cellsize), (i*cellsize) );
      p.setWorldMatrix( matrix );
      //p.setClipRect(j*cellsize, i*cellsize, cellsize, cellsize);
      paintCell(&p, i, j);
      //p.setClipping(FALSE);
      matrix.reset();
      p.setWorldMatrix( matrix );
    }
    //kapp->processEvents();
  }

  p.end();

}

/*
void KColorGrid::resizeEvent(QResizeEvent *)
{
  //debug("resize: width:  %d - total: %d", width(), totalwidth);
  //debug("resize: height: %d - total: %d", height(), totalheight);
}
*/

QSize KColorGrid::sizeHint() const
{
  return QSize(totalwidth, totalheight);
}

int KColorGrid::getY( int y )
{
  if(y > (totalheight-1))
    y = totalheight;
  if(cellsize == 1)
    return y;
  return (y/cellsize);
}

int KColorGrid::getX( int x )
{
  if( x > totalwidth-1)
    x = totalwidth;
  if(cellsize == 1)
    return x;
  return (x/cellsize);
}

const QRect KColorGrid::viewRect()
{
  //debug("viewRect");
  const QRect r(0, 0, width(), height());
  //debug("viewRect - %d x %d - %d x %d", x, y, w, h);
  return r;
}

void KColorGrid::setNumRows(int n)
{
  //debug("setNumRows");
  if(n < 0 || n == rows)
    return;

  rows = n;

  gridcolors.resize(n*numCols());
  //QTableView::setNumRows(n);
  totalheight = (n * cellsize) + 1;
  resize(totalwidth, totalheight);
  //debug("setNumRows() - gridcolors: %d  size: %d", gridcolors.size(), numCols()*numRows());
}

void KColorGrid::setNumCols(int n)
{
  //debug("setNumCols");
  if(n < 0)
    return;
  int on = numCols();
  KColorArray gc(gridcolors);
  gc.detach();
  //debug("gc size: %d numrows: %d", gc.size(), numRows());
  gridcolors.resize(n*numRows());
  cols = n;

  totalwidth = (n * cellsize) + 1;
  resize(totalwidth, totalheight);
  //debug("numRows: %d", numRows());
  //debug("gridcolor: %d  grid: %d", gridcolors.size(), numRows()*numCols());
  if(numRows() == 0)
    return;

  for(int i = 0; i < numRows(); i++)
  {
    for(int j = 0; j < on; j++)
    {
      //debug("row %d , col %d", i, j);
      if(on <= n)
      {
        //debug("%d %d", (i*numCols())+j, (i*on)+j);
        gridcolors.at((i*numCols())+j) = gc.at((i*on)+j);
      }
      else
        break;
    }
  }

  //debug("setNumCols() - gridcolors: %d  size: %d", gridcolors.size(), numCols()*numRows());
}

void KColorGrid::fill( uint color)
{
  gridcolors.fill(color);
  numcolors.resize(1);
  numcolors.at(0) = color;
  emit colorschanged(numcolors.size(), numcolors.data());
}

void KColorGrid::setColor( int colNum, uint col, bool update )
{
  //debug("KColorGrid::setColor");
  uint oldcolor = gridcolors[colNum];
  gridcolors[colNum] = col;

  if(!update)
    return;

  //debug("KColorGrid::setColor - before adding");
  if(!numcolors.contains(col))
  {
    //debug("KColorGrid::setColor() - adding %u - %d %d %d",
    //  col, qRed(col), qGreen(col), qBlue(col));
    numcolors.append(col);
    //debug("KColorGrid::setColor() - adding done %d", numcolors.size()-1); 
    //numcolors++;
    emit addingcolor(col);
  }

  //debug("KColorGrid::setColor - before removing");
  if(!gridcolors.contains(oldcolor))
  {
    int idx = numcolors.find(oldcolor);
    if(idx != -1)
    {
      //debug("KColorGrid::setColor() - removing %u - %d %d %d",
      //  oldcolor, qRed(oldcolor), qGreen(oldcolor), qBlue(oldcolor));
      numcolors.remove(idx);
      //debug("KColorGrid::setColor() - removing done"); 
      emit colorschanged(numcolors.size(), numcolors.data());
    }
    //numcolors--;
  }

  //debug("KColorGrid::setColor - before updateCell");
  repaint((colNum%numCols())*cellsize,(colNum/numCols())*cellsize,  cellsize, cellsize);
  //updateCell( colNum/numCols(), colNum%numCols(), false );
  //debug("KColorGrid::setColor - after updateCell");
}

void KColorGrid::updateCell( int row, int col, bool erase )
{
    //debug("updateCell - before repaint");
  QWMatrix matrix;
  QPainter p;
  p.begin( this );
  matrix.translate( (col*cellsize), (row*cellsize) );
  p.setWorldMatrix( matrix );
  paintCell(&p, row, col);
  p.end();

}


void KColorGrid::updateColors()
{
  numcolors.resize(0);
  for(int i = 0; i < (int)gridcolors.size(); i++)
  {
    uint col = gridcolors.at(i);
    if(!numcolors.contains(col))
      numcolors.append(col);
  }
  emit colorschanged(numcolors.size(), numcolors.data());
}

void KColorGrid::setCellSize( int s )
{
  cellsize = s;
  totalwidth = (numCols() * s) + 1;
  totalheight = (numRows() * s) + 1;
  resize(totalwidth, totalheight);
  if ( isVisible() )
    repaint(viewRect(), false);
}


