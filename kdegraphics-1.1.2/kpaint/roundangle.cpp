// $Id: roundangle.cpp,v 1.3 1998/10/25 16:06:53 habenich Exp $

#include <kdebug.h>
#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "roundangle.h"
#include "app.h"

#define ROUNDNESS (40)

Roundangle::Roundangle(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;

  tooltip= i18n("Roundangle");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Roundangle::activating()
{
KDEBUG(KDEBUG_INFO, 3000, "Roundangle::activating() hook called\n");
  canvas->setCursor(crossCursor);
}

void Roundangle::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Roundangle::mousePressEvent() handler called\n");
  
  if (isActive()) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "RubberLine: Warning Left Button press received when pressed\n");
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

void Roundangle::mouseMoveEvent(QMouseEvent *e)
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

	// Erase old roundangle
	paint.drawRoundRect(startx, starty, lastx-startx, lasty-starty, 
			    ROUNDNESS, ROUNDNESS);
	// Draw new roundangle
	paint.drawRoundRect(startx, starty, x-startx, y-starty,
			    ROUNDNESS, ROUNDNESS);

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

void Roundangle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

KDEBUG(KDEBUG_INFO, 3000, "Roundangle::mouseReleaseEvent() handler called\n");

  if (isActive() && (e->button() == activeButton)) {
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
    paint.drawRoundRect(startx, starty, lastx-startx, lasty-starty,
			ROUNDNESS, ROUNDNESS);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());

    if (activeButton == LeftButton) {
      paint.setPen(leftpen);
      paint.setBrush(leftbrush);
    }
    else {
      paint.setPen(rightpen);
      paint.setBrush(rightbrush);
    }

    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);
    // Draw new roundangle
    paint.drawRoundRect(startx, starty, x-startx, y-starty, ROUNDNESS, ROUNDNESS);
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *Roundangle::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");
  pixdir.append("roundangle.xpm");
  return new QPixmap(pixdir);
}

#include "roundangle.moc"

