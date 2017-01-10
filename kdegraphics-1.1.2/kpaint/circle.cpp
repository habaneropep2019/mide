// $Id: circle.cpp,v 1.13.2.1 1999/02/16 15:11:39 kulow Exp $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "math.h"
#include "circle.h"
#include "app.h"

Circle::Circle() : Tool()
{
  drawing= FALSE;
  tooltip= i18n("Circle");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

Circle::Circle(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;
  tooltip= i18n("Circle");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Circle::activating()
{
KDEBUG(KDEBUG_INFO, 3000, "Circle::activating() hook called\n");

  canvas->setCursor(crossCursor);
}

void Circle::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Circle::mousePressEvent() handler called\n");
  
  if (isActive()) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "Circle: Warning button press received while drawing\n");
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

void Circle::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  int bbx, bby, r;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	// Erase old circle
	// The test is to prevent problems when the circle is smaller
	// than 2 by 2 pixels. (It leaves a point behind as the circle
	// grows).

	r= (int) sqrt( (double)(startx-lastx)*(startx-lastx) +
		       (starty-lasty)*(starty-lasty) );

	bbx= startx-r;
	bby= starty-r;

	paint.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  paint.setPen(leftpen);
	else
	  paint.setPen(rightpen);

	paint.setRasterOp(DEFAULT_RASTER_OP);

	if (r >= 2)
	  paint.drawEllipse(bbx, bby, 2*r, 2*r);

	// Draw new circle
	r= (int) sqrt( (double)(startx-x)*(startx-x)+(starty-y)*(starty-y) );

	bbx= startx-r;
	bby= starty-r;

	if (r >= 2) 
	  paint.drawEllipse(bbx, bby, 2*r, 2*r);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning move event received when inactive (ignoring)\n");
  }
}

void Circle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;
  int bbx, bby, r;

KDEBUG(KDEBUG_INFO, 3000, "Circle::mouseReleaseEvent() handler called\n");

  if (isActive() && (e->button() == activeButton) && drawing) {
    emit modified();

    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old circle
    // The test is to prevent problems when the circle is smaller
    // than 2 by 2 pixels. (It leaves a point behind as the circle
    // grows).

    r= (int) sqrt( (startx-lastx)*(startx-lastx) +
		   (starty-lasty)*(starty-lasty) );

    bbx= startx-r;
    bby= starty-r;

    paint.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);

    paint.setRasterOp(DEFAULT_RASTER_OP);

    if (r >= 2)
      paint.drawEllipse(bbx, bby, 2*r, 2*r);

    paint.end();

    // Draw new circle
    r= (int) sqrt( (double)(startx-x)*(startx-x)+(starty-y)*(starty-y) );

    bbx= startx-r;
    bby= starty-r;

    paint.begin(canvas->pixmap());
    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);


    if (r >= 2) {
      if (activeButton == LeftButton) {
	paint.setPen(leftpen);
	paint.setBrush(leftbrush);
      }
      else {
	paint.setPen(rightpen);
	paint.setBrush(rightbrush);
      }
      paint.drawEllipse(bbx, bby, 2*r, 2*r);
    }

    lastx= x;
    lasty= y;

    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
}

QPixmap *Circle::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");
  pixdir.append("circle.xpm");
  return new QPixmap(pixdir);
}

#include "circle.moc"
