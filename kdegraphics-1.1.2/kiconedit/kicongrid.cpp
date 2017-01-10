/*
    KDE Icon Editor - a small graphics drawing program for the KDE.
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

#include <kpixmap.h>
#include <kruler.h>
#include "debug.h"
#include "kicongrid.h"
#include "main.h"
#include "pics/logo.xpm"

KGridView::KGridView(QImage *image, QWidget *parent, const char *name) : QFrame(parent, name)
{
  pprops = 0L;
  _corner = 0L;
  _hruler = _vruler = 0L;
  _grid = 0L;

  acceptdrop = false;
  
  pprops = props(this);

  viewport = new QScrollView(this);
  CHECK_PTR(viewport);

  _grid = new KIconEditGrid(image, viewport->viewport());
  CHECK_PTR(_grid);
  viewport->addChild(_grid);

  if(pprops->backgroundmode == FixedPixmap)
  {
    QPixmap pix(pprops->backgroundpixmap.data());
    if(pix.isNull())
    {
      QPixmap pmlogo((const char**)logo);
      pix = pmlogo;
    }
    viewport->viewport()->setBackgroundPixmap(pix);
    _grid->setBackgroundPixmap(pix);
  }
  else
  {
    viewport->viewport()->setBackgroundColor(pprops->backgroundcolor);
  }

  _corner = new QFrame(this);
  _corner->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  _hruler = new KRuler(KRuler::horizontal, this);
  _hruler->setEndLabel(i18n("width"));
  _hruler->setOffset( 0 );
  _hruler->setRange(0, 1000);

  _vruler = new KRuler(KRuler::vertical, this);
  _vruler->setEndLabel(i18n("height"));
  _vruler->setOffset( 0 );
  _vruler->setRange(0, 1000);

  connect(_grid, SIGNAL(scalingchanged(int, bool)), SLOT(scalingChange(int, bool)));
  connect(_grid, SIGNAL(sizechanged(int, int)), SLOT(sizeChange(int, int)));
  connect(_grid, SIGNAL(needPainting()), SLOT(paintGrid()));
  connect( _grid, SIGNAL(xposchanged(int)), _hruler, SLOT(slotNewValue(int)) );
  connect( _grid, SIGNAL(yposchanged(int)), _vruler, SLOT(slotNewValue(int)) );
  connect(viewport, SIGNAL(contentsMoving(int, int)), SLOT(moving(int, int)));

  setSizes();
  QResizeEvent e(size(), size());
  resizeEvent(&e);
}

void KGridView::paintGrid()
{
  _grid->repaint(viewRect(), false);
}

void KGridView::setSizes()
{
  if(pprops->showrulers)
  {
    _hruler->setLittleMarkDistance(_grid->scaling());
    _vruler->setLittleMarkDistance(_grid->scaling());

    _hruler->setMediumMarkDistance(5);
    _vruler->setMediumMarkDistance(5);

    _hruler->setBigMarkDistance(10);
    _vruler->setBigMarkDistance(10);

    _hruler->showTinyMarks(true);
    _hruler->showLittleMarks(false);
    _hruler->showMediumMarks(true);
    _hruler->showBigMarks(true);
    _hruler->showEndMarks(true);

    _vruler->showTinyMarks(true);
    _vruler->showLittleMarks(false);
    _vruler->showMediumMarks(true);
    _vruler->showBigMarks(true);
    _vruler->showEndMarks(true);

    _hruler->setValuePerLittleMark(_grid->scaling());
    _vruler->setValuePerLittleMark(_grid->scaling());
    _hruler->setValuePerMediumMark(_grid->scaling()*5);
    _vruler->setValuePerMediumMark(_grid->scaling()*5);

    _hruler->setPixelPerMark(_grid->scaling());
    _vruler->setPixelPerMark(_grid->scaling());

    _hruler->setMaxValue(_grid->width());
    _vruler->setMaxValue(_grid->height());

    _hruler->show();
    _vruler->show();

    _corner->show();
    //resize(_grid->width()+_vruler->width(), _grid->height()+_hruler->height());
  }
  else
  {
    _hruler->hide();
    _vruler->hide();
    _corner->hide();
    //resize(_grid->size());
  }
}

void KGridView::sizeChange(int, int)
{
  setSizes();
}

void KGridView::moving(int x, int y)
{
  //debug("Moving: %i x %i", x, y);
  _hruler->setOffset(abs(x));
  _vruler->setOffset(abs(y));
}

void KGridView::scalingChange(int, bool)
{
  setSizes();
}

void KGridView::setShowRulers(bool mode)
{
  pprops->showrulers = mode;
  setSizes();
  QResizeEvent e(size(), size());
  resizeEvent(&e);
}

void KGridView::setAcceptDrop(bool a)
{
  if(a == acceptdrop)
    return;
  acceptdrop = a;
  paintDropSite();
}

const QRect KGridView::viewRect()
{
  int x, y, cx, cy;
  if(viewport->horizontalScrollBar()->isVisible())
  {
    x = viewport->contentsX();
    cx = viewport->viewport()->width();
  }
  else
  {
    x = 0;
    cx = viewport->contentsWidth();
  }

  if(viewport->verticalScrollBar()->isVisible())
  {
    y = viewport->contentsY();
    cy = viewport->viewport()->height();
  }
  else
  {
    y = 0;
    cy = viewport->contentsHeight();
  }

  return QRect(x, y, cx, cy);
}

void KGridView::paintDropSite()
{
  QPainter p;
  p.begin( _grid );
  p.setRasterOp (NotROP);
  p.drawRect(viewRect());
  p.end();
}

void KGridView::paintEvent(QPaintEvent *e)
{
  //if(acceptdrop)
    //paintDropSite();
}


void KGridView::resizeEvent(QResizeEvent*)
{
  debug("KGridView::resizeEvent");
  setSizes();
  if(pprops->showrulers)
  {
    _hruler->setGeometry(_vruler->width(), 0, width(), _hruler->height());
    _vruler->setGeometry(0, _hruler->height(), _vruler->width(), height());

    _corner->setGeometry(0, 0, _vruler->width(), _hruler->height());
    viewport->setGeometry(_corner->width(), _corner->height(),
                   width()-_corner->width(), height()-_corner->height());
  }
  else
    viewport->setGeometry(0, 0, width(), height());
}

const QImage *fixTransparence(QImage *image)
{
  *image = image->convertDepth(32);
  image->setAlphaBuffer(true);
  for(int y = 0; y < image->height(); y++)
  {
    uint *l = (uint*)image->scanLine(y);
    for(int x = 0; x < image->width(); x++, l++)
    {
      if(*l < OPAQUE_MASK || *l == (TRANSPARENT|OPAQUE_MASK))
      {
        *l = TRANSPARENT;
      }
      //else debug("Opaque!");
    }
  }
  return image;
}

KIconEditGrid::KIconEditGrid(QImage *image, QWidget *parent, const char *name)
 : KColorGrid(parent, name, 1)
{
  initMetaObject();
  img = image;
  selected = 0;

  //setBackgroundMode(NoBackground);
  //kdeColor(1);

  for(uint i = 0; i < 42; i++)
    iconcolors.append(iconpalette[i]);

  setupImageHandlers();
  debug("Formats: %d", formats->count());
  btndown = isselecting = ispasting = modified = false;
  //modified = false;
  img->create(32, 32, 32);
  img->fill(TRANSPARENT);
  //numcolors = 0;
  currentcolor = qRgb(0,0,0)|OPAQUE_MASK;

  setMouseTracking(true);

  setNumRows(32);
  setNumCols(32);
  fill(TRANSPARENT);

  connect( kapp->clipboard(), SIGNAL(dataChanged()), SLOT(checkClipboard()));

  // this is kind of a hack but the application global clipboard object
  // doesn't always send the signals.
  QTimer *cbtimer = new QTimer(this);
  connect( cbtimer, SIGNAL(timeout()), SLOT(checkClipboard()));
  cbtimer->start(2500);

  createCursors();
}

KIconEditGrid::~KIconEditGrid()
{
  debug("KIconEditGrid - destructor: done");
}

void KIconEditGrid::paintEvent(QPaintEvent *e)
{
//debug("KIconGrid::paintEvent");
  KColorGrid::paintEvent(e);

  if(hasGrid())
  {
    int x = e->rect().x() - ((e->rect().x() % cellSize()) + cellSize());
    if(x < 0) x = 0;
    int y = e->rect().y() - ((e->rect().y() % cellSize()) + cellSize());
    if(y < 0) y = 0;
    int cx = e->rect().right() + cellSize();
    int cy = e->rect().bottom() + cellSize();

    QPainter p;
    p.begin( this );

    for(int i = x; i < cx; i += cellSize())
      p.drawLine(i, y, i, cy);

    for(int i = y; i < cy; i += cellSize())
      p.drawLine(x, i, cx, i);

    p.end();
  }
//debug("KIconGrid::paintEvent - done");
}

void KIconEditGrid::paintCell( QPainter *painter, int row, int col )
{
  int sel = row * numCols() + col;
  int s = cellSize();
  uint c = colorAt(sel);

  if(s == 1)
  {
    if((ispasting || isselecting) && isMarked(col, row))
    {
      painter->drawPoint(0, 0);
      return;
    }
    else
    {
      painter->setPen(c);
      painter->drawPoint(0, 0);
    }
  }
  else
  {
    painter->fillRect(0, 0, s, s, (QBrush)c); //brush);
    if((ispasting || isselecting) && isMarked(col, row))
    {
      painter->drawWinFocusRect( 1, 1, s-1, s-1);
      return;
    }
  }

  switch( tool )
  {
    case Find:
    case Spray:
    case Eraser:
    case Freehand:
      if ( sel == selected)
      {
        if(cellSize() > 1)
          painter->drawWinFocusRect( 1, 1, s-1, s-1 );
        else
          painter->drawPoint(0, 0);
      }
      break;
    //case Select:
    case FilledRect:
    case Rect:
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    case Line:
      if(btndown && isMarked(col, row))
      {
        if(cellSize() > 1)
          painter->drawWinFocusRect( 1, 1, s-1, s-1);
        else
          painter->drawPoint(0, 0);
      }
      break;
    default:
      break;
  }

  //debug("KIconEditGrid::paintCell() - done");
}

void KIconEditGrid::mousePressEvent( QMouseEvent *e )
{
  if(!e || (e->button() != LeftButton))
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  //int cell = row * numCols() + col;

  if(!img->valid(col, row))
    return;

  btndown = true;
  start.setX(col);
  start.setY(row);

  if(ispasting)
  {
    ispasting = false;
    editPaste(true);
  }

  if(isselecting)
  {
    QPointArray a(pntarray.copy());
    pntarray.resize(0);
    drawPointArray(a, Mark);
    emit selecteddata(false);
  }

  switch( tool )
  {
    case SelectRect:
    case SelectCircle:
      isselecting = true;
      break;
    default:
      break;
  }

}

void KIconEditGrid::mouseMoveEvent( QMouseEvent *e )
{
  if(!e)
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  int cell = row * numCols() + col;

  QPoint tmpp(col, row);
  if(tmpp == end)
    return;

  if(img->valid(col, row))
  {
    //debug("%d X %d", col, row);
    emit poschanged(col, row);
    emit xposchanged((col*scaling())+scaling()/2); // for the rulers
    emit yposchanged((row*scaling())+scaling()/2);
  }

  if(ispasting && !btndown && img->valid(col, row))
  {
    if( (col + cbsize.width()) > (numCols()-1) )
      insrect.setX(numCols()-insrect.width());
    else
      insrect.setX(col);
    if( (row + cbsize.height()) > (numRows()-1) )
      insrect.setY(numRows()-insrect.height());
    else
      insrect.setY(row);
    insrect.setSize(cbsize);
    //debug("Moving: %d x %d", insrect.width(), insrect.height());
    start = insrect.topLeft();
    end = insrect.bottomRight();
    drawRect(false);
    return;
  }

  if(!img->valid(col, row) || !btndown)
    return;

  end.setX(col);
  end.setY(row);

  if(isselecting)
  {
    if(tool == SelectRect)
      drawRect(false);
    else
      drawEllipse(false);
    return;
  }

  switch( tool )
  {
    case Eraser:
      currentcolor = TRANSPARENT;
    case Freehand:
    {
      setColor( cell, currentcolor );
  //img.setPixel(col, row, currentcolor.pixel());

      if ( selected != cell )
      {
        modified = true;
        int prevSel = selected;
        selected = cell;
        repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
        repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
        *((uint*)img->scanLine(row) + col) = (colorAt(cell));
      }
      break;
    }
    case Find:
    {
      iconcolors.closestMatch(colorAt(cell));
      if ( selected != cell )
      {
        int prevSel = selected;
        selected = cell;
        repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
        repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
      }
      break;
    }
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    {
      drawEllipse(false);
      break;
    }
    //case Select:
    case FilledRect:
    case Rect:
    {
      drawRect(false);
      break;
    }
    case Line:
    {
      drawLine(false);
      break;
    }
    case Spray:
    {
      drawSpray(QPoint(col, row));
      modified = true;
      break;
    }
    default:
      break;
  }

  p = *img;

  emit changed(QPixmap(p));
  //emit colorschanged(numColors(), data());
}

void KIconEditGrid::mouseReleaseEvent( QMouseEvent *e )
{
  if(!e || (e->button() != LeftButton))
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  btndown = false;
  end.setX(col);
  end.setY(row);
  int cell = row * numCols() + col;

  switch( tool )
  {
    case Eraser:
      currentcolor = TRANSPARENT;
    case Freehand:
    {
      if(!img->valid(col, row))
        return;
      setColor( cell, currentcolor );
      //if ( selected != cell )
      //{
        //modified = true;
        int prevSel = selected;
        selected = cell;
        repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
        repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
        *((uint*)img->scanLine(row) + col) = colorAt(cell);
        p = *img;
      //}
      break;
    }
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    {
      drawEllipse(true);
      break;
    }
    case FilledRect:
    case Rect:
    {
      drawRect(true);
      break;
    }
    case Line:
    {
      drawLine(true);
      break;
    }
    case Spray:
    {
      drawSpray(QPoint(col, row));
      break;
    }
    case FloodFill:
    {
      QApplication::setOverrideCursor(waitCursor);
      drawFlood(col, row, colorAt(cell));
      QApplication::restoreOverrideCursor();
      updateColors();
      emit needPainting();
      p = *img;
      break;
    }
    case Find:
    {
      currentcolor = colorAt(cell);
      if ( selected != cell )
      {
        int prevSel = selected;
        selected = cell;
        repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
        repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
      }

      break;
    }
    default:
      break;
  }
  emit changed(QPixmap(p));
  //emit colorschanged(numColors(), data());
}

//void KIconEditGrid::setColorSelection( const QColor &color )
void KIconEditGrid::setColorSelection( uint c )
{
  currentcolor = c;
}

void KIconEditGrid::loadBlank( int w, int h )
{
  img->create(w, h, 32);
  img->fill(TRANSPARENT);
  setNumRows(h);
  setNumCols(w);
  fill(TRANSPARENT);
  emit sizechanged(numCols(), numRows());
  emit colorschanged(numColors(), data());
}

void KIconEditGrid::load( QImage *image)
{
  debug("KIconEditGrid::load");

  setUpdatesEnabled(false);

  if(image != 0L)
  {
    *img = *fixTransparence(image);
    //*img = image->convertDepth(32);
    //img->setAlphaBuffer(true);
  }
  else
  {
    QString msg = i18n("There was an error loading a blank image.\n");
    KMsgBox::message (this, i18n("Error"), msg.data());
    return;
  }

  setNumRows(img->height());
  setNumCols(img->width());

  for(int y = 0; y < numRows(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < numCols(); x++, l++)
    {
/*
      //uint gray = (qRgb(200, 200, 200) | OPAQUE_MASK);
      //uint bc = (TRANSPARENT | OPAQUE_MASK);
      //if(*l == gray || *l == bc || *l < 0xff000000)  // this is a hack but I couldn't save it as transparent otherwise
      if(*l < 0xff000000 || *l == (TRANSPARENT|OPAQUE_MASK))  // this is a hack but I couldn't save it as transparent otherwise
      {
        *l = TRANSPARENT;
      }
      //debug("KIcnGrid::load: %d %%", (((y*ncols)+x)/imgsize) * 100);
      //debug("KIconEditGrid::load: RGB: %d %d %d", qRed(*l), qGreen(*l), qBlue(*l));
*/
      setColor((y*numCols())+x, *l, false);
    }
    //debug("Row: %d", y);
    kapp->processEvents(200);
  }
  updateColors();
  emit sizechanged(numCols(), numRows());
  emit colorschanged(numColors(), data());
  emit changed(pixmap());
  setUpdatesEnabled(true);
  emit needPainting();
  //repaint(viewRect(), false);
}

const QPixmap &KIconEditGrid::pixmap()
{
  if(!img->isNull())
    p = *img;
  //p.convertFromImage(*img, 0);
  return(p);
}

void KIconEditGrid::getImage(QImage *image)
{
  debug("KIconEditGrid::getImage");
  *image = *img;
}

bool KIconEditGrid::zoomTo(int scale)
{
  QApplication::setOverrideCursor(waitCursor);
  emit scalingchanged(cellSize(), false);
  setUpdatesEnabled(false);
  setCellSize( scale );
  setUpdatesEnabled(true);
  emit needPainting();
  QApplication::restoreOverrideCursor();
  emit scalingchanged(cellSize(), true);
  if(scale == 1)
    return false;
  return true;
}

bool KIconEditGrid::zoom(Direction d)
{
  int f = (d == In) ? (cellSize()+1) : (cellSize()-1);
  QApplication::setOverrideCursor(waitCursor);
  emit scalingchanged(cellSize(), false);
  setUpdatesEnabled(false);
  setCellSize( f );
  setUpdatesEnabled(true);
  //emit needPainting();
  QApplication::restoreOverrideCursor();

  emit scalingchanged(cellSize(), true);
  if(d == Out && cellSize() <= 1)
    return false;
  return true;
}

void KIconEditGrid::checkClipboard()
{
  bool ok = false;
  const QImage *tmp = clipboardImage(ok);
  if(tmp)
    delete tmp;
  if(ok)
    emit clipboarddata(true);
  else
    emit clipboarddata(false);
}

// PIXMAP format still isn't supported. It shouldn't be that hard - maybe I should have a look?
/*  
const QImage *KIconEditGrid::clipboardImage(bool &ok)
{
  QPixmap pix = *kapp->clipboard()->pixmap();
  QImage *image = new QImage;
  if(pix.isNull())
    debug("Invalid pixmap in clipboard!");
  else
  {
    debug("Valid pixmap in clipboard!");
    *image = pix;
  }
  return image;
}
*/

// the caller *must* delete the image afterwards
const QImage *KIconEditGrid::clipboardImage(bool &ok)
{
  ok = false;
  QString str = kapp->clipboard()->text();
  QBuffer buf(str);
  buf.open(IO_ReadOnly);
  QImageIO iio;
  iio.setIODevice(&buf);
  iio.setFormat("XPM");
  
  QImage *image = 0L;
  if(iio.read() && !iio.image().isNull())
  {
    image = new QImage(iio.image().convertDepth(32));
    image->setAlphaBuffer(true);
    ok = true;
  }

  buf.close();

  return image;
}


void KIconEditGrid::editSelectAll()
{
  start.setX(0);
  start.setY(0);
  end.setX(numCols());
  end.setY(numRows());
  isselecting = true;
  drawRect(false);
  emit newmessage(i18n("All selected"));
}

void KIconEditGrid::editClear()
{
  img->fill(TRANSPARENT);
  fill(TRANSPARENT);
  update();
  modified = true;
  p = *img;
  emit changed(p);
  emit newmessage(i18n("Cleared"));
}

// the returned image *must* be deleted by the caller.

QImage *KIconEditGrid::getSelection(bool cut)
{
  const QRect rect = pntarray.boundingRect();
  int nx = 0, ny = 0, nw = 0, nh = 0;
  rect.rect(&nx, &ny, &nw, &nh);

  QImage *tmp = new QImage(nw, nh, 32);
  tmp->fill(TRANSPARENT);

  int s = pntarray.size(); //((rect.size().width()) * (rect.size().height()));
  for(int i = 0; i < s; i++)
  {
    int x = pntarray[i].x();
    int y = pntarray[i].y();
    if(img->valid(x, y) && rect.contains(QPoint(x, y)))
    {
      *((uint*)tmp->scanLine(y-ny) + (x-nx)) = *((uint*)img->scanLine(y) + x);
      if(cut)
      {
        *((uint*)img->scanLine(y) + x) = TRANSPARENT;
        setColor( (y*numCols()) + x, TRANSPARENT, false );
      }
    }
  }

  QPointArray a(pntarray.copy());
  pntarray.resize(0);
  drawPointArray(a, Mark);
  emit selecteddata(false);
  if(cut)
  {
    updateColors();
    repaint(rect.x()*cellSize(), rect.y()*cellSize(),
            rect.width()*cellSize(), rect.height()*cellSize(), false);
    p = *img;
    emit changed(p);
    emit colorschanged(numColors(), data());
    emit newmessage(i18n("Selected area cutted"));
  }
  else
    emit newmessage(i18n("Selected area copied"));

  return tmp;
}

// the returned image *must* be deleted by the caller.
/*
QImage *KIconEditGrid::getSelection(bool cut)
{
  const QRect rect = pntarray.boundingRect();

  int nx = 0, ny = 0, nw = 0, nh = 0;
  rect.rect(&nx, &ny, &nw, &nh);
  debug("KIconEditGrid: Selection Rect: %d %d %d %d", nx, ny, nw, nh);

  QImage *tmp = new QImage(nw, nh, 32);

  QApplication::setOverrideCursor(waitCursor);
  for(int y = 0; y < nh; y++)
  {
    uint *l = ((uint*)img->scanLine(y+ny)+nx);
    uint *cl = (uint*)tmp->scanLine(y);
    for(int x = 0; x < nw; x++, l++, cl++)
    {
      *cl = *l;
      if(cut)
      {
        *l = TRANSPARENT;
        setColor( ((y+ny)*numCols()) + (x+nx), TRANSPARENT, false );
      }
    }
    kapp->processEvents();
  }

  QApplication::restoreOverrideCursor();

  QPointArray a(pntarray.copy());
  pntarray.resize(0);
  drawPointArray(a, Mark);
  emit selecteddata(false);
  if(cut)
  {
    updateColors();
    repaint(rect.x()*cellSize(), rect.y()*cellSize(),
            rect.width()*cellSize(), rect.height()*cellSize(), false);
    p = *img;
    emit changed(p);
    emit colorschanged(numColors(), data());
    emit newmessage(i18n("Selected area cutted"));
  }
  else
    emit newmessage(i18n("Selected area copied"));
  return tmp;
}
*/

void KIconEditGrid::editCopy(bool cut)
{
  QImage *sel = getSelection(cut);

  sel->setAlphaBuffer(true);
  QString str = "";
  QBuffer buf(str);
  buf.open(IO_WriteOnly);
  QImageIO iio;
  iio.setImage(*sel);
  iio.setIODevice(&buf);
  iio.setFormat("XPM");
  iio.setDescription("Created by KDE Draw");
  
  if(iio.write())
  {
    QClipboard *cb = kapp->clipboard();
    cb->setText(str.data());
    //debug(str.data());
  }
  else
  {
    QString msg = i18n("There was an copying:\nto the clipboard!");
    KMsgBox::message(this, i18n("Warning"), msg.data(), KMsgBox::EXCLAMATION);
  }
  buf.close();
  delete sel;
  isselecting = false;
}

void KIconEditGrid::editPaste(bool paste)
{
  bool ok = false;
  const QImage *tmp = clipboardImage(ok);
  fixTransparence((QImage*)tmp);

  Properties *pprops = props(this);

  if(ok)
  {
    if( (tmp->size().width() > img->size().width()) || (tmp->size().height() > img->size().height()) )
    {
      if(KMsgBox::yesNo(this, i18n("Warning"),
          i18n("The clipboard image is larger than the current image!\nPaste as new image?")) == 1)
      {
        editPasteAsNew();
      }
      delete tmp;
      return;
    }
    else if(!paste)
    {
      ispasting = true;
      cbsize = tmp->size();
      //debug("insrect size: %d x %d", insrect.width(), insrect.height());
      return;
      emit newmessage(i18n("Pasting"));
    }
    else
    {
      //debug("KIconEditGrid: Pasting at: %d x %d", insrect.x(), insrect.y());

      QApplication::setOverrideCursor(waitCursor);

      for(int y = insrect.y(), ny = 0; y < numRows(), ny < insrect.height(); y++, ny++)
      {
        uint *l = ((uint*)img->scanLine(y)+insrect.x());
        uint *cl = (uint*)tmp->scanLine(ny);
        for(int x = insrect.x(), nx = 0; x < numCols(), nx < insrect.width(); x++, nx++, l++, cl++)
        {
          if(*cl != TRANSPARENT || pprops->pastetransparent)
          {
            *l = *cl;
            setColor((y*numCols())+x, (uint)*cl, false);
          }
        }
      }
      updateColors();
      repaint(insrect.x()*cellSize(), insrect.y()*cellSize(),
              insrect.width()*cellSize(), insrect.height()*cellSize(), false);

      QApplication::restoreOverrideCursor();

      modified = true;
      p = *img;
      emit changed(QPixmap(p));
      emit sizechanged(numCols(), numRows());
      emit colorschanged(numColors(), data());
      emit newmessage(i18n("Done pasting"));
    }
  }
  else
  {
    QString msg = i18n("Invalid pixmap data in clipboard!\n");
    KMsgBox::message(this, i18n("Warning"), msg.data());
  }
  delete tmp;
}

void KIconEditGrid::editPasteAsNew()
{
  bool ok = false;
  const QImage *tmp = clipboardImage(ok);

  if(ok)
  {
    *img = *tmp;
    load(img);
    modified = true;
    //repaint(viewRect(), false);

    p = *img;
    emit changed(QPixmap(p));
    emit sizechanged(numCols(), numRows());
    emit colorschanged(numColors(), data());
    emit newmessage(i18n("Done pasting"));
  }
  else
  {
    QString msg = i18n("Invalid pixmap data in clipboard!\n");
    KMsgBox::message(this, i18n("Warning"), msg.data());
  }
  delete tmp;
}

//#if QT_VERSION >= 140
void KIconEditGrid::editResize()
{
  debug("KIconGrid::editResize");
#if QT_VERSION >= 140
  KResize *rs = new KResize(this, 0, true, QSize(numCols(), numRows()));
  if(rs->exec())
  {
    const QSize s = rs->getSize();
    *img = img->smoothScale(s.width(), s.height());
    load(img);

    modified = true;
  }
  delete rs;
#endif
}
//#endif

void KIconEditGrid::setSize(const QSize s)
{
  debug("::setSize: %d x %d", s.width(), s.height());
  img->create(s.width(), s.height(), 32);
  img->fill(TRANSPARENT);
  load(img);
}

void KIconEditGrid::createCursors()
{
  KIconLoader *loader = kapp->getIconLoader();
  QBitmap bmp, mask(22, 22);
  QPixmap pix;

  cursor_normal = QCursor(arrowCursor);

  pix = loader->loadIcon("paintbrush.xpm");
  if(pix.isNull())
  {
    cursor_paint = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: paintbrush.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursor_paint = QCursor(bmp, mask, 6, 18);
  }

  pix = loader->loadIcon("flood.xpm");
  if(pix.isNull())
  {
    cursor_flood = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: flood.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursor_flood = QCursor(bmp, mask, 5, 16);
  }

  pix = loader->loadIcon("aim.xpm");
  if(pix.isNull())
  {
    cursor_aim = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: aim.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursor_aim = QCursor(bmp, mask);
  }

  pix = loader->loadIcon("spraycan.xpm");
  if(pix.isNull())
  {
    cursor_spray = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: aim.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    bmp = pix;
    cursor_spray = QCursor(bmp, mask);
  }

  pix = loader->loadIcon("eraser.xpm");
  if(pix.isNull())
  {
    cursor_erase = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: aim.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    bmp = pix;
    cursor_erase = QCursor(bmp, mask, 5, 16);
  }
}

void KIconEditGrid::setTool(DrawTool t)
{
  btndown = false;
  tool = t;

  if(tool != SelectRect && tool != SelectCircle)
    isselecting = false;

  switch( tool )
  {
    case SelectRect:
      isselecting = true;
      setCursor(cursor_aim);
      break;
    case SelectCircle:
      isselecting = true;
      setCursor(cursor_aim);
      break;
    case Line:
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    case FilledRect:
    case Rect:
      setCursor(cursor_aim);
      break;
    case Freehand:
      setCursor(cursor_paint);
      break;
    case Spray:
      setCursor(cursor_spray);
      break;
    case Eraser:
      setCursor(cursor_erase);
      currentcolor = TRANSPARENT;
      break;
    case FloodFill:
      setCursor(cursor_flood);
      break;
    case Find:
      debug("Tool == Find");
      setCursor(arrowCursor);
      debug("Tool == Find");
      break;
    default:
      break;
  }
}

void KIconEditGrid::drawFlood(int x, int y, uint oldcolor)
{
  if((!img->valid(x, y)) || (colorAt((y * numCols())+x) != oldcolor) || (colorAt((y * numCols())+x) == currentcolor))
    return;

  *((uint*)img->scanLine(y) + x) = currentcolor;
  setColor((y*numCols())+x, currentcolor, false);

  drawFlood(x, y-1, oldcolor);
  drawFlood(x, y+1, oldcolor);
  drawFlood(x-1, y, oldcolor);
  drawFlood(x+1, y, oldcolor);
}

void KIconEditGrid::drawSpray(QPoint point)
{
  int x = (point.x()-5);
  int y = (point.y()-5);
  //debug("drawSpray() - %d X %d", x, y);
  
  pntarray.resize(0);
  int points = 0;
  for(int i = 1; i < 4; i++, points++)
  {
    int dx = (rand() % 10);
    int dy = (rand() % 10);
    pntarray.putPoints(points, 1, x+dx, y+dy);
  }

  drawPointArray(pntarray, Draw);
}

void KIconEditGrid::drawEllipse(bool drawit)
{
  if(drawit)
  {
    drawPointArray(pntarray, Draw);
    p = *img;
    emit changed(p);
    //repaint(pntarray.boundingRect());
    return;
  }

  QPointArray a(pntarray.copy());
  int x = start.x(), y = start.y(), cx, cy;
  //cx = (x > end.x()) ? (x - end.x()) : (end.x() - x);
  if(x > end.x())
  {
    cx = x - end.x();
    x = x - cx;
  }
  else
    cx = end.x() - x;
  if(y > end.y())
  {
    cy = y - end.y();
    y = y - cy;
  }
  else
    cy = end.y() - y;
  //cy = (y > end.y()) ? (y - end.y()) : (end.y() - y);
  int d = (cx > cy) ? cx : cy;

  //debug("%d, %d - %d %d", x, y, d, d);
  pntarray.resize(0);
  drawPointArray(a, Mark);
  //repaint();
  if(tool == Circle || tool == FilledCircle || tool == SelectCircle)
    pntarray.makeEllipse(x, y, d, d);
  else if(tool == Ellipse || tool == FilledEllipse)
    pntarray.makeEllipse(x, y, cx, cy);

  if((tool == FilledEllipse) || (tool == FilledCircle) || (tool == SelectCircle))
  {
    int s = pntarray.size();
    int points = s;
    for(int i = 0; i < s; i++)
    {
      int x = pntarray[i].x();
      int y = pntarray[i].y();
      for(int j = 0; j < s; j++)
      {
         if((pntarray[j].y() == y) && (pntarray[j].x() > x))
         {
           for(int k = x; k < pntarray[j].x(); k++, points++)
             pntarray.putPoints(points, 1, k, y);
           break;
         }
      }
    }
  }

  drawPointArray(pntarray, Mark);
  if(tool == SelectCircle && pntarray.size() > 0 && !ispasting)
    emit selecteddata(true);
}

void KIconEditGrid::drawRect(bool drawit)
{
  if(drawit)
  {
    drawPointArray(pntarray, Draw);
    p = *img;
    emit changed(p);
    //repaint();
    return;
  }

  QPointArray a(pntarray.copy());
  int x = start.x(), y = start.y(), cx, cy;
  //cx = (x > end.x()) ? (x - end.x()) : (end.x() - x);
  if(x > end.x())
  {
    cx = x - end.x();
    x = x - cx;
  }
  else
    cx = end.x() - x;
  if(y > end.y())
  {
    cy = y - end.y();
    y = y - cy;
  }
  else
    cy = end.y() - y;

  //debug("%d, %d - %d %d", x, y, cx, cy);
  pntarray.resize(0);
  drawPointArray(a, Mark); // remove previous marking

  int points = 0;

  if(tool == FilledRect || (tool == SelectRect))
  {
    for(int i = x; i < x+cx; i++)
    {
      for(int j = y; j < y+cy; j++, points++)
        pntarray.putPoints(points, 1, i, j);
    }
  }
  else
  {
    for(int i = x; i <= x+cx; i++, points++)
      pntarray.putPoints(points, 1, i, y);
    for(int i = y; i <= y+cy; i++, points++)
      pntarray.putPoints(points, 1, x, i);
    for(int i = x; i <= x+cx; i++, points++)
      pntarray.putPoints(points, 1, i, y+cy);
    for(int i = y; i <= y+cy; i++, points++)
      pntarray.putPoints(points, 1, x+cx, i);
  }

  drawPointArray(pntarray, Mark);
  if(tool == SelectRect && pntarray.size() > 0 && !ispasting)
    emit selecteddata(true);
}

void KIconEditGrid::drawLine(bool drawit)
{
  if(drawit)
  {
    drawPointArray(pntarray, Draw);
    p = *img;
    emit changed(p);
    //repaint();
    return;
  }

  QPointArray a(pntarray.copy());
  pntarray.resize(0);
  drawPointArray(a, Mark); // remove previous marking

  int x, y, dx, dy, delta; // = start.x(), y = start.y(), cx = end.x(), cy = end.y();
  dx = end.x() - start.x();
  dy = end.y() - start.y();
  x = start.x();
  y = start.y();

  delta = QMAX(abs(dx), abs(dy));
  if (delta > 0) {
    dx /= delta;
    dy /= delta;
    for(int i = 0; i <= delta; i++) {
      pntarray.putPoints(i, 1, x, y);
      x += dx;
      y += dy;
    }
  }
  drawPointArray(pntarray, Mark);
}

void KIconEditGrid::drawPointArray(QPointArray a, DrawAction action)
{
  QRect rect = a.boundingRect();
  bool update = false;

  int s = a.size(); //((rect.size().width()) * (rect.size().height()));
  for(int i = 0; i < s; i++)
  {
    int x = a[i].x();
    int y = a[i].y();
    //if(img->valid(x, y) && !QSize(x, y).isNull() && rect.contains(QPoint(x, y)))
    if(img->valid(x, y) && rect.contains(QPoint(x, y)))
    {
      //debug("x: %d - y: %d", x, y);
      switch( action )
      {
        case Draw:
        {
          *((uint*)img->scanLine(y) + x) = currentcolor; //colors[cell]|OPAQUE;
          int cell = y * numCols() + x;
          setColor( cell, currentcolor, false );
          modified = true;
          update = true;
          //updateCell( y, x, FALSE );
          break;
        }
        case Mark:
        case UnMark:
          repaint(x*cellsize,y*cellsize, cellsize, cellsize, false);
          //updateCell( y, x, true );
          break;
        default:
          break;
      }
    }
  }
  if(update)
  {
    updateColors();
    repaint(rect.x()*cellSize()-1, rect.y()*cellSize()-1,
        rect.width()*cellSize()+1, rect.height()*cellSize()+1, false);
    pntarray.resize(0);
  }

}

bool KIconEditGrid::isMarked(QPoint point)
{
  return isMarked(point.x(), point.y());
}

bool KIconEditGrid::isMarked(int x, int y)
{
  if(((y * numCols()) + x) == selected)
    return true;

  int s = pntarray.size();
  for(int i = 0; i < s; i++)
  {
    if(y == pntarray[i].y() && x == pntarray[i].x())
      return true;
  }

  return false;
}

// Fast diffuse dither to 3x3x3 color cube
// Based on Qt's image conversion functions
static bool kdither_32_to_8( const QImage *src, QImage *dst )
{
    register QRgb *p;
    uchar  *b;
    int	    y;
	
	//printf("kconvert_32_to_8\n");
	
    if ( !dst->create(src->width(), src->height(), 8, 256) ) {
		warning("KPixmap: destination image not valid\n");
		return FALSE;
	}

    int ncols = 256;

    static uint bm[16][16];
    static int init=0;
    if (!init) {
		// Build a Bayer Matrix for dithering

		init = 1;
		int n, i, j;

		bm[0][0]=0;

		for (n=1; n<16; n*=2) {
	    	for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
		    	bm[i][j]*=4;
		    	bm[i+n][j]=bm[i][j]+2;
		    	bm[i][j+n]=bm[i][j]+3;
		    	bm[i+n][j+n]=bm[i][j]+1;
			}
	    	}
		}

		for (i=0; i<16; i++)
	    	for (j=0; j<16; j++)
			bm[i][j]<<=8;
    }

    dst->setNumColors( ncols );

#define MAX_R 2
#define MAX_G 2
#define MAX_B 2
#define INDEXOF(r,g,b) (((r)*(MAX_G+1)+(g))*(MAX_B+1)+(b))

	int rc, gc, bc;

	for ( rc=0; rc<=MAX_R; rc++ )		// build 2x2x2 color cube
	    for ( gc=0; gc<=MAX_G; gc++ )
		for ( bc=0; bc<=MAX_B; bc++ ) {
		    dst->setColor( INDEXOF(rc,gc,bc),
			qRgb( rc*255/MAX_R, gc*255/MAX_G, bc*255/MAX_B ) );
		}	

	int sw = src->width();
	int* line1[3];
	int* line2[3];
	int* pv[3];

	line1[0] = new int[src->width()];
	line2[0] = new int[src->width()];
	line1[1] = new int[src->width()];
	line2[1] = new int[src->width()];
	line1[2] = new int[src->width()];
	line2[2] = new int[src->width()];
	pv[0] = new int[sw];
	pv[1] = new int[sw];
	pv[2] = new int[sw];

	for ( y=0; y < src->height(); y++ ) {
	    p = (QRgb *)src->scanLine(y);
	    b = dst->scanLine(y);
		int endian = (QImage::systemByteOrder() == QImage::BigEndian);
		int x;
		uchar* q = src->scanLine(y);
		uchar* q2 = src->scanLine(y+1 < src->height() ? y + 1 : 0);
		for (int chan = 0; chan < 3; chan++) {
		    b = dst->scanLine(y);
		    int *l1 = (y&1) ? line2[chan] : line1[chan];
		    int *l2 = (y&1) ? line1[chan] : line2[chan];
		    if ( y == 0 ) {
			for (int i=0; i<sw; i++)
			    l1[i] = q[i*4+chan+endian];
		    }
		    if ( y+1 < src->height() ) {
			for (int i=0; i<sw; i++)
			    l2[i] = q2[i*4+chan+endian];
		    }
		    // Bi-directional error diffusion
		    if ( y&1 ) {
			for (x=0; x<sw; x++) {
			    int pix = QMAX(QMIN(2, (l1[x] * 2 + 128)/ 255), 0);
			    int err = l1[x] - pix * 255 / 2;
			    pv[chan][x] = pix;

			    // Spread the error around...
			    if ( x+1<sw ) {
				l1[x+1] += (err*7)>>4;
				l2[x+1] += err>>4;
			    }
			    l2[x]+=(err*5)>>4;
			    if (x>1) 
				l2[x-1]+=(err*3)>>4;
			}
		    } else {
			for (x=sw; x-->0; ) {
			    int pix = QMAX(QMIN(2, (l1[x] * 2 + 128)/ 255), 0);
			    int err = l1[x] - pix * 255 / 2;
			    pv[chan][x] = pix;

			    // Spread the error around...
			    if ( x > 0 ) {
				l1[x-1] += (err*7)>>4;
				l2[x-1] += err>>4;
			    }
			    l2[x]+=(err*5)>>4;
			    if (x+1 < sw) 
				l2[x+1]+=(err*3)>>4;
			}
		    }
		}
		if (endian) {
		    for (x=0; x<sw; x++) {
			*b++ = INDEXOF(pv[2][x],pv[1][x],pv[0][x]);
		    }
		} else {
		    for (x=0; x<sw; x++) {
			*b++ = INDEXOF(pv[0][x],pv[1][x],pv[2][x]);
		    }
		}
	}

	delete line1[0];
	delete line2[0];
	delete line1[1];
	delete line2[1];
	delete line1[2];
	delete line2[2];
	delete pv[0];
	delete pv[1];
	delete pv[2];
	
#undef MAX_R
#undef MAX_G
#undef MAX_B
#undef INDEXOF

    return TRUE;
}

// this doesn't work the way it should but the way KPixmap does.
void KIconEditGrid::mapToKDEPalette()
{
  QImage dest;

  kdither_32_to_8(img, &dest);
  *img = dest.convertDepth(32);

  for(int y = 0; y < img->height(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < img->width(); x++, l++)
    {
      if(*l < 0xff000000)
      {
        *l = *l | 0xff000000;
      }
    }
  }

  load(img);
  return;
/*
#if QT_VERSION > 140
  *img = img->convertDepthWithPalette(32, iconpalette, 42);
  load(img);
  return;
#endif
*/
  QApplication::setOverrideCursor(waitCursor);
  for(int y = 0; y < numRows(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < numCols(); x++, l++)
    {
      if(*l != TRANSPARENT)
      {
        if(!iconcolors.contains(*l))
          *l = iconcolors.closestMatch(*l);
      }
    }
  }
  load(img);
  modified = true;
  QApplication::restoreOverrideCursor();
}

void KIconEditGrid::grayScale()
{
  for(int y = 0; y < numRows(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < numCols(); x++, l++)
    {
      if(*l != TRANSPARENT)
      {
        uint c = (qBlue(*l) + qRed(*l) + qGreen(*l))/3;
        *l = QColor(c, c, c).rgb()|OPAQUE_MASK;
      }
    }
  }
  load(img);
  modified = true;
}


