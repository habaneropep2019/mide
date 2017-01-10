/* 	$Id: kmonthview.h,v 1.22 1999/01/14 20:41:40 pbrown Exp $	 */

#ifndef _KMONTHVIEW_H 
#define _KMONTHVIEW_H

#include <qlabel.h>
#include <qframe.h>
#include <qdatetm.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qintdict.h>

#include <kapp.h>

#include "qdatelist.h"
#include "calobject.h"
#include "kdpevent.h"
#include "kpbutton.h"
#include "ksellabel.h" 

class EventListBoxItem: public QListBoxItem
{
 public:
  EventListBoxItem(const char *s);
  void setText(const char *s)
    { QListBoxItem::setText(s); }
  void setRecur(bool on) 
    { recur = on; }
  void setAlarm(bool on)
    { alarm = on; }
  const QPalette &palette () const { return myPalette; };
  void setPalette(const QPalette &p) { myPalette = p; };

 protected:
  virtual void paint(QPainter *);
  virtual int height(const QListBox *) const;
  virtual int width(const QListBox *) const;
 private:
  bool    recur;
  bool    alarm;
  QPixmap alarmPxmp;
  QPixmap recurPxmp;
  QPalette myPalette;
};

class KNoScrollListBox: public QListBox {
  Q_OBJECT
 public:
  KNoScrollListBox(QWidget *parent=0, const char *name=0)
    :QListBox(parent, name)
    {
      clearTableFlags();
      setTableFlags(Tbl_clipCellPainting | Tbl_cutCellsV | Tbl_snapToVGrid |
		    Tbl_scrollLastHCell| Tbl_smoothHScrolling);
    }
  ~KNoScrollListBox() {}

 signals:
  void shiftDown();
  void shiftUp();
  void rightClick();

 protected slots:
  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);
  void mousePressEvent(QMouseEvent *);
};

class KSummaries: public KNoScrollListBox {
  Q_OBJECT
 public:
  KSummaries(QWidget    *parent=0, 
	     CalObject  *calendar = 0, 
	     QDate       qd       = QDate::currentDate(),
	     int         index    = 0,
	     const char *name=0);
  ~KSummaries() {}
  QDate getDate() { return(myDate); }
  void setDate(QDate);
  void calUpdated();
  void setAmPm(bool ampm) { timeAmPm = ampm; };
  KDPEvent *getSelected() { return currIdxs->find(itemIndex); };

 signals:
  void daySelected(int index);
  void editEventSignal(KDPEvent *);

 protected slots:
  void itemHighlighted(int);
  void itemSelected(int);

 private:
   QDate               myDate;
   int                 idx, itemIndex;
   CalObject          *myCal;
   QList<KDPEvent>    events;
   QIntDict<KDPEvent> *currIdxs; 
   bool                timeAmPm;
};

class KDPMonthView: public QFrame {
   Q_OBJECT
 public:
   KDPMonthView(CalObject *calendar,
		QWidget    *parent   = 0, 
		QDate       qd       = QDate::currentDate(),
		const char *name     = 0);
   ~KDPMonthView();

   enum { EVENTADDED, EVENTEDITED, EVENTDELETED };

   /** returns the currently selected event */
   KDPEvent *getSelected();
 public slots:
   void updateConfig();
   void selectDates(const QDateList);
   void changeEventDisplay(KDPEvent *, int);

 signals:
   void newEventSignal(QDate);
   void editEventSignal(KDPEvent *);
   void deleteEventSignal(KDPEvent *);
   void datesSelected(const QDateList);

 protected slots:
   void updateView();
   void resizeEvent(QResizeEvent *);
   void goBackYear();
   void goForwardYear();
   void goBackMonth();
   void goForwardMonth();
   void goBackWeek();
   void goForwardWeek();
   void daySelected(int index);
   void newEventSlot(int index);
   void doRightClickMenu();
   void newEventSelected() { emit newEventSignal(daySummaries[*selDateIdxs.first()]->getDate()); };
   void editSelected() { emit editEventSignal(getSelected()); };
   void deleteSelected() { emit deleteEventSignal(getSelected()); };

 protected:
   void viewChanged();
   
 private:
   // date range label.
   QLabel           *dispLabel;
   // day display vars
   QFrame           *vFrame;
   QGridLayout      *dayLayout;
   QLabel           *dayNames[7];
   KSelLabel        *dayHeaders[42];
   KSummaries       *daySummaries[42];
   bool              shortdaynames;
   bool              weekStartsMonday;

   // display control vars
   QFrame           *cFrame;
   QVBoxLayout      *cLayout;
   KPButton         *upYear;
   KPButton         *upMonth;
   KPButton         *upWeek;
   KPButton         *downWeek;
   KPButton         *downMonth;
   KPButton         *downYear;
   QPopupMenu       *rightClickMenu;

   // state data.
   QDate             myDate;
   QDateList         selDates;
   CalObject        *myCal;
   QList<int>        selDateIdxs;
   QPalette          holidayPalette;
};

#endif
 
