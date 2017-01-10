#ifndef __DOCKED__H__
#define __DOCKED__H__

#include <qwidget.h>
#include <qpixmap.h>
#include <stdio.h>
#include <kprocess.h>
#include <qpopmenu.h>
#include <kquickhelp.h>
#include <ktmainwindow.h>

#define ICONWIDTH 24
#define ICONHEIGHT 24

class Sysload : public KTMainWindow {
  Q_OBJECT
public:
  Sysload();
  virtual void mousePressEvent(QMouseEvent *);

private slots:
  void doUpdate();
  void updateCPULoad();
  void menuCallback(int id);

  void showCPULoad();
  void showSysLoad();

private:
  int idx;
  KProcess proc;
  float history[ICONWIDTH];
  float cpuload;
  int idle;
  QPopupMenu *menu, *options;
  KConfig *conf;  
  KQuickHelpWindow *qh;
  float av1, av2, av3;

  void clearHistory();
  bool sysload(float *av1, float *av2, float *av3);
  void toggleKPM();
  void popupMenu(QPoint p);
};

#endif
