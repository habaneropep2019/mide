// $Id: canvas.cpp,v 1.16.2.2 1999/04/08 21:23:47 dfaure Exp $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <qpainter.h>
#include <qimage.h>
#include <qwmatrix.h>
#include <klocale.h>
#include <kapp.h>
#include <math.h>
#include "canvas.h"
#include "tool.h"

#define CUT_FILL_RGB 0xFFFFFF //white

// static initialisers
// can't construct a pixmap before qapplication
QPixmap *Canvas::clipboardPix = NULL;
int Canvas::inst = 0;

Canvas::Canvas(int width, int height,
	       QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  currentTool= 0;
  s= INACTIVE;
  matrix= new QWMatrix;
  zoomed= 0;

  // Create pixmap
  pix= new QPixmap(width, height);
  if (!pix) {
    KDEBUG(KDEBUG_INFO, 3000, "Canvas::Canvas(): Cannot create pixmap\n");
    exit(1);
  }

  pix->fill(QColor("white"));

  setZoom(100);

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
  emit sizeChanged();
  
  if ((0 == inst) && (NULL == Canvas::clipboardPix)) {
    // debug("install clipboard");
    Canvas::clipboardPix = new QPixmap;
  }
  Canvas::inst++;
  
}

Canvas::Canvas(const char *filename, QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  currentTool= 0;
  s= INACTIVE;
  zoomed= 0;
  matrix= new QWMatrix;

  // Create pixmap
  pix= new QPixmap(filename);
  if (!pix) {
    KDEBUG(KDEBUG_INFO, 3000, "Canvas::Canvas(): Cannot create pixmap\n");
    exit(1);
  }

  resize(pix->width(), pix->height());

  setZoom(100);

  emit sizeChanged();

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
  
  if ((0 == inst) && (NULL == Canvas::clipboardPix)) {
    // debug("install clipboard");
    Canvas::clipboardPix = new QPixmap;
  }
  Canvas::inst++;
  
}

Canvas::~Canvas()
{
  /*
  inst--;
  if ((0 == inst) && (NULL != Canvas::clipboardPix)) {
    delete Canvas::clipboardPix;
    Canvas::clipboardPix = NULL;
  }
  */
}   

bool Canvas::isModified()
{
  return modified_;
}

void Canvas::clearModified()
{
  modified_= false;
}

void Canvas::markModified()
{
  if (!modified_) {
    modified_= true;
    KDEBUG(KDEBUG_INFO, 3000, "Canvas: emitting modified()\n");
  }
  emit modified();
}

void Canvas::setSelection(const QRect &rect)
{
  selection_= rect;
  haveSelection_= true;
  emit selection(true);
}

const QRect &Canvas::selection()
{
  return selection_;
}

void Canvas::clearSelection()
{
  haveSelection_= false;
  selection_= QRect(0,0,0,0);
  emit selection(false);
}

QPixmap *Canvas::selectionData()
{
  QPixmap *p;

  if (haveSelection_) {
    p= new QPixmap(selection_.width(), selection_.height());
    bitBlt(p, 0, 0, pix,
	   selection_.left(), selection_.top(),
	   selection_.width(), selection_.height(),
	   CopyROP, true);
  }
  else
    p= 0;

  return p;
}

// -------- CUT / COPY / PASTE ------------

/* currently not implemented */
void 
Canvas::cut()
{
  if (haveSelection_) {
    // copy the selection and fill the 
    // copied rect with CUT_FILL_RGB, mostly white, see definition above
    copy();
    const QColor c((QRgb)CUT_FILL_RGB);
    QPixmap p(selection_.size());
    p.fill(c);
    bitBlt(pix, selection_.topLeft(), &p, selection_, CopyROP);    
  }
}


void 
Canvas::copy()
{
  if (haveSelection_) {
    Canvas::clipboardPix->resize(selection_.size());
    const QPoint p(0,0);
    bitBlt(Canvas::clipboardPix, p, pix, selection_, CopyROP);
    clearSelection();
    emit clipboard(true);
  }
}

void
Canvas::paste()
{
// #warning "paste not implemented"
  warning("paste from clipboard not implemented");
  
  clearSelection();
}


// ---------- ZOOM ------------------------

void Canvas::setZoom(int z)
{
  QPainter p;
  int w, h;

  zoomFactor= z;
  matrix->reset();
  matrix->scale((float) z/100, (float) z/100);

  if (zoomed != 0)
    delete zoomed;

  w= (int) (pix->width()* ((float) zoomFactor/100));
  h= (int) (pix->height()*((float) zoomFactor/100));

  zoomed= new QPixmap(w, h);
  zoomed->fill(QColor("white"));

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  if ((w != width()) || (h != height())) {
    resize(w,h);
    emit sizeChanged();
  }
  repaint(0);
}

void Canvas::updateZoomed()
{
  QPainter p;
/*  int w, h; */

  zoomed->fill(QColor("white"));

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  repaint(0);
}

int Canvas::zoom()
{
  return zoomFactor;
}

void Canvas::activate(Tool *t)
{
  assert(!isActive());
  currentTool= t;
  s= ACTIVE;
}

void Canvas::deactivate()
{
  assert(isActive());
  s= INACTIVE;
  currentTool= 0;
}

QPixmap *Canvas::pixmap()
{
  return pix;
}

QPixmap *Canvas::zoomedPixmap()
{
  return zoomed;
}

void Canvas::setPixmap(QPixmap *px)
{
  QPainter p;
  int w, h;

  *pix= *px;
  emit pixmapChanged(pix);

  delete zoomed;

  w= (int) (px->width()* ((float) zoomFactor/100));
  h= (int) (px->height()*((float) zoomFactor/100));

  zoomed= new QPixmap(w, h);

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  if ((w != width()) || (h != height())) {
    resize(w,h);
    emit sizeChanged();
  }
  repaint(0);
}

void Canvas::setDepth(int d)
{
  QImage i;
  QPixmap *px;

  assert((d == 1) || (d == 4) || (d == 8) || 
	 (d == 15) || (d == 16) ||
	 (d == 24) || (d == 32));

  if (d != pix->depth()) {
    i= pix->convertToImage();
    i.convertDepth(d);
    px= new QPixmap(pix->width(), pix->height(), d);
    *px= i;
    setPixmap(px);
    emit pixmapChanged(px);
    delete px;
  }
}

void Canvas::resizeImage(int w, int h)
{
  QWMatrix matrix;
  QPainter p;

  if ((w != pix->width()) || (h != pix->height())) {
    QPixmap *newpix= new QPixmap(w, h);
    matrix.scale((float) w/pix->width(), (float) h/pix->height());
    p.begin(newpix);
    p.setWorldMatrix(matrix);
    p.drawPixmap(0,0,*pix);
    p.end();

    delete pix;
    pix= newpix;
    setZoom(zoom());
    emit pixmapChanged(pix);
  }
  repaint(0);
}

void Canvas::paintEvent(QPaintEvent *)
{
  bitBlt(this, 0, 0, zoomed);
}

void Canvas::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Canvas::mousePressEvent() redirector called\n");
  if (isActive())
    currentTool->mousePressEvent(e);
} 

void Canvas::mouseMoveEvent(QMouseEvent *e)
{
  if (isActive())
    currentTool->mouseMoveEvent(e);
}

void Canvas::mouseReleaseEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Canvas::mouseReleaseEvent() redirector called\n");
  if (isActive())
    currentTool->mouseReleaseEvent(e);
}

bool Canvas::isActive()
{
  if (s == ACTIVE)
    return true;
  else
    return false;
}

bool Canvas::load(const char *filename, const char *format)
{
  bool s;
  QPixmap p;
  QPixmap q; // Fix UMR when reading transparent pixels (they hold junk)

  if (!format) { // format == NULL
    s= p.load(filename);
  } else {
    s= p.load(filename, format);
  }

  if (s) {
    q.resize(p.size());
    q.fill(QColor("white"));
    bitBlt(&q, 0, 0, &p);
    setPixmap(&q);
    emit pixmapChanged(pix);
  }

  repaint(0);

  return s;
}

bool Canvas::save(const char *filename, const char *format)
{
  bool s;

KDEBUG2(KDEBUG_INFO, 3000, "Canvas::save() file= %s, format= %s\n", filename, format);

  s= pix->save(filename, format);

KDEBUG1(KDEBUG_INFO, 3000, "Canvas::save() returning %d\n", s);

  return s;
}

void Canvas::keyPressEvent(QKeyEvent *e)
{
  //KDEBUG(KDEBUG_INFO, 3000, "Canvas::keyPressEvent() redirector called\n");
  if (isActive())
    currentTool->keyPressEvent(e);
}


void Canvas::keyReleaseEvent(QKeyEvent *e)
{
  //KDEBUG(KDEBUG_INFO, 3000, "Canvas::keyReleaseEvent() redirector called\n");
  if (isActive())
    currentTool->keyReleaseEvent(e);
}


#include "canvas.moc"
