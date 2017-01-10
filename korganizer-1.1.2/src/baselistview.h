#ifndef _BASE_LIST_VIEW_H
#define _BASE_LIST_VIEW_H

#include <ktablistbox.h>
#include <kapp.h>
#include <qlist.h>
#include <qpopmenu.h>
#include <drag.h>

#include "qdatelist.h"
#include "calobject.h"
#include "kdpevent.h"

class BaseListView : public KTabListBox
{
  Q_OBJECT

public:
  BaseListView(CalObject *calendar, QWidget *parent=0, const char *name=0);
  virtual ~BaseListView();

  KDPEvent *getSelected();
  enum { EVENTADDED, EVENTEDITED, EVENTDELETED };

public slots:
  virtual void updateView();
 /** used to re-read configuration when it has been modified by the
  *  options dialog. */
  virtual void updateConfig();
  void selectEvents(QList<KDPEvent>);
  virtual void changeEventDisplay(KDPEvent *, int);
  int getSelectedId();
  void showDates(bool show);
  inline void showDates();
  inline void hideDates();
  inline void setEventList(QList<KDPEvent> el) { currEvents = el; updateView(); };

protected slots:
  void editEvent();
  void deleteEvent();

signals:
  void editEventSignal(KDPEvent *);
  void deleteEventSignal(KDPEvent *);
  void addEventSignal(KDPEvent *);

protected:
  /* overloaded for drag support */
  bool prepareForDrag(int, int, char **, int *, int *);

  KConfig *config;
  QStrList makeEntries();      // make a list of rows from currEvents.
  virtual QString makeDisplayStr(KDPEvent *anEvent);
  CalObject *calendar;          // pointer to calendar object
  QList<int> currIds; // ID list for currEvents.
  QList<KDPEvent> currEvents;  // list of current events
  QList<KDPEvent> selectedEvents;
  QStrList entries;            // list of entries for current events
  QList<QRect> toolTipList;     // list of tooltip locations.
  bool timeAmPm;
};

#endif
