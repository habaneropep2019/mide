#include "toplevel.h"
#include "toplevel.moc"

#include <qpixmap.h>
#include <qpainter.h>
#include <time.h>
#include <kapp.h>
#include <klocale.h>


TopLevel::TopLevel ()
  : KTopLevelWidget()
{
  world = new World(this);

  setupStatusBar();

  setView(world);

  startTimer(1000); 
  timerEvent(NULL);

  setFixedSize(world->width(), world->height()+statusbar->height());

  show();
}


TopLevel::~TopLevel ()
{
  delete statusbar;
}


void TopLevel::setupStatusBar()
{
  statusbar = new KStatusBar(this);
  statusbar->insertItem("", ID_LOCALTIME);
  statusbar->setInsertOrder(KStatusBar::LeftToRight);
  setStatusBar(statusbar);
}  


void TopLevel::timerEvent(QTimerEvent *)
{
  char buffer[160];
  char buffer1[80];

  time_t t = time(NULL) + World::offset;

  struct tm *tmp = localtime(&t);
  strftime(buffer, 80, klocale->translate  ("Local time: %x, %H:%M:%S"),tmp);

  tmp = gmtime(&t);
  strftime(buffer1, 80, klocale->translate ("  GMT Time: %x, %H:%M:%S"),tmp);

  strcat(buffer, buffer1);
  statusbar->changeItem(buffer, ID_LOCALTIME);

  world->setTime();
}
