#ifndef _ALARMDAEMON_H
#define _ALARMDAEMON_H
/* 	$Id: alarmdaemon.h,v 1.8.2.1 1999/04/01 20:29:45 pbrown Exp $	 */

#include <qobject.h>
#include <qlist.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

#include "calobject.h"
#include "kdpevent.h"

class DockWidget : public QWidget {
  Q_OBJECT
public:
  DockWidget(const char *name = 0);
  ~DockWidget();
  void dock();
  void undock();
  const bool isDocked() { return docked; }
  const bool alarmsOn() { return popupMenu->isItemChecked(itemId); }
  void paintEvent(QPaintEvent */*e*/) { paintIcon(); }
  void paintIcon() { bitBlt(this, 0, 0, 
			    (alarmsOn() ? &dPixmap1 : &dPixmap2)); }
  void mousePressEvent(QMouseEvent *e);

public slots:
  void toggleAlarmsEnabled() 
    {
      popupMenu->setItemChecked(itemId,!popupMenu->isItemChecked(itemId));
      paintIcon();
    }
 
 protected:
  QPixmap dPixmap1, dPixmap2;
  QPopupMenu *popupMenu;
  bool docked;
  int itemId;
};


class AlarmDaemon : public QObject {
  Q_OBJECT

public:
  AlarmDaemon(const char *fn, QObject *parent = 0, const char *name = 0);
  virtual ~AlarmDaemon();
  void dock() { docker->dock(); }

public slots:
  void showAlarms(QList<KDPEvent> &alarmEvents);
  void reloadCal();

signals:
  void alarmSignal(QList<KDPEvent> &);
  
private:
  DockWidget *docker;
  CalObject *calendar;
  const char *fileName;
  QString newFileName;
};

//class AlarmDialog : public QDialog {
//public:
//private:
//}
#endif
