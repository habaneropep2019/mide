
#include "text.moc"

#include <qpainter.h>
#include <qkeycode.h>
#include "app.h"

#define TIMER_INTERVALL 500
#define CURSOR_WIDTH 9 // has to be odd number
#define CURSOR_HEIGHT 15

Text::Text()
  : Tool(),
    startx(0), // starty(0),
    cursorH(CURSOR_HEIGHT),
    lastx(0), lasty(0),
    drawing(false), showingC(false),
    charBuffer(0)
{
  lineStack.setAutoDelete(true);  
}

Text::Text(const char *toolname)
  : Tool(toolname),
    startx(0), // starty(0),
    cursorH(CURSOR_HEIGHT),
    lastx(0), lasty(0),
    drawing(false), showingC(false),
    charBuffer(0)
{
  lineStack.setAutoDelete(true);  
}

void 
Text::mousePressEvent(QMouseEvent *e)
{
  killTimers();
  if (showingC) {
    paintC();
  }
  if (drawing) {
    // save your work
    paintBuffer();
    drawing = false;
  }
  lastx = e->x();
  lasty = e->y();
}

void 
Text::mouseReleaseEvent(QMouseEvent *e)
{
  if (abs(e->x()-lastx)<3 && abs(e->y()-lasty)<3) {
    int *i;
    drawing = true;
    charBuffer = "";
    startx = lastx = e->x();
    starty = lasty = e->y();
    //    debug("linestack count %i",lineStack.count());
    // clear stack
    while ((i=lineStack.pop()) != NULL)
      delete i;
    //    debug("linestack count %i",lineStack.count());
    // here paint a blinking textmarker
    //    debug("text enabled");
    cursorH = canvas->fontMetrics().ascent();
    cursorW = (cursorH/2)+1;
    paintC();
    startTimer(TIMER_INTERVALL);
  }
}

void 
Text::keyPressEvent(QKeyEvent *e)
{
  // FIXME !!!!!!!!!!!!
  // maybe a bit long time to work this function
  // next time implement a queue with chars, working asynchron

  QPainter paint;
  paint.begin(canvas->zoomedPixmap());

  int key = e->key();
  //  debug("keycode %i",key);
  if ( Key_Return == key || Key_Enter == key ) { //  "return" 
    if (showingC) {
      paintC(&paint);
    }
    // goto next line marked with startx
    lineStack.push(new int(lastx));
    lastx = startx;
    lasty += paint.fontMetrics().height();
    if (showingC) {
      paintC(&paint);
    }
    paint.end();
    canvas->repaint();
    charBuffer += '\n';
    return;    
  }

  if (Key_Backspace == key || Key_Delete == key ) { // one char back
    char lastc;
    if (charBuffer.length() == 0) {
      paint.end();
      return;
    }
    if (showingC) {
      paintC(&paint);
    }
    lastc = charBuffer.at(charBuffer.length()-1);
    // debug("remove %c", lastc);
    if (lastc == '\n') {
      charBuffer.remove(charBuffer.length()-1,1);
      // goto previous line
      int *i = lineStack.pop();
      lasty -= paint.fontMetrics().height();
      lastx = *i;
      delete i;
    }
    else {
      int w = paint.fontMetrics().width(lastc);
      QRect r = paint.fontMetrics().boundingRect(lastc);
      char str[] = {lastc,0};
      lastx -= w;
      paint.setRasterOp(NotROP);
      paint.drawText(lastx, lasty, str);
      charBuffer.remove(charBuffer.length()-1,1);
      canvas->repaint(r);
    }
    if (showingC) {
      paintC(&paint);
    }
    paint.end();
    return;
  }
  
  int w, ascii = e->ascii();
  char str[] = {ascii,0};
  //  debug ("keyclick %s received",str);

  // draw char
  paint.drawText(lastx, lasty, str);
  w = paint.fontMetrics().width(str[0]);
  QRect r = paint.fontMetrics().boundingRect(str[0]);
  // set cursor one left
  if (showingC) {
    paintC(&paint);
    lastx += w;
    paintC(&paint);
  }
  else {
    lastx += w;
  }
  paint.end();
  canvas->repaint(r);
  charBuffer += ascii;
}

void 
Text::activating()
{
  canvas->setCursor(crossCursor);
}

void
Text::deactivating()
{
  killTimers();
  paintBuffer();
  // erase the selector frame
  if (showingC) {
    paintC();
  }
}

QPixmap *
Text::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_toolbardir().copy();
  //  pixdir.append("/kpaint/toolbar/");
  pixdir.append("/largetext.xpm");
  return new QPixmap(pixdir);
}

void 
Text::timerEvent(QTimerEvent *)
{
  paintC();
}

void
Text::paintC(QPainter *p)
{
  // draw textcursor
  QRect pr(lastx-cursorW/2, lasty-cursorH,
	   cursorW, cursorH+1); // paint area

  QPainter *paint;
  if (NULL==p) {
    paint = new QPainter();
    paint->begin(canvas->zoomedPixmap());
  } else {
    paint = p;
  }
  //  paint.setPen(QPen(green, 0, DashLine));
  paint->setRasterOp(DEFAULT_RASTER_OP);
  paint->drawLine(pr.topLeft(), pr.topRight());
  paint->drawLine(pr.bottomLeft(), pr.bottomRight());
  paint->drawLine(lastx, pr.y(), lastx, lasty);
  //  paint.drawRect(startx, starty, lastx-startx, lasty-starty);
  if (NULL==p) {
    paint->end();
    delete paint;
  }
  canvas->repaint(pr);
  showingC = !showingC;
}

void
Text::paintBuffer()
{
  if ( !charBuffer.isEmpty()) {
    QRect r;
    QPainter paint;
    paint.begin(canvas->pixmap());
    // paint text
    //    paint.drawText(startx, starty, charBuffer);
    paint.drawText(startx, starty - paint.fontMetrics().ascent(),
		   10,10, (DontClip|ExpandTabs),
		   charBuffer, -1, &r );
    paint.end();
    canvas->updateZoomed();
    canvas->repaint(r);    
    emit modified();
  }
}
