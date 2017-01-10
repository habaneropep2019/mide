// $Id: spraycan.cpp,v 1.11 1998/10/18 20:00:04 habenich Exp $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "spraycan.h"
#include "app.h"

SprayCan::SprayCan(const char *toolname) : Tool(toolname)
{
  drawing= FALSE;
  brushsize= 10;
  density= 100;
  tooltip= i18n("Spray Can");
  props= Tool::HasLineProperties | Tool::HasCustomProperties;

  timer= new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(drawDot()) );
}

SprayCan::~SprayCan()
{
  delete timer;
}

void SprayCan::activating()
{
KDEBUG1(KDEBUG_INFO, 3000, "SprayCan::activating() hook called canvas=%p\n", canvas);
  drawing= FALSE;

  canvas->setCursor(crossCursor);
}

void SprayCan::mouseMoveEvent(QMouseEvent *e)
{
  if (isActive() && drawing) {
      x= (e->pos()).x();
      y= (e->pos()).y();
  }
}

void SprayCan::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "SprayCan::mousePressEvent() handler called\n");
  
  if (isActive()) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "SprayCan: Warning Left Button press received when pressed\n");
    }
    x= (e->pos()).x();
    y= (e->pos()).y();
    activeButton= e->button();

    // Start the timer (multishot)
    timer->start(50, FALSE);

    drawing= TRUE;
  }
}


void SprayCan::mouseReleaseEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "SprayCan::mouseReleaseEvent() handler called\n");
  
  if (isActive() && (e->button() == activeButton)) {
    if (drawing) {
      // Stop the timer
      timer->stop();
    }
    drawing= FALSE;
    canvas->updateZoomed();
  }
}

void SprayCan::drawDot()
{
  int dx,dy;
  int i;
  QPainter painter1;
  QPainter painter2;
  QWMatrix m;

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

  painter1.drawPoint(x, y);
  painter2.drawPoint(x, y);

  for (i= 0; i < (density/20); i++) {
    dx= (rand() % (2*brushsize))-brushsize;
    dy= (rand() % (2*brushsize))-brushsize;
    painter1.drawPoint(x+dx, y+dy);
    painter2.drawPoint(x+dx, y+dy);
  }
  painter1.end();
  painter2.end();
  canvas->repaint(0);
}

QPixmap *SprayCan::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");

  pixdir.append("spraycan.xpm");
  return new QPixmap(pixdir);
}

#include "spraycan.moc"




