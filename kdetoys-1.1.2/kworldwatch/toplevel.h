#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_


#include <ktopwidget.h>
#include <kstatusbar.h>

#include "world.h"


const int ID_LOCALTIME = 1;


class TopLevel : public KTopLevelWidget
{
  Q_OBJECT;
  
public:

  TopLevel();
  ~TopLevel();

  QPopupMenu *file, *help;

protected:

  void timerEvent(QTimerEvent *event);

private:

  KStatusBar *statusbar;

  void setupStatusBar();
  
  World *world;

};

#endif
