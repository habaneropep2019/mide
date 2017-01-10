// $Id: line.cpp,v 1.14 1998/10/25 16:06:49 habenich Exp $

// Note: When used to draw more than one line segment should use draw poly line
// functions to avoid glitches at the joints.

#include <kdebug.h>
#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "line.h"
#include "app.h"

Line::Line(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;
  tooltip= i18n("Line");
  props= Tool::HasLineProperties;
}

void Line::activating()
{
KDEBUG(KDEBUG_INFO, 3000, "Line::activating() hook called\n");

  canvas->setCursor(crossCursor);
}

void Line::mousePressEvent(QMouseEvent *e)
{
#if 0
  int x, y;
#endif
  QPainter paint;

  KDEBUG(KDEBUG_INFO, 3000, "RubberLine::mousePressEvent() handler called\n");

  if (isActive()) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "Line: Warning button press received while drawing\n");
    }
    else {
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      activeButton= e->button();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    }
  }

#if 0
  // This code used to allow multi segment lines (badly)
  // It is being replaced by a seperate polyline tool.
  else if (isActive() && (e->button() == RightButton) && drawing) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());
    paint.setPen(leftpen);
    paint.setRasterOp(XorROP);
    paint.drawLine(startx, starty, lastx, lasty);
    paint.setRasterOp(CopyROP);
    // Draw new line
    paint.drawLine(startx, starty, lastx, lasty);
    paint.end();

    startx= x;
    starty= y;
    lastx= startx;
    lasty= starty;
  }

  canvas->repaint(0);
#endif
  
  if (!isActive()) {
KDEBUG(KDEBUG_WARN, 3000, "Line: Warning event received when inactive (ignoring)\n");
  }
}

void Line::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  paint.setPen(leftpen);
	else
	  paint.setPen(rightpen);

	paint.setRasterOp(DEFAULT_RASTER_OP);

	// Draw new line
	paint.drawLine(startx, starty, x, y);
	// Erase old line
	paint.drawLine(startx, starty, lastx, lasty);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Line: Warning event received when inactive (ignoring)\n");
  }
}

void Line::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

KDEBUG(KDEBUG_INFO, 3000, "Line::mouseReleaseEvent() handler called\n");

  if (isActive() && drawing && (e->button() == activeButton)) {
    emit modified();
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());
    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);
    paint.setRasterOp(DEFAULT_RASTER_OP);
    paint.drawLine(startx, starty, lastx, lasty);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());
    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);
    paint.setWorldMatrix(m);

    paint.setRasterOp(CopyROP);
    // Draw new line
    paint.drawLine(startx, starty, lastx, lasty);
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *Line::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");
  pixdir.append("line.xpm");
  return new QPixmap(pixdir);
}

#include "line.moc"
