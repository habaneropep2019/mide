/* 	$Id: calobject.h,v 1.46 1998/12/19 20:25:01 pbrown Exp $	 */

#ifndef _CALOBJECT_H
#define _CALOBJECT_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetm.h>
#include <qintdict.h>
#include <qdict.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qregexp.h>

#include "vcc.h"
#include "vobject.h"
#include "kdpevent.h"

#define _PRODUCT_ID "-//K Desktop Environment//NONSGML KOrganizer//EN"
#define _TIME_ZONE "-0500" /* hardcoded, overridden in config file. */
#define _VCAL_VERSION "1.0"
#define BIGPRIME 1031 /* should allow for at least 4 appointments 365 days/yr
			 to be almost instantly fast. */

class VCalDrag;

/**
  * This is the main "calendar" object class for KOrganizer.  It holds
  * information like all appointments/events, user information, etc. etc.
  * one calendar is associated with each TopWidget (@see topwidget.h).
  *
  * @short class providing in interface to a calendar
  * @author Preston Brown
  * @version $Revision: 1.46 $
  */
class CalObject : public QObject {
  Q_OBJECT

public:
  /** constructs a new calendar, with variables initialized to sane values. */
  CalObject();
  virtual ~CalObject();

  /** loads a calendar on disk in vCalendar format into the current calendar.
   * any information already present is lost. Returns TRUE if successful,
   * else returns FALSE.
   * @param fileName the name of the calendar on disk.
   */
  bool load(const QString &fileName);
  /** writes out the calendar to disk in vCalendar format. Returns nonzero
   * on save error.
   * @param fileName the name of the file
   */
  int save(const QString &fileName);
  /** clears out the current calendar, freeing all used memory etc. etc. */
  void close();

  /** create an object to be used with the Xdnd Drag And Drop protocol. */
  VCalDrag *createDrag(KDPEvent *selectedEv, QWidget *owner);
  /** cut, copy, and paste operations follow. */
  void cutEvent(KDPEvent *);
  bool copyEvent(KDPEvent *);
  /** pastes the event and returns a pointer to the new event pasted. */
  KDPEvent *pasteEvent(const QDate *, const QTime *newTime = 0L,
		       VObject *vc=0L);

  /** set the owner of the calendar.  Should be owner's full name. */
  const QString &getOwner() const;
  /** return the owner of the calendar's full name. */
  void setOwner(const QString &os);
  /** set the email address of the calendar owner. */
  inline const QString &getEmail() { return emailString; };
  /** return the email address of the calendar owner. */
  inline void setEmail(const QString &es) { emailString = es; };
  /** adds a KDPEvent to this calendar object.
   * @param anEvent a pointer to the event to add
   */

  /** query whether autoSave is set or not */
  bool autoSave() { return aSave; };

  /** methods to get/set the local time zone */
  void setTimeZone(const char *tz);
  inline void setTimeZone(int tz) { timeZone = tz; };
  int getTimeZone() const { return timeZone; };
  /* compute an ISO 8601 format string from the time zone. */
  QString getTimeZoneStr() const;

  void addEvent(KDPEvent *anEvent);
  /** deletes an event from this calendar. We could just use
   * a unique ID to search for the event, but using the date too is faster.
   * @param date the date upon which the event occurs.  
   * @param eventId the unique ID attached to the event
   */
  void deleteEvent(const QDate &date, int eventId);
  void deleteEvent(KDPEvent *);
  /** retrieves an event from the calendar, based on a date and an evenId.
   * faster than specifying an eventId alone. 
   */
  KDPEvent *getEvent(const QDate &date, int eventId);
  /** retrieves an event from the calendar on the basis of ID alone. */
  KDPEvent *getEvent(int eventId);
  /** retrieves an event on the basis of the unique string ID. */
  KDPEvent *getEvent(const QString &UniqueStr);
  /** builds and then returns a list of all events that match for the
   * date specified. useful for dayView, etc. etc. */
  QList<KDPEvent> getEventsForDate(const QDate &date, bool sorted = FALSE);
  QList<KDPEvent> getEventsForDate(const QDateTime &qdt);

  /*
   * returns a QString with the text of the holiday (if any) that falls
   * on the specified date.
   */
  QString getHolidayForDate(const QDate &qd);
  
  /** returns the number of events that are present on the specified date. */
  int numEvents(const QDate &qd);

  /** add a todo to the todolist. */
  void addTodo(KDPEvent *todo);
  /** remove a todo from the todolist. */
  void deleteTodo(KDPEvent *);
  const QList<KDPEvent> &getTodoList() const;
  /** searches todolist for an event with this id, returns pointer or null. */
  KDPEvent *getTodo(int id);
  /** searches todolist for an event with this unique string identifier,
    returns a pointer or null. */
  KDPEvent *getTodo(const QString &UniqueStr);

  /** traversal methods */
  KDPEvent *first();
  KDPEvent *last();
  KDPEvent *next();
  KDPEvent *prev();
  KDPEvent *current();

  /** translates a VObject of the TODO type into a KDPEvent */
  KDPEvent *VTodoToEvent(VObject *vtodo);
  /** translates a VObject into a KDPEvent and returns a pointer to it. */
  KDPEvent *VEventToEvent(VObject *vevent);
  /** translate a KDPEvent into a VTodo-type VObject and return pointer */
  VObject *eventToVTodo(const KDPEvent *anEvent);
  /** translate a KDPEvent into a VObject and returns a pointer to it. */
  VObject* eventToVEvent(const KDPEvent *anEvent);
  QList<KDPEvent> search(const QRegExp &) const;

  void showDialogs(bool d) { dialogsOn = d; };

signals:
  /** emitted at regular intervals to indicate that the events in the
    list have triggered an alarm. */
  void alarmSignal(QList<KDPEvent> &);
  /** emitted whenever an event in the calendar changes.  Emits a pointer
    to the changed event. */
  void calUpdated(KDPEvent *);

public slots:
  /** checks to see if any alarms are pending, and if so, returns a list
   * of those events that have alarms. */
  void checkAlarms();
  void updateConfig();
 
protected slots:
  /** this method should be called whenever a KDPEvent is modified directly
   * via it's pointer.  It makes sure that the calObject is internally
   * consistent. */
  void updateEvent(KDPEvent *anEvent);

protected:
  /* methods */

  /** inserts an event into its "proper place" in the calendar. */
  void insertEvent(const KDPEvent *anEvent);

  /** on the basis of a QDateTime, forms a hash key for the dictionary. */
  long int makeKey(const QDateTime &dt);
  /** on the basis of a QDate, forms a hash key for the dictionary */
  long int makeKey(const QDate &d);

  /** update internal position information */
  void updateCursors(KDPEvent *dEvent);

  /** takes a QDate and returns a string in the format YYYYMMDDTHHMMSS */
  QString qDateToISO(const QDate &);
  /** takes a QDateTime and returns a string in format YYYYMMDDTHHMMSS */
  QString qDateTimeToISO(const QDateTime &, bool zulu=TRUE);
  /** takes a string in the format YYYYMMDDTHHMMSS and returns a 
   * valid QDateTime. */
  QDateTime ISOToQDateTime(const char *dtStr);
  /** takes a vCalendar tree of VObjects, and puts all of them that have
   * the "event" property into the dictionary, todos in the todo-list, etc. */
  void populate(VObject *vcal);

  /** takes a number 0 - 6 and returns the two letter string of that day,
    * i.e. MO, TU, WE, etc. */
  const char *dayFromNum(int day);
  /** the reverse of the above function. */
  int numFromDay(const QString &day);

  /** shows an error dialog box. */
  void loadError(const QString &fileName);
  /** shows an error dialog box. */
  void parseError(const char *prop);

  /* variables */
  QString ownerString;                    // who the calendar belongs to
  QString emailString;                    // email address of the owner
  int timeZone;                           // timezone OFFSET from GMT (MINUTES)
  QIntDict<QList<KDPEvent> > *calDict;    // dictionary of lists of events.
  QList<KDPEvent> recursList;             // list of repeating events.

  QList<KDPEvent> todoList;               // list of "todo" items.

  QDate *oldestDate;
  QDate *newestDate;
  QDate cursorDate;                       // last date we were looking at.
  QListIterator<KDPEvent> *cursor;        // for linear traversal methods
  QListIterator<KDPEvent> recursCursor;   // for linear traversal methods

  bool aSave;                             // prompt for saving, or autosave?
  bool dialogsOn;                         // display various GUI dialogs?
};

#ifdef __cplusplus
extern "C" {
#endif
char *parse_holidays(char *, int year, short force);
struct holiday {
  char            *string;        /* name of holiday, 0=not a holiday */
  unsigned short  dup;            /* reference count */
};   
extern struct holiday holiday[366];
#if __cplusplus
};
#endif

#endif
