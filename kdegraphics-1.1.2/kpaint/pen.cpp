// $Id: pen.cpp,v 1.12 1998/10/18 19:59:57 habenich Exp $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "pen.h"
#include "app.h"

Pen::Pen(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;
  tooltip= i18n("Pen");
  props= Tool::HasLineProperties;
}

void Pen::activating()
{
  KDEBUG1(KDEBUG_INFO, 3000, "Pen::activating() hook called canvas=%p\n", canvas);
  drawing= FALSE;

  canvas->setCursor(crossCursor);
}

void Pen::mousePressEvent(QMouseEvent *e)
{
  int x,y;
  QPainter painter1;
  QPainter painter2;
  QWMatrix m;

  KDEBUG(KDEBUG_INFO, 3000, "Pen::mousePressEvent() handler called\n");
  
  if (isActive()) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "Pen: Warning button press received while drawing\n");
    }
    x= (e->pos()).x();
    y= (e->pos()).y();
    activeButton= e->button();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));

    painter1.begin(canvas->pixmap());

    if (activeButton == LeftButton)
      painter1.setPen(leftpen);
    else
      painter1.setPen(rightpen);
    painter1.setWorldMatrix(m);

    painter2.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      painter2.setPen(leftpen);
    else
      painter2.setPen(rightpen);

    painter1.drawPoint(x, y);
    painter2.drawPoint(x, y);

    painter1.end();
    painter2.end();

    canvas->repaint(0);
    lastx= x;
    lasty= y;
    drawing= TRUE;
  } 
  if (!isActive()) {
    KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}


void Pen::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter painter1;
  QPainter painter2;
  QWMatrix m;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if (drawing) {
      if ((x != lastx) || (y != lasty)) {
	emit modified();
	m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));

	painter1.begin(canvas->pixmap());

	if (activeButton == LeftButton)
	  painter1.setPen(leftpen);
	else
	  painter1.setPen(rightpen);

	painter1.setWorldMatrix(m);

	painter2.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  painter2.setPen(leftpen);
	else
	  painter2.setPen(rightpen);

	painter1.drawLine(lastx, lasty, x, y);
	painter2.drawLine(lastx, lasty, x, y);

	lastx= x;
	lasty= y;

	painter1.end();
	painter2.end();
	canvas->repaint(0);
      }
    }
  }
  else {
    KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

void Pen::mouseReleaseEvent(QMouseEvent *e)
{
  KDEBUG(KDEBUG_INFO, 3000, "Pen::mouseReleaseEvent() handler called\n");
  if (isActive() && (e->button() == activeButton) && drawing)
    drawing= FALSE;
  if (!isActive()) {
    KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *Pen::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");

  pixdir.append("pen.xpm");
  return new QPixmap(pixdir);
}

#include "pen.moc"
