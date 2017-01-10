#ifndef _WORLD_H_
#define _WORLD_H_


#include <qwidget.h>
#include <qpixmap.h>
#include <qpopmenu.h>
#include <qlist.h>
#include <qtooltip.h>
#include <time.h>


const int GREENWICH = 235;

class TimeTip : public QToolTip
{
public:

  TimeTip(QWidget *parent) : QToolTip(parent) {};
    
protected:

  void maybeTip(const QPoint &pos);	
  
};


class Flag 
{
public:

  int x, y;
  int color;
  QString note;
    
};


class World : public QWidget
{
  Q_OBJECT

public:

  World(QWidget *parent);
  ~World();
  
  void setTime();

  QString &getTip(const QPoint &pos);

  static time_t offset;
    
protected:

  virtual void paintEvent(QPaintEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);

  void updateMap();

  void loadFlags();  
  void drawFlags();
  void appendFlag(int x, int y, int c);
  void saveFlags();

  double flagDistance(unsigned int i, const QPoint &pos);
  int nearestFlag(const QPoint &pos, double &dist);
    
private:

  int gmt_position;

  QPixmap    map, cleanMap, darkMap;
  QBitmap    *illuMask;
  QPixmap    redFlag, blueFlag, greenFlag, yellowFlag;
  QPopupMenu *menu;
  QPoint     pos;
  QToolTip   *tip;
  QString    tipText;

  bool illumination;
        
  QList<Flag> flags;
      
private slots:

  void addRedFlag();
  void addGreenFlag();
  void addBlueFlag();
  void addYellowFlag();
  void deleteFlag();
  void about();
  void annotateFlag();
  void illuminateMap();
  void toggleSimulation();
        
};

#endif
