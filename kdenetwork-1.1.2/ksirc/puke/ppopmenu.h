#ifndef PPOPMENU_H
#define PPOPMENU_H

class PPopupMenu;

#include "qpopmenu.h"
#include "pobject.h"
#include "pmessage.h"

#include "ptablevw.h"
#include "pmenudta.h"

extern "C" {
  // Init and setup code
  PObject *createWidget(CreateArgs &ca);
}

class PPopupMenu : public PTableView
{
  Q_OBJECT
public:
  PPopupMenu(PObject *child);
  virtual ~PPopupMenu();

  virtual void messageHandler(int fd, PukeMessage *pm);

  virtual void setWidget(QObject *_menu);
  virtual QPopupMenu *widget();


signals:
  void outputMessage(int fd, PukeMessage *pm);
  void widgetDestroyed(widgetId wI);

protected slots:
  void got_activated ( int itemId );

private:
  QPopupMenu *menu;
  PMenuData *pmd;

};

#endif
