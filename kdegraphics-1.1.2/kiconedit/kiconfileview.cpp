// -*- c++ -*-
/*  
    KDE Icon Editor - a small graphics drawing program for the KDE

    Copyright (C) 1998 Thomas Tanghus (tanghus@kde.org)

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

#include <kdir.h>
#include <qpixmap.h>
//#include <qimage.h>
//#include <qimageio.h>
#include "kiconfileview.h"
#include "qkeycode.h"
#include <qpainter.h>
#include <kapp.h>
#include "debug.h"
//#include "xview.h"

#define PIX_HEIGHT 60
#define PIX_WIDTH 100


KIconFileView::KIconFileView(bool s, QDir::SortSpec sorting,
                                 QWidget *parent, const char *name)
    : QTableView(parent, name), KFileInfoContents(s,sorting)
{
  //debug("KIconFileView - constructor");
  width_max = 100;
  width_length = 0;
    
  QWidget::setFocusPolicy(QWidget::StrongFocus);
    
  setLineWidth( 2 );
  setFrameStyle( Panel | Sunken );

  rowsVisible = 1;

  setCellHeight( fontMetrics().lineSpacing() + PIX_HEIGHT + 5);

  setTableFlags(Tbl_autoHScrollBar | Tbl_cutCellsV | Tbl_smoothHScrolling);
  curCol = curRow = 0;
  // QTableView::setNumCols(0);
  QTableView::setNumRows(1);
    
  setBackgroundMode( PaletteBase );
  touched = false;
  pixmaps.setAutoDelete(true);
}

KIconFileView::~KIconFileView()
{
  debug("KIconFileView - destructor");
}

void KIconFileView::setAutoUpdate(bool f)
{
    QTableView::setAutoUpdate(f);
}

void KIconFileView::highlightItem(unsigned int i)
{
    debug("highlightItem %d", i);

    int col =  i / rowsVisible;
    highlightItem( i - rowsVisible * col, col );
}

void KIconFileView::highlightItem(int row, int col)
{ 
    debug("highlightItem %d %d", row, col);

    if (col * rowsVisible + row  >= static_cast<int>(count()))
	return;

    int cx;
    if (!colXPos ( col , &cx ))
	cx = 0;
    
    int edge = leftCell();              // find left edge
    if ( col < edge || cx < 0) {
	edge = leftCell() - curCol + col;
	if (edge < 0)
	    edge = 0;
	setLeftCell( edge );
    }

    edge = lastColVisible();
    if ( col >= edge )
	setLeftCell( leftCell() + col - edge + 1);

    edge = topCell();
    if ( row < edge )
	setTopCell( edge - 1 );

    edge = lastRowVisible();
    if ( row >= edge )
	setTopCell( topCell() + 1 );

    if (curCol != static_cast<int>(col) || 
	curRow != static_cast<int>(row)) 
    {
	int oldRow = curRow;
	int oldCol = curCol;
	curCol = col; curRow = row;
	updateCell( oldRow, oldCol);
	updateCell( row, col );
    }
}

void KIconFileView::clearView()
{
    setNumCols(1);
    pixmaps.clear();
    curCol = curRow = hasFocus() ? 0 : -1;
    width_length = 0;
}

int KIconFileView::cellWidth ( int col )
{
  int offset = col * rowsVisible;
  int width = 50;
  for (int j = 0; j < rowsVisible; j++) 
  {
    int w = fontMetrics().width(text(offset + j));
    if (width < w)
      width = w;
  }
  width += 10; 
  return width;
}


void KIconFileView::paintCell( QPainter *p, int row, int col)
{
    //debug("paintCell");
    uint index = col * rowsVisible + row;
    //debug("paintCell: %s", text(index));

    int w = cellWidth( col );                   // width of cell in pixels
    int h = PIX_HEIGHT + 5 + fontMetrics().height(); //cellHeight( row );                  // height of cell in pixels
    int x2 = w - 1;
    int y2 = h - 1;

    if ( (row == curRow) && (col == curCol) ) { // if we are on current cell,
        if ( hasFocus() ) {
              p->fillRect(0, 0, x2, y2, kapp->selectColor);
	      p->setPen( kapp->selectTextColor );
        }
        else { // no focus => draw only a dashed line around current item
             p->setPen( DotLine );               // used dashed line to
             p->drawRect( 0, 0, x2, y2 );        // draw rect along cell edges
             p->setPen( SolidLine );             // restore to normal
             p->setPen( kapp->windowTextColor );
        }
    } else // not on current cell, use the normal color
          p->setPen( kapp->windowTextColor );

    if (index < count()) {
	p->drawPixmap(w/2 - pixmaps.at(index)->width()/2, PIX_HEIGHT/2 - pixmaps.at(index)->height()/2, *pixmaps.at(index));
	p->drawText(w/2 - fontMetrics().width(text(index))/2, cellHeight() - fontMetrics().height()/2, text(index));
	//p->drawText(3 + pixmaps.at(index)->width(), 15, text(index));
    }
    //debug("paintCell - done");
}


void KIconFileView::keyPressEvent( QKeyEvent* e )
{
    int newRow = curRow;                        // store previous current cell
    int newCol = curCol;
    int oldRow = curRow;
    int oldCol = curCol;
    int lastItem = 0;
    int jump     = 0;

    switch( e->key() ) {                        // Look at the key code
    case Key_Left:
	if( newCol > 0 )
	    newCol--;
        else newRow = 0;
	break;
    case Key_Right:                         // Correspondingly...
	if( newCol < numCols()-1 )
	    newCol++;
        else newRow = lastRowVisible();
	if (newCol * rowsVisible + oldRow >= static_cast<int>(count()))
	    newRow = count() - rowsVisible * newCol - 1;
	break;
    case Key_Up:
	if( newRow >= 0 ) {
	    newRow--;
	    if (newRow == -1)
		if (newCol == 0)
		    newRow = 0;
		else {
		    newRow = rowsVisible - 1;
		    newCol--;
		}
	}
	break;
    case Key_Down:
	if( newRow <= numRows()-1 ) {
	    newRow++;
	    if (newRow >= numRows()) {
		newRow = 0;
 	 	newCol++;
	    }
	}
	break;
    case Key_Home:
        newRow = 0;
        newCol = 0; 
        break;
    case Key_End:
        newRow = count() % numRows() - 1; // calc last item in last col
        newCol = numCols() - 1;
        break;
    case Key_PageUp:
        jump = lastColVisible() - leftCell(); // num of cols we want to jump
        if ( curCol - jump < 0 ) {
          newRow = 0;
          newCol = 0;
        } else newCol = curCol - jump;
        break;
    case Key_PageDown:
	jump = lastColVisible() - leftCell();
        lastItem = count() % numRows() - 1;   // last item in last col

        if ( curCol + jump >= numCols() ) { // too far, just go to last col
          newCol = numCols() - 1;
          newRow = lastItem; 
        } else {
          newCol += jump; 
          if ( newCol == numCols()-1 && curRow > lastItem ) 
	      newRow = lastItem;
          else 
	      newRow = curRow;
        }
        break;
    case Key_Enter:
    case Key_Return:
	select( curCol * rowsVisible + curRow );
	return;
	break;
    default: 
	{
	    if ((e->ascii() >= 'a' && e->ascii() <= 'z') || 
		(e->ascii() >= 'A' && e->ascii() <= 'Z')) {
		char tmp[2] = " ";
		tmp[0] = e->ascii();
		QString res = findCompletion(tmp);
		if (!res.isNull())
		    debug("found %s",res.data());
	    } else
		e->ignore();
	}
	return;     
    }
   
    highlightItem( newRow, newCol );
    
    if ( curRow != oldRow || curCol != oldCol )
	highlight( curRow + curCol * rowsVisible );
   
}


bool KIconFileView::insertItem(const KFileInfo *i, int index)
{
//debug("insertItem: %s", i->fileName());
    QPixmap *pix;
    QString path;
    if (numCols() * rowsVisible < static_cast<int>(count()))
        setNumCols(numCols() + 1);
    
    if (i->isDir())
    {
      if (i->isReadable())
	pixmaps.insert(index, new QPixmap(*pix_folder));
      else
	pixmaps.insert(index, new QPixmap(*pix_lfolder));	
    }
    else
    {
      if (i->isReadable())
      {
        path = i->absURL();
        path += i->fileName();
        pix = new QPixmap;
        if(checkXVFile(i, path.data(), pix))
        {
 	  pixmaps.insert(index, pix);
        }
        else
        {
	  pixmaps.insert(index, new QPixmap(*pix_file));
	  delete pix;
	}
      }
      else
	pixmaps.insert(index, new QPixmap(*pix_lfile));
    }
    
    int curCol = index / rowsVisible;

    return colIsVisible(curCol) || curCol < leftCell() ;
}

void KIconFileView::resizeEvent ( QResizeEvent *e )
{
    QTableView::resizeEvent(e);
    rowsVisible = viewHeight() / QTableView::cellHeight();
    //if (!rowIsVisible(rowsVisible))
	//rowsVisible--;
    
    int cols;
    if (rowsVisible <= 0) 
	rowsVisible = 1;
    setNumRows(rowsVisible);
    cols = count() / rowsVisible + 1;
    setNumCols(cols);
    QTableView::repaint(true);
}

void KIconFileView::mousePressEvent( QMouseEvent* e )
{
    int oldRow = curRow;                        // store previous current cell
    int oldCol = curCol;
    QPoint clickedPos = e->pos();               // extract pointer position
    curRow = findRow( clickedPos.y() );         // map to row; set current cell
    
    if (curRow > static_cast<int>(rowsVisible))
	curRow = rowsVisible;
    
    curCol = findCol( clickedPos.x() );         // map to col; set current cell

    uint index = curCol * rowsVisible + curRow;

    if ( index  >= count()) {
	curCol = oldCol;
	curRow = oldRow;
	return;
    }
    
    if ( (curRow != oldRow)                     // if current cell has moved,
         || (curCol != oldCol) ) {
        updateCell( oldRow, oldCol );           // erase previous marking
        updateCell( curRow, curCol );           // show new current cell
    }
    
    if ( useSingle() && isDir(index)) 
	select( index );
    else
	highlight( index );
}

void KIconFileView::mouseDoubleClickEvent ( QMouseEvent *e )
{
    int oldRow = curRow;                        
    int oldCol = curCol;
    QPoint clickedPos = e->pos();               
    curRow = findRow( clickedPos.y() );         
    
    if (curRow > static_cast<int>(rowsVisible))
	curRow = rowsVisible;
    
    curCol = findCol( clickedPos.x() );         
    
    uint index = curCol * rowsVisible + curRow;

    if ( index >= count()) {
	curCol = oldCol;
	curRow = oldRow;
	return;
    }
    
    if ( (curRow != oldRow)                     
         || (curCol != oldCol) ) {
        updateCell( oldRow, oldCol );
        updateCell( curRow, curCol );
    }
    select( index );
}
    
void KIconFileView::focusInEvent ( QFocusEvent * )
{
    if (curRow < 0 || curCol < 0)
	curRow = curCol = 0;
    updateCell( curRow, curCol );
}  

void KIconFileView::focusOutEvent ( QFocusEvent * ) 
{
    updateCell( curRow, curCol );  
}


