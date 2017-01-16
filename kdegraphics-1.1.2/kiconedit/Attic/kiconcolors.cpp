/*
    KDE Icon Editor - a small graphics drawing program for the KDE
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
#include "kiconcolors.h"

KDrawColors::KDrawColors(QWidget *parent) : KColorGrid(parent, 0, 3)
{
  debug("KDrawColors - constructor");
  initMetaObject();
  setCellSize(17);
  setGrid(true);
  setGridState(KColorGrid::Shaded);
  selected = 0;
  debug("KDrawColors - constructor - done");
}

void KDrawColors::paintCell( QPainter *painter, int row, int col )
{
  //KColorGrid::paintCell(painter, row, col);
  uint c = colorAt( row * numCols() + col );
  QBrush brush(c);
  int d = spacing();

  qDrawShadePanel( painter, d, d, cellSize()-d, cellSize()-d,
                colorGroup(), true, 1, &brush);
  if ( row * numCols() + col == selected)
     painter->drawWinFocusRect( d+1, d+1, cellSize()-(2*d)+1, cellSize()-(2*d)+1 );
}

void KDrawColors::mouseReleaseEvent( QMouseEvent *e )
{
  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  int cell = row * numCols() + col;

  if ( selected != cell )
  {
    int prevSel = selected;
    selected = cell;
    updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
    updateCell( row, col, FALSE );
  }

  emit newcolor(colorAt(cell)|OPAQUE_MASK);
}

KSysColors::KSysColors(QWidget *parent) : KDrawColors(parent)
{
  debug("KSysColors - constructor");
  initMetaObject();

  setNumRows(7);
  setNumCols(6);
  //fill(backgroundColor().rgb()|OPAQUE_MASK);
  setFixedSize(numCols()*cellSize(), numRows()*cellSize());

  int numcells = 42;

  debug("KSysColors - constructor - before setColor");
  for(int i = 0; i < numcells; i++)
    setColor(i, iconpalette[i]|OPAQUE_MASK);
  debug("KSysColors - constructor - done");
}

KCustomColors::KCustomColors(QWidget *parent) : KDrawColors(parent)
{
  debug("KCustomColors - constructor");
  initMetaObject();
  setNumRows(3);
  setNumCols(6);
  fill(kapp->backgroundColor.rgb()|OPAQUE_MASK);
  setFixedSize(numCols()*cellSize(), numRows()*cellSize());
  freecells = new bool[numRows()*numCols()];
  for(int i = 0; i < numRows()*numCols(); i++)
    freecells[i] = true;
  debug("KCustomColors - constructor - done");
}

KCustomColors::~KCustomColors()
{
  delete [] freecells;
}

void KCustomColors::mouseDoubleClickEvent(QMouseEvent *e)
{
  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  int cell = row * numCols() + col;

  QColor color;
  KColorDialog *cdlg = new KColorDialog;
  cdlg->setColor(colorAt(cell));
  if(cdlg->getColor(color))
  {
    setColor(cell, color.rgb());
    emit newcolor(color.rgb()|OPAQUE_MASK);
    freecells[cell] = false;
  }
  delete cdlg;
}

void KCustomColors::addColor(uint c)
{
  if(!contains(c))
  {
    int f = getFreeCell();
    if(f != -1)
    {
      QColor color(c);
      if(!color.isValid())
      {
        debug("KCustomColors::addColor: Not a valid color: %u", c);
        return;
      }
      //debug("KCustomColors::addColor: Adding color: %u - %d %d %d", c, qRed(c), qGreen(c), qBlue(c));
      setColor(f, c);
      freecells[f] = false;
    }
  }
}

int KCustomColors::getFreeCell()
{
  for (int i = 0; i < numRows()*numCols(); i++)
  {
    if(freecells[i])
    {
      if(i+1 < numRows()*numCols())
        freecells[i+1] = true;
      else
        freecells[0] = true;
      return i;
      break;
    } 
  }
  freeAllCells(); // start over again - not very elegant
  return 0;
}

void KCustomColors::freeAllCells()
{
  for (int i = 0; i < numRows()*numCols(); i++)
    freecells[i] = true;
}

void KCustomColors::clear()
{
  fill(TRANSPARENT);
  freeAllCells();
  update();
}


