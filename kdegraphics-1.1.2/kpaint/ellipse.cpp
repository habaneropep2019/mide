// $Id: ellipse.cpp,v 1.14 1998/10/25 16:06:45 habenich Exp $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "ellipse.h"
#include "app.h"

Ellipse::Ellipse() : Tool()
{
  drawing= FALSE;
  tooltip= i18n("Ellipse");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

Ellipse::Ellipse(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;
  tooltip= i18n("Ellipse");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Ellipse::activating()
{
KDEBUG(KDEBUG_INFO, 3000, "Ellipse::activating() hook called\n");
  canvas->setCursor(crossCursor);
}

void Ellipse::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Ellipse::mousePressEvent() handler called\n");
  
  if (isActive()) {
    if (drawing) {
KDEBUG(KDEBUG_INFO, 3000, "Ellipse: Warning Left Button press received when pressed\n");
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
  if (!isActive()) {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

void Ellipse::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	// Erase old ellipse
	// The test is to prevent problems when the ellipse is smaller
	// than 2 by 2 pixels. (It leaves a point behind as the ellipse
	// grows).

	paint.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  paint.setPen(leftpen);
	else
	  paint.setPen(rightpen);

	paint.setRasterOp(DEFAULT_RASTER_OP);

	if (abs(lastx-startx)*abs(lasty-starty) >= 4) 
	  paint.drawEllipse(startx, starty, lastx-startx, lasty-starty);

	// Draw new ellipse
	if (abs(x-startx)*abs(y-starty) > 4) 
	  paint.drawEllipse(startx, starty, x-startx, y-starty);

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

void Ellipse::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

KDEBUG(KDEBUG_INFO, 3000, "Ellipse::mouseReleaseEvent() handler called\n");

  if (isActive() && (e->button() == activeButton) && drawing) {
    emit modified();
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old ellipse
    paint.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);

    paint.setRasterOp(DEFAULT_RASTER_OP);

    if (abs(lastx-startx)*abs(lasty-starty) > 4) 
      paint.drawEllipse(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    paint.begin(canvas->pixmap());

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);

    // Draw new ellipse
    if (abs(x-startx)*abs(y-starty) > 4) {
      if (activeButton == LeftButton) {
	paint.setPen(leftpen);
	paint.setBrush(leftbrush);
      }
      else {
	paint.setPen(rightpen);
	paint.setBrush(rightbrush);
      }
      paint.drawEllipse(startx, starty, x-startx, y-starty);
    }

    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *Ellipse::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");
  pixdir.append("ellipse.xpm");
  return new QPixmap(pixdir);
}

#include "ellipse.moc"
