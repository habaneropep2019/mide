#include "world.h"
#include "world.moc"

#include "kinputline.h"
#include "astro.h"
#include "sunclock.h"

#include <kapp.h>
#include <kconfig.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <time.h>
#include <klocale.h>
#include <qmsgbox.h>
#include <qpushbt.h>
#include <kiconloader.h>
#include <math.h>


time_t World::offset = 0;


void TimeTip::maybeTip(const QPoint &pos)
{
  QString text = ((World*)parentWidget())->getTip(pos);

  if (text.length() > 0)
    tip(QRect(pos,QSize(1,1)), text);
}


World::World(QWidget *parent)
  : QWidget(parent)
{
  QFont        font("Courier", 10);
  QFontMetrics metric(font);
  KIconLoader  iconLoader;

  cleanMap = iconLoader.loadIcon("world.gif");
  if (cleanMap.isNull())
    abort();
  darkMap = iconLoader.loadIcon("world_dark.gif");
  if (darkMap.isNull())
    abort();
  map = cleanMap;

  illuMask = new QBitmap(map.width(), map.height());

  redFlag = iconLoader.loadIcon("flag-red.xpm");
  blueFlag = iconLoader.loadIcon("flag-blue.xpm");
  greenFlag = iconLoader.loadIcon("flag-green.xpm");
  yellowFlag = iconLoader.loadIcon("flag-yellow.xpm");  

  KConfig *config = kapp->getConfig();
  config->setGroup("General");
  illumination = config->readBoolEntry("Illumination", TRUE);

  loadFlags();
  repaint(FALSE);
  	
  // set up the menu
  QPopupMenu *flagsmenu = new QPopupMenu();
  flagsmenu->insertItem(redFlag, this, SLOT(addRedFlag()));
  flagsmenu->insertItem(greenFlag, this, SLOT(addGreenFlag()));
  flagsmenu->insertItem(blueFlag, this, SLOT(addBlueFlag()));
  flagsmenu->insertItem(yellowFlag, this, SLOT(addYellowFlag()));

  menu = new QPopupMenu();
  menu->insertItem(klocale->translate("&Add a flag"),flagsmenu);
  menu->insertItem(klocale->translate("&Delete nearest flag"), this, SLOT(deleteFlag()));
  menu->insertItem(klocale->translate("A&nnotate flag..."), this, SLOT(annotateFlag()));
  menu->insertSeparator();
  menu->insertItem(klocale->translate("Toggle &Illumination"), this, SLOT(illuminateMap()));
  menu->insertItem(klocale->translate("Toggle &Simulation"), this, SLOT(toggleSimulation()));
  menu->insertSeparator();
  menu->insertItem(klocale->translate("About &World Watch..."), this, SLOT(about()));

  setFont(font);
  setTime();
  resize(map.width(),map.height()+metric.height()+12);

  tip = new TimeTip(this);  

  show();

  offset = 0;
}


World::~World()
{
  delete tip;
  delete illuMask;
}

void World::setTime()
{
  if (offset != 0)
    offset += 7*86500;

  time_t t = time(NULL)+offset;
  struct tm *tmp = gmtime(&t);
  time_t sec = tmp->tm_hour*60*60 + tmp->tm_min*60 + tmp->tm_sec;

  int old_position = gmt_position;
  gmt_position = 470 * sec / 86400;

  if (old_position != gmt_position)
    repaint(FALSE);
}


void World::paintEvent(QPaintEvent *)
{
  updateMap();
  
  QPainter p;

  p.begin(this);
  p.setFont(font());

  if (gmt_position >= GREENWICH)
    {	
      p.drawPixmap(gmt_position-GREENWICH, 0,
                   map, 
                   0, 0,
                   map.width()-gmt_position+GREENWICH); 
      p.drawPixmap(0,0,
                   map,
                   map.width()-gmt_position+GREENWICH, 0,
                   gmt_position-GREENWICH);	
    }
  else
    {	
      p.drawPixmap(0,0,
                   map,
                   GREENWICH-gmt_position, 0,
                   map.width()+gmt_position-GREENWICH);	
      p.drawPixmap(map.width()+gmt_position-GREENWICH, 0,
                   map, 
                   0, 0,
                   GREENWICH-gmt_position); 
    }

  // horizontal line
  p.drawLine(0,map.height()+1,width(),map.height()+1);
  // major lines
  for (int c=0; c<6; c++)
    p.drawLine(c*(map.width()-1)/4, map.height()+1, c*(map.width()-1)/4, map.height()+9);
  // minor lines
  for (int c=0; c<25; c++)
    p.drawLine(c*(map.width()-1)/24, map.height()+1, c*(map.width()-1)/24, map.height()+5);

  // text
  p.drawText(0,map.height()+10,width(),height(),AlignHCenter,"12:00");
  p.drawText(0,map.height()+10,width()/2,height(),AlignHCenter,"06:00");
  p.drawText(width()/2,map.height()+10,width()/2,height(),AlignHCenter,"18:00");
  p.drawText(0,map.height()+10,width(),height(),AlignLeft,"00:00");
  p.drawText(0,map.height()+10,width(),height(),AlignRight,"24:00");

  p.end();
}




void World::mousePressEvent(QMouseEvent *event)
{
  pos = event->pos();

  if ( (event->button() == RightButton) && (event->pos().y() <= map.height()))
    menu->popup(mapToGlobal(pos));
}


QString &World::getTip(const QPoint &pos)
{
  char buffer[20];

  if (pos.y() < map.height())
    { 
      time_t time=0;
      
      if (pos.x()>0)
       time = pos.x()*24*60*60/map.width();
      if (pos.x()>=map.width())
	time = 24*60*60;

      strftime(buffer, 20, "%H:%M", gmtime(&time));
      tipText = buffer;      

      int nearest; 
      double dist;
    
      nearest = nearestFlag(pos, dist);
  
      if (nearest>=0 && (unsigned int)nearest<flags.count() && dist<20.0)
        if (!flags.at(nearest)->note.isEmpty())
          {
            tipText += ", ";
            tipText += flags.at(nearest)->note;
          }
    }
  else
    tipText = ""; 
 
  return tipText;
}


void World::addRedFlag()
{
  appendFlag(pos.x(), pos.y()-15,0);
  repaint(FALSE);;
}


void World::addGreenFlag()
{
  appendFlag(pos.x(), pos.y()-15,1);
  repaint(FALSE);;
}


void World::addBlueFlag()
{
  appendFlag(pos.x(), pos.y()-15,2);
  repaint(FALSE);;
}


void World::addYellowFlag()
{
  appendFlag(pos.x(), pos.y()-15,3);
  repaint(FALSE);;
}


class AboutBox : public QDialog
{
public:
  
  AboutBox();

protected:

  void paintEvent(QPaintEvent *event);

private:

  QPixmap     back;
  QPushButton button;
};


AboutBox::AboutBox()
  : QDialog(NULL, "about", TRUE),
    button(klocale->translate("Close"), this)
{
  KIconLoader iconLoader;
  QPixmap back(iconLoader.loadIcon("world2.gif"));

  setCaption(klocale->translate("About KDE World Watch"));
  setFixedSize(back.width(), back.height());
  setBackgroundPixmap(back);

  connect(&button, SIGNAL(clicked()), this, SLOT(accept()));
  
  button.adjustSize();
  button.move((width()-button.width())/2, height()-3*button.height()/2);
}


void AboutBox::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setFont(QFont("Courier", 14, QFont::Bold));
  p.setBackgroundMode(TransparentMode);
  p.drawText(0,0,width(),3*height()/4, AlignCenter, 
    klocale->translate("The KDE World Wide Watch\n\nwritten by Matthias Hölzer\n(hoelzer@physik.uni-wuerzburg.de)"));
}


void World::about()
{
  AboutBox *box = new AboutBox();

  box->exec();

  delete box;
}


void World::loadFlags()
{
  KConfig *config = kapp->getConfig();
  QString key;
  int count;
  Flag *flag;

  flags.setAutoDelete(TRUE);
  flags.clear();

  config->setGroup("General");
  count = config->readNumEntry("Number", -1);

  for (int i=0; i<count; i++)
    {
      flag = new Flag();

      key.sprintf("Flag %i", i);
      config->setGroup(key);
      flag->x = config->readNumEntry("x", 0);
      flag->y = config->readNumEntry("y", 0);
      flag->color = config->readNumEntry("flag", 0);
      flag->note = config->readEntry("note", "");
      flags.append(flag);
    }

}


void World::drawFlags()
{
  QPainter p(&map);

  for (Flag *flag=flags.first(); flag != NULL; flag=flags.next())
  {
    switch(flag->color)
    {
      case 0: p.drawPixmap(flag->x, flag->y, redFlag); break;
      case 1: p.drawPixmap(flag->x, flag->y, greenFlag); break;
      case 2: p.drawPixmap(flag->x, flag->y, blueFlag); break;
      case 3: p.drawPixmap(flag->x, flag->y, yellowFlag); break;
    }
  }
}


void World::appendFlag(int x, int y, int c)
{
  Flag *flag = new Flag();
  
  x = x+GREENWICH-gmt_position-5;
  if (x<0)
    x += map.width();
  if (x>map.width())
    x -= map.width();

  flag->x = x;
  flag->y = y;
  flag->color = c;

  flags.append(flag);

  saveFlags();
}


void World::saveFlags()
{
  KConfig *config = kapp->getConfig();
  QString key;

  config->setGroup("General");
  config->writeEntry("Number", flags.count());

  for (unsigned int i=0; i<flags.count(); i++)
    {
      key.sprintf("Flag %i", i);
      config->setGroup(key);
      config->writeEntry("x", flags.at(i)->x);
      config->writeEntry("y", flags.at(i)->y);
      config->writeEntry("flag", flags.at(i)->color);
      config->writeEntry("note", flags.at(i)->note);
    }
}


double World::flagDistance(unsigned int i, const QPoint &pos)
{
  if (i>=flags.count())
    return 1e10;
  
  int x = flags.at(i)->x;
  int y = flags.at(i)->y;  

  x = x + gmt_position - GREENWICH;
  if (x<0)
    x += map.width();
  if (x>map.width())
    x -= map.width();

  x -= pos.x();
  y -= pos.y();
 
  return sqrt(x*x+y*y);
}


int World::nearestFlag(const QPoint &pos, double &dist)
{
  int nearest = -1;
  double distance = 1e9;

  for (unsigned int i=0; i<flags.count(); i++)
    if (flagDistance(i, pos) < distance)
      {
        distance = flagDistance(i, pos);
        nearest = i;
      }

  dist = distance;
  return nearest;
}


void World::deleteFlag()
{
  double dist;
  int nearest = nearestFlag(pos, dist);

  if (nearest >= 0 && (unsigned int)nearest<flags.count())
    flags.remove(nearest);
  repaint(FALSE);;

  saveFlags();
}


void World::annotateFlag()
{
  int i; 
  double dist;
 
  i = nearestFlag(pos, dist);
  
  if (i>=0 && (unsigned int)i<flags.count())
    {
      KInputLine il(this, klocale->translate("Annotation"), flags.at(i)->note);
      il.setCaption(klocale->translate("Annotate the nearest flag"));

      if (il.exec() == QDialog::Accepted)
        flags.at(i)->note = il.getText();
    }

  saveFlags();
}


void World::updateMap()
{
  time_t t;
  struct tm *tmp;
  double jt, sunra, sundec, sunrv, sunlong;
  short *wtab;  

  if (illumination)
    {
      map = darkMap;
 
      // calculate the position of the sun

      t = time(NULL) + offset;
      tmp = gmtime(&t);
      jt = jtime(tmp);
      sunpos(jt,FALSE, &sunra, &sundec, &sunrv, &sunlong);

      // calculate the illuminated area

      wtab = new short[map.height()];
      projillum(wtab,map.width(),map.height(),sundec);

      // draw illumination

      illuMask->fill(black);
      QPainter p;
      p.begin(illuMask);
     
      int start, stop;
      int middle = map.width()/2 + GREENWICH - gmt_position;
      for (int y=0; y<map.height(); y++)
        if (wtab[y]>0)
          {
            start = middle - wtab[y];
            stop = middle + wtab[y];
            if (start < 0)
              {
                p.drawLine(0,y,stop,y);
                p.drawLine(map.width()+start,y,map.width(),y);
              }
            else
              if (stop > map.width())
                {
                  p.drawLine(start,y,map.width(),y);
                  p.drawLine(0,y,stop-map.width(),y);
                }
              else
                p.drawLine(start,y,stop,y);
          }
      p.end();
      
      QPainter mp(&map);
     
      cleanMap.setMask(*illuMask);
      mp.drawPixmap(0,0,cleanMap);

      // deallocate width table
      
      delete [] wtab;
    }
  else
    {
      cleanMap.setMask(NULL);
      map = cleanMap;
    }

  // draw flags

  drawFlags();
}


void World::illuminateMap()
{
  illumination = !illumination;
  repaint(FALSE);

  KConfig *config = kapp->getConfig();
  config->setGroup("General");
  config->writeEntry("Illumination", illumination);
}


void World::toggleSimulation()
{
  if (offset == 0)
    offset = 1;
  else
    offset = 0;
}
