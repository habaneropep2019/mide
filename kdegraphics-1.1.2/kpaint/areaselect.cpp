// $Id: areaselect.cpp,v 1.8.2.1 1999/04/08 21:23:46 dfaure Exp $

#include <kdebug.h>
#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "areaselect.h"
#include "app.h"

#define TIMER_INTERVALL 500


AreaSelect::AreaSelect(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;
  showedSF= false;
  tooltip= klocale->translate("Area Selection");
  props= 0;
}

/*
AreaSelect::~AreaSelect()
{
}
*/

void 
AreaSelect::selection(bool b)
{
  // debug("AreaSelect::selection %i",b);
  if (!b) {
    disableSelection();
  }
}

void 
AreaSelect::disableSelection()
{
  // stop all the timer 
  killTimers();
  if (showedSF) {
    drawFrame(); // to erase the frame
  }
}

void AreaSelect::activating()
{
  KDEBUG(KDEBUG_INFO, 3000, "AreaSelect::activating() hook called\n");
  canvas->setCursor(crossCursor);
}

void AreaSelect::deactivating()
{
  KDEBUG(KDEBUG_INFO, 3000, "AreaSelect::deactivating() hook called\n");
  // erase old selection frame
  disableSelection();
  canvas->clearSelection();
}

void
AreaSelect::timerEvent(QTimerEvent *)
{
  drawFrame();
}

void AreaSelect::mousePressEvent(QMouseEvent *e)
{
  KDEBUG(KDEBUG_INFO, 3000, "AreaSelect::mousePressEvent() handler called\n");
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      KDEBUG(KDEBUG_WARN, 3000, "AreaSelect: Warning Left Button press received when pressed\n");
    }
    else {
      // erase old selection frame
      disableSelection();
      // mark new frame beginning
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    } 
  }
  if (!isActive()) {
    KDEBUG(KDEBUG_WARN, 3000, "AreaSelect: Warning event received when inactive (ignoring)\n");
  }
}

void AreaSelect::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());
	paint.setPen(QPen(green, 0, DashLine));
	paint.setRasterOp(DEFAULT_RASTER_OP);

	// Erase old rectangle
	paint.drawRect(startx, starty, lastx-startx, lasty-starty);
	// Draw new rectangle
	paint.drawRect(startx, starty, x-startx, y-starty);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

void AreaSelect::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

KDEBUG(KDEBUG_INFO, 3000, "AreaSelect::mouseReleaseEvent() handler called\n");

  if (isActive() && (e->button() == LeftButton)) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());
    paint.setPen(QPen(green, 0, DashLine));
    paint.setRasterOp(DEFAULT_RASTER_OP);
    paint.drawRect(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());
    //    paint.setPen(pen);
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);
    canvas->setSelection(QRect(startx, starty, x-startx, y-starty));
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);

    startTimer(TIMER_INTERVALL);
    showedSF= false;
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *
AreaSelect::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");
  pixdir.append("areaselect.xpm");
  return new QPixmap(pixdir);
}

void 
AreaSelect::drawFrame()
{
  QPainter paint;
  paint.begin(canvas->zoomedPixmap());
  paint.setPen(QPen(green, 0, DashLine));
  paint.setRasterOp(DEFAULT_RASTER_OP);
  paint.drawRect(startx, starty, lastx-startx, lasty-starty);
  paint.end();
  canvas->repaint(0);
  showedSF = !showedSF;
}

#include "areaselect.moc"
