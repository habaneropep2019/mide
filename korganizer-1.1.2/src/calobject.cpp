// Calendar class for KOrganizer
// (c) 1998 Preston Brown
// 	$Id: calobject.cpp,v 1.141.2.5 1999/04/19 03:44:34 pbrown Exp $

#include "config.h"

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <stdlib.h>

#include <qdatetm.h>
#include <qstring.h>
#include <qlist.h>
#include <stdlib.h>
#include <qmsgbox.h>
#include <qregexp.h>
#include <kiconloader.h>
#include <kapp.h>
#include <qclipbrd.h>
#include <qdialog.h>
#include <qmsgbox.h>

#include "vcaldrag.h"
#include "qdatelist.h"
#include "calobject.h"
#include "calobject.moc"

CalObject::CalObject() : QObject(), recursCursor(recursList)
{
  struct passwd *pwent;
  uid_t userId;
  QString tmpStr;

  // initialization
  oldestDate = 0L;
  newestDate = 0L;
  cursor = 0L;
  aSave = FALSE;
  dialogsOn = TRUE;

  // initialize random numbers.  This is a hack, and not
  // even that good of one at that.
  srandom(time(0L));

  tmpStr = KApplication::localconfigdir();
  tmpStr += "/korganizerrc";
  KConfig *config = new KConfig(tmpStr.data());
  
  // user information...
  userId = getuid();
  pwent = getpwuid(userId);
  config->setGroup("Personal Settings");
  tmpStr = config->readEntry("user_name");
  if (tmpStr.isEmpty()) {
    
    if (strlen(pwent->pw_gecos) > 0)
      setOwner(pwent->pw_gecos);
    else
      setOwner(pwent->pw_name);
    config->writeEntry("user_name",getOwner().data());
  } else {
    setOwner(tmpStr);
  }

  tmpStr = config->readEntry("user_email");
  if (tmpStr.isEmpty()) {
    char cbuf[80];
    tmpStr = pwent->pw_name;
    tmpStr += "@";

#ifdef HAVE_GETHOSTNAME
    if (gethostname(cbuf, 79)) {
      // error getting hostname
      tmpStr += "localhost";
    } else {
      hostent he;
      if (gethostbyname(cbuf)) {
	  he = *gethostbyname(cbuf);
	  tmpStr += he.h_name;
      } else {
	// error getting hostname
	tmpStr += "localhost";
      }
    }
#else
    tmpStr += "localhost";
#endif
    setEmail(tmpStr);
    config->writeEntry("user_email",tmpStr.data());
  } else {
    setEmail(tmpStr);
  }

  config->setGroup("Time & Date");

  tmpStr = config->readEntry("Time Zone");
  int dstSetting = config->readNumEntry("Daylight Savings", 0);
  extern long int timezone;
  struct tm *now;
  time_t curtime;
  curtime = time(0);
  now = localtime(&curtime);
  int hourOff = - ((timezone / 60) / 60);
  if (now->tm_isdst)
    hourOff += 1;
  QString tzStr;
  tzStr.sprintf("%.2d%.2d",
		hourOff, 
		abs((timezone / 60) % 60));

  // if no time zone was in the config file, write what we just discovered.
  if (tmpStr.isEmpty()) {
    config->writeEntry("Time Zone", tzStr.data());
  }
  
  // if daylight savings has changed since last load time, we need
  // to rewrite these settings to the config file.
  if ((now->tm_isdst && !dstSetting) ||
      (!now->tm_isdst && dstSetting)) {
    config->writeEntry("Time Zone", tzStr.data());
    config->writeEntry("Daylight Savings", now->tm_isdst);
  }
  
  setTimeZone(tzStr.data());

  config->setGroup("General");
  aSave = config->readBoolEntry("Auto Save", FALSE);
    
  // done with config object
  delete config;

  recursList.setAutoDelete(TRUE);
  // solves the leak?
  todoList.setAutoDelete(TRUE);

  calDict = new QIntDict<QList<KDPEvent> > (BIGPRIME);
  calDict->setAutoDelete(TRUE);
}

CalObject::~CalObject() 
{
  close();
  delete calDict;
  if (cursor)
    delete cursor;
  if (newestDate)
    delete newestDate;
  if (oldestDate)
    delete oldestDate;
}

bool CalObject::load(const QString &fileName)
{

  VObject *vcal = 0L;
  char *fn = fileName.data();

  // do we want to silently accept this, or make some noise?  Dunno...
  // it is a semantical thing vs. a practical thing.
  if (fileName.isEmpty())
    return TRUE;

  // this is not necessarily only 1 vcal.  Could be many vcals, or include
  // a vcard...
  vcal = Parse_MIME_FromFileName(fn);

  if (!vcal) {
    loadError(fileName);
    return FALSE;
  }

  // any other top-level calendar stuff should be added/initialized here

  // put all vobjects into their proper places
  populate(vcal);

  // clean up from vcal API stuff
  cleanVObjects(vcal);
  cleanStrTbl();

  // set cursors to beginning of list.
  first(); 

  return TRUE;
}

void CalObject::close()
{
  QIntDictIterator<QList<KDPEvent> > qdi(*calDict);
  QList<KDPEvent> *tmpList;
  QList<KDPEvent> multiDayEvents;
  KDPEvent *anEvent;
  while (qdi.current()) {
    tmpList = qdi.current();
    ++qdi;
    tmpList->setAutoDelete(TRUE);
    for (anEvent = tmpList->first(); anEvent; anEvent = tmpList->next()) {
      // if the event spans multiple days, we need to put a pointer to it
      // in a list for later deletion, and only remove it's reference.  
      // Otherwise, since the list contains the only pointer to it, we
      // really want to blow it away.
      if (anEvent->isMultiDay()) {
	if (multiDayEvents.findRef(anEvent) == -1)
	  multiDayEvents.append(anEvent);
	tmpList->take();
      } else {
	tmpList->remove();
      }
    }
  }
  multiDayEvents.setAutoDelete(TRUE);
  multiDayEvents.clear();
  calDict->clear();
  recursList.clear();
  todoList.clear();
  
  // reset oldest/newest date markers
  delete oldestDate; oldestDate = 0L;
  delete newestDate; newestDate = 0L;
  if (cursor) {
    delete cursor;
    cursor = 0L;
  } 
}

int CalObject::save(const QString &fileName)
{
  QString tmpStr;
  VObject *vcal, *vo;
  const char *fn = (const char *) fileName;

  vcal = newVObject(VCCalProp);

  //  addPropValue(vcal,VCLocationProp, "0.0");
  addPropValue(vcal,VCProdIdProp, _PRODUCT_ID);
  tmpStr = getTimeZoneStr();
  addPropValue(vcal,VCTimeZoneProp, tmpStr.data());
  addPropValue(vcal,VCVersionProp, _VCAL_VERSION);

  // TODO STUFF
  QListIterator<KDPEvent> qlt(todoList);
  for (; qlt.current(); ++qlt) {
    vo = eventToVTodo(qlt.current());
    addVObjectProp(vcal, vo);
  }


  // EVENT STUFF
  QIntDictIterator<QList<KDPEvent> > dictIt(*calDict);

  while (dictIt.current()) {
    QListIterator<KDPEvent> listIt(*dictIt.current());
    while (listIt.current()) {
      // if the event is multi-day, we only want to save the
      // first instance that is in the dictionary
      if (listIt.current()->isMultiDay()) {
	QList<KDPEvent> *tmpList = calDict->find(makeKey(listIt.current()->getDtStart().date()));
	if (dictIt.current() == tmpList) {
	  vo = eventToVEvent(listIt.current());
	  addVObjectProp(vcal, vo);
	}
      } else {
	vo = eventToVEvent(listIt.current());
	addVObjectProp(vcal, vo);
      }
      ++listIt;
    }
    ++dictIt;
  }

  // put in events that recurs
  QListIterator<KDPEvent> qli(recursList);
  for (; qli.current(); ++qli) {
    vo = eventToVEvent(qli.current());
    addVObjectProp(vcal, vo);
  }

  writeVObjectToFile((char *) fn, vcal);
  cleanVObjects(vcal);
  cleanStrTbl();

  if (QFile::exists(fn))
    return 0;
  else
    return 1; // error
}

VCalDrag *CalObject::createDrag(KDPEvent *selectedEv, QWidget *owner)
{
  VObject *vcal, *vevent;
  QString tmpStr;
  
  vcal = newVObject(VCCalProp);
  
  addPropValue(vcal,VCProdIdProp, _PRODUCT_ID);
  tmpStr = getTimeZoneStr();
  addPropValue(vcal,VCTimeZoneProp, tmpStr.data());
  addPropValue(vcal,VCVersionProp, _VCAL_VERSION);
  
  vevent = eventToVEvent(selectedEv);
  
  addVObjectProp(vcal, vevent);

  VCalDrag *vcd = new VCalDrag(vcal, owner);
  // free memory associated with vCalendar stuff
  cleanVObject(vcal);  
  vcd->setPixmap(Icon("newevent.xpm"));

  return vcd;
}

void CalObject::cutEvent(KDPEvent *selectedEv)
{
  if (copyEvent(selectedEv))
    deleteEvent(selectedEv);
}

bool CalObject::copyEvent(KDPEvent *selectedEv)
{
  QClipboard *cb = QApplication::clipboard();
  VObject *vcal, *vevent;
  QString tmpStr;
  char *buf;

  vcal = newVObject(VCCalProp);

  //  addPropValue(vcal,VCLocationProp, "0.0");
  addPropValue(vcal,VCProdIdProp, _PRODUCT_ID);
  tmpStr = getTimeZoneStr();
  addPropValue(vcal,VCTimeZoneProp, tmpStr.data());
  addPropValue(vcal,VCVersionProp, _VCAL_VERSION);

  vevent = eventToVEvent(selectedEv);

  addVObjectProp(vcal, vevent);

  buf = writeMemVObject(0, 0, vcal);
  
  // free memory associated with vCalendar stuff
  cleanVObject(vcal);
  
  // paste to clipboard, then clear temp. buffer
  cb->setText(buf);
  delete buf;
  buf = 0L;

  return TRUE;
}

KDPEvent *CalObject::pasteEvent(const QDate *newDate, 
				const QTime *newTime, VObject *vc)
{
  VObject *vcal, *curVO, *curVOProp;
  VObjectIterator i;
  int daysOffset;

  KDPEvent *anEvent = 0L;

  if (!vc) {
    QClipboard *cb = QApplication::clipboard();
    int bufsize;
    const char *buf;
    buf = cb->text();
    bufsize = strlen(buf);
    
    if (strncmp("BEGIN:VCALENDAR",buf,
		strlen("BEGIN:VCALENDAR"))) {
      if (dialogsOn)
	QMessageBox::critical(kapp->mainWidget(), i18n("KOrganizer Error"),
			      i18n("An error has occurred parsing the "
				   "contents of the clipboard.\nYou can "
				   "only paste a valid vCalendar into "
				   "KOrganizer.\n"));
      return 0;
    }
    
    vcal = Parse_MIME(buf, bufsize);
    
    if (vcal == 0)
      if ((curVO = isAPropertyOf(vcal, VCCalProp)) == 0) {
	if (dialogsOn)
	  QMessageBox::critical(kapp->mainWidget(), i18n("KOrganizer Error"),
				i18n("An error has occurred parsing the "
				     "contents of the clipboard.\nYou can "
				     "only paste a valid vCalendar into "
				     "KOrganizer.\n"));
	return 0;
      }
  } else {
    vcal = vc;
  }

  initPropIterator(&i, vcal);
  
  // we only take the first object.
  do  {
    curVO = nextVObject(&i);
  } while (strcmp(vObjectName(curVO), VCEventProp));
  
  // now, check to see that the object is BOTH an event, and if so,
  // that it has a starting date
  if (strcmp(vObjectName(curVO), VCEventProp) == 0) {
    if ((curVOProp = isAPropertyOf(curVO, VCDTstartProp)) ||
	(curVOProp = isAPropertyOf(curVO, VCDTendProp))) {
      
      // we found an event with a start time, put it in the dict
      anEvent = VEventToEvent(curVO);
      // if we pasted an event that was the result of a copy in our
      // own calendar, now we have duplicate UID strings.  Need to generate
      // a new one for this new event.
      int hashTime = QTime::currentTime().hour() + 
	QTime::currentTime().minute() + QTime::currentTime().second() +
	QTime::currentTime().msec();
      QString uidStr;
      uidStr.sprintf("KOrganizer - %li.%d",random(),hashTime);
      if (getEvent(anEvent->getVUID()))
	anEvent->setVUID(uidStr.data());

      daysOffset = anEvent->getDtEnd().date().dayOfYear() - 
	anEvent->getDtStart().date().dayOfYear();
      
      if (newTime)
	anEvent->setDtStart(QDateTime(*newDate, *newTime));
      else
	anEvent->setDtStart(QDateTime(*newDate, anEvent->getDtStart().time()));
      
      anEvent->setDtEnd(QDateTime(newDate->addDays(daysOffset),
				  anEvent->getDtEnd().time()));
      addEvent(anEvent);
    } else {
      debug(i18n("found a VEvent with no DTSTART/DTEND! Skipping"));
    }
  } else if (strcmp(vObjectName(curVO), VCTodoProp) == 0) {
    anEvent = VTodoToEvent(curVO);
    addTodo(anEvent);
  } else {
    debug("unknown event type in paste!!!");
  }
  // get rid of temporary VObject
  deleteVObject(vcal);
  updateCursors(anEvent);
  return anEvent;
}

const QString &CalObject::getOwner() const
{
  return ownerString;
}

void CalObject::setOwner(const QString &os)
{
  int i;
  ownerString = os.data(); // to detach it
  i = ownerString.find(',');
  if (i != -1)
    ownerString = ownerString.left(i);
}

void CalObject::setTimeZone(const char *tz)
{
  bool neg = FALSE;
  int hours, minutes;
  QString tmpStr(tz);

  if (tmpStr.left(1) == "-")
    neg = TRUE;
  if (tmpStr.left(1) == "-" || tmpStr.left(1) == "+")
    tmpStr.remove(0, 1);
  hours = tmpStr.left(2).toInt();
  if (tmpStr.length() > 2) 
    minutes = tmpStr.right(2).toInt();
  else
    minutes = 0;
  timeZone = (60*hours+minutes);
  if (neg)
    timeZone = -timeZone;
}

QString CalObject::getTimeZoneStr() const 
{
  QString tmpStr;
  int hours = abs(timeZone / 60);
  int minutes = abs(timeZone % 60);
  bool neg = timeZone < 0;

  tmpStr.sprintf("%c%.2d%.2d",
		 (neg ? '-' : '+'),
		 hours, minutes);
  return tmpStr;
}

// don't ever call this unless a kapp exists!
void CalObject::updateConfig()
{
  bool updateFlag = FALSE;
  KConfig *config = kapp->getConfig();
  config->setGroup("Personal Settings");

  ownerString = config->readEntry("user_name");

  // update events to new organizer (email address) 
  // if it has changed...
  QString configEmail = config->readEntry("user_email");

  if (emailString != configEmail) {
    QString oldEmail = emailString;
    oldEmail.detach();
    emailString = configEmail;
    KDPEvent *firstEvent, *currEvent;
    bool atFirst = TRUE;

    firstEvent = last();
    // gotta skip over the first one, which is same as first. 
    // I know, bad coding.
    for (currEvent = prev(); currEvent; currEvent = prev()) {
      debug("in calobject::updateConfig(), currEvent summary: %s",currEvent->getSummary().data());
      if ((currEvent == firstEvent) && !atFirst) {
	break;
      }
      if (currEvent->getOrganizer() == oldEmail) {
	currEvent->setReadOnly(FALSE);
	currEvent->setOrganizer(emailString); 
        updateFlag = TRUE;
      }
      atFirst = FALSE;
    }
  }

  config->setGroup("Time & Date");
  setTimeZone(config->readEntry("Time Zone").data());

  config->setGroup("General");
  aSave = config->readBoolEntry("Auto Save", FALSE);

  if (updateFlag)
    emit calUpdated((KDPEvent *) 0L);
}

void CalObject::addEvent(KDPEvent *anEvent)
{
  anEvent->isTodo = FALSE;
  insertEvent(anEvent);
  // set event's read/write status
  if (anEvent->getOrganizer() != getEmail())
    anEvent->setReadOnly(TRUE);
  connect(anEvent, SIGNAL(eventUpdated(KDPEvent *)), this,
	  SLOT(updateEvent(KDPEvent *)));
  emit calUpdated(anEvent);
}

void CalObject::deleteEvent(const QDate &date, int eventId)
{
  QList<KDPEvent> *tmpList;
  KDPEvent *anEvent;
  int extraDays, dayOffset;
  QDate startDate, tmpDate;

  tmpList = calDict->find(makeKey(date));
  // if tmpList exists, the event is in the normal dictionary; 
  // it doesn't recur.
  if (tmpList) {
    for (anEvent = tmpList->first(); anEvent;
	 anEvent = tmpList->next()) {
      if (anEvent->getEventId() == eventId) {
	if (!anEvent->isMultiDay()) {
	  tmpList->setAutoDelete(FALSE);
	  tmpList->remove();
	  goto FINISH;
	} else {
	  //debug("deleting multi-day event");
	  // event covers multiple days.
	  startDate = anEvent->getDtStart().date();
	  extraDays = startDate.daysTo(anEvent->getDtEnd().date());
	  for (dayOffset = 0; dayOffset <= extraDays; dayOffset++) {
	    tmpDate = startDate.addDays(dayOffset);
	    tmpList = calDict->find(makeKey(tmpDate));
	    if (tmpList) {
	      for (anEvent = tmpList->first(); anEvent;
		   anEvent = tmpList->next()) {
		if (anEvent->getEventId() == eventId)
		  tmpList->remove();
	      }
	    }
	  }
	  // now we need to free the memory taken up by the event...
	  delete anEvent;
	  goto FINISH;
	}
      }
    }
  }
  for (anEvent = recursList.first(); anEvent;
       anEvent = recursList.next()) {
    if (anEvent->getEventId() == eventId) {
      recursList.remove();
    }
  }


 FINISH:
  // update oldest / newest dates if necessary
  // basically, first we check to see if this was the oldest
  // date in the calendar.  If it is, then we keep adding 1 to
  // the oldest date until we come up with a location in the 
  // QDate dictionary which has some entries.  Now, this might
  // be the oldest date, but we want to check the recurrence list
  // to make sure it has nothing older.  We start looping through
  // it, and each time we find something older, we adjust the oldest
  // date and start the loop again.  If we go through all the entries,
  // we are assured to have the new oldest date.
  //
  // the newest date is analogous, but sort of opposite.
  if (date == (*oldestDate)) {
    for (; !calDict->find(makeKey((*oldestDate))) &&
	   (*oldestDate != *newestDate); 
	 (*oldestDate) = oldestDate->addDays(1));
    recursList.first();
    while ((anEvent = recursList.current())) {
      if (anEvent->getDtStart().date() < (*oldestDate)) {
	(*oldestDate) = anEvent->getDtStart().date();
	recursList.first();
      }
      anEvent = recursList.next();
    }
  }

  if (date == (*newestDate)) {
    for (; !calDict->find(makeKey((*newestDate))) &&
	   (*newestDate != *oldestDate);
	 (*newestDate) = newestDate->addDays(-1));
    recursList.first();
    while ((anEvent = recursList.current())) {
      if (anEvent->getDtStart().date() > (*newestDate)) {
	(*newestDate) = anEvent->getDtStart().date();
	recursList.first();
      }
      anEvent = recursList.next();
    }
  }	  
}

// probably not really efficient, but...it works for now.
void CalObject::deleteEvent(KDPEvent *anEvent)
{
  int id = anEvent->getEventId();
  QDate startDate(anEvent->getDtStart().date());

  deleteEvent(startDate, id);
  emit calUpdated(anEvent);
}

KDPEvent *CalObject::getEvent(const QDate &date, int eventId)
{
  QList<KDPEvent> tmpList(getEventsForDate(date));
  KDPEvent *anEvent = 0;

  for (anEvent = tmpList.first(); anEvent; anEvent = tmpList.next()) {
    if (anEvent->getEventId() == eventId) {
      updateCursors(anEvent);
      return anEvent;
    }
  }
  return (KDPEvent *) 0L;
}

KDPEvent *CalObject::getEvent(int eventId)
{
  QList<KDPEvent>* eventList;
  QIntDictIterator<QList<KDPEvent> > dictIt(*calDict);
  KDPEvent *anEvent;

  while (dictIt.current()) {
    eventList = dictIt.current();
    for (anEvent = eventList->first(); anEvent;
	 anEvent = eventList->next()) {
      if (anEvent->getEventId() == eventId) {
	updateCursors(anEvent);
	return anEvent;
      }
    }
    ++dictIt;
  }
  for (anEvent = recursList.first(); anEvent;
       anEvent = recursList.next()) {
    if (anEvent->getEventId() == eventId) {
      updateCursors(anEvent);
      return anEvent;
    }
  }
  // catch-all.
  return (KDPEvent *) 0L;
}

KDPEvent *CalObject::getEvent(const QString &UniqueStr)
{
  QList<KDPEvent> *eventList;
  QIntDictIterator<QList<KDPEvent> > dictIt(*calDict);
  KDPEvent *anEvent;

  while (dictIt.current()) {
    eventList = dictIt.current();
    for (anEvent = eventList->first(); anEvent;
	 anEvent = eventList->next()) {
      if (anEvent->getVUID() == UniqueStr) {
	updateCursors(anEvent);
	return anEvent;
      }
    }
    ++dictIt;
  }
  for (anEvent = recursList.first(); anEvent;
       anEvent = recursList.next()) {
    if (anEvent->getVUID() == UniqueStr) {
      updateCursors(anEvent);
      return anEvent;
    }
  }
  // catch-all.
  return (KDPEvent *) 0L;
}

void CalObject::addTodo(KDPEvent *todo)
{
  todo->isTodo = TRUE;
  todoList.append(todo);
  connect(todo, SIGNAL(eventUpdated(KDPEvent *)), this,
	  SLOT(updateEvent(KDPEvent *)));
  emit calUpdated(todo);
}

void CalObject::deleteTodo(KDPEvent *todo)
{
  todoList.findRef(todo);
  todoList.remove();
  emit calUpdated(todo);
}


const QList<KDPEvent> &CalObject::getTodoList() const
{
  return todoList;
}

KDPEvent *CalObject::getTodo(int id)
{
  KDPEvent *aTodo;
  for (aTodo = todoList.first(); aTodo;
       aTodo = todoList.next())
    if (id == aTodo->getEventId())
      return aTodo;
  // item not found
  return (KDPEvent *) 0L;
}

KDPEvent *CalObject::getTodo(const QString &UniqueStr)
{
  KDPEvent *aTodo;
  for (aTodo = todoList.first(); aTodo;
       aTodo = todoList.next())
    if (aTodo->getVUID() == UniqueStr)
      return aTodo;
  // not found
  return (KDPEvent *) 0L;
}

KDPEvent *CalObject::first()
{
  if (!oldestDate || !newestDate)
    return (KDPEvent *) 0L;
  
  QList<KDPEvent> *tmpList;
  
  if (calDict->isEmpty() && recursList.isEmpty())
    return (KDPEvent *) 0L;
  
  
  if (cursor) {
    delete cursor;
    cursor = 0L;
  }
  
  recursCursor.toFirst();
  if ((tmpList = calDict->find(makeKey((*oldestDate))))) {
    cursor = new QListIterator<KDPEvent> (*tmpList);
    cursorDate = cursor->current()->getDtStart().date();
    return cursor->current();
  } else {
    recursCursor.toFirst();
    while (recursCursor.current() &&
	   recursCursor.current()->getDtStart().date() != (*oldestDate))
      ++recursCursor;
    cursorDate = recursCursor.current()->getDtStart().date();
    return recursCursor.current();
  }
}

KDPEvent *CalObject::last()
{
  if (!oldestDate || !newestDate)
    return (KDPEvent *) 0L;

  QList<KDPEvent> *tmpList;
  
  if (calDict->isEmpty() && recursList.isEmpty())
    return (KDPEvent *) 0L;
  
  if (cursor) {
    delete cursor;
    cursor = 0L;
  }
  
  recursCursor.toLast();
  if ((tmpList = calDict->find(makeKey((*newestDate))))) {
    cursor = new QListIterator<KDPEvent> (*tmpList);
    cursor->toLast();
    cursorDate = cursor->current()->getDtStart().date();
    return cursor->current();
  } else {
    while (recursCursor.current() &&
	   recursCursor.current()->getDtStart().date() != (*newestDate))
      --recursCursor;
    cursorDate = recursCursor.current()->getDtStart().date();
    return recursCursor.current();
  }
}

KDPEvent *CalObject::next()
{
  if (!oldestDate || !newestDate)
    return (KDPEvent *) 0L;

  QList<KDPEvent> *tmpList;
  int maxIterations = oldestDate->daysTo(*newestDate);
  int itCount = 0;
  
  if (calDict->isEmpty() && recursList.isEmpty())
    return (KDPEvent *) 0L;
  
  // if itCount is greater than maxIterations (i.e. going around in 
  // a full circle) we have a bug. :)
  while (itCount <= maxIterations) {
    ++itCount;
  RESET: if (cursor) {
    if (!cursor->current()) {
      // the cursor should ALWAYS be on a valid element here,
      // but if something has been deleted from the list, it may
      // no longer be on one. be dumb; reset to beginning of list.
      cursor->toFirst();
      if (!cursor->current()) {
	// shit! we are traversing a cursor on a deleted list.
	delete cursor;
	cursor = 0L;
	goto RESET;
      }
    }
    KDPEvent *anEvent = cursor->current();
    ++(*cursor);
    if (!cursor->current()) {
      // we have run out of events on this day.  delete cursor!
      delete cursor;
      cursor = 0L;
    }
    return anEvent;
  } else {
    // no cursor, we must check if anything in recursList
    while (recursCursor.current() &&
	   recursCursor.current()->getDtStart().date() != cursorDate)
      ++recursCursor;
    if (recursCursor.current()) {
      // we found one. :)
      KDPEvent *anEvent = recursCursor.current();
      // increment, so we are set for the next time.
      // there are more dates in the list we were last looking at
      ++recursCursor;
      // check to see that we haven't run off the end.  If so, reset.
      //if (!recursCursor.current())
      //	recursCursor.toFirst();
      return anEvent;
    } else {
      // we ran out of events in the recurrence cursor.  
      // Increment cursorDate, make a new calDict cursor if dates available.
      // reset recursCursor to beginning of list.
      cursorDate = cursorDate.addDays(1);
      // we may have circled the globe.
      if (cursorDate == (newestDate->addDays(1)))
	cursorDate = (*oldestDate);
      
      if ((tmpList = calDict->find(makeKey(cursorDate)))) {
	cursor = new QListIterator<KDPEvent> (*tmpList);
	cursor->toFirst();
      }
      recursCursor.toFirst();
    }
  }
  } // infinite while loop.
  debug("ieee! bug in calobject::next()");
  return (KDPEvent *) 0L;
}

KDPEvent *CalObject::prev()
{
  if (!oldestDate || !newestDate)
    return (KDPEvent *) 0L;

  QList<KDPEvent> *tmpList;
  int maxIterations = oldestDate->daysTo(*newestDate);
  int itCount = 0;

  if (calDict->isEmpty() && recursList.isEmpty())
    return (KDPEvent *) 0L;
  
  // if itCount is greater than maxIterations (i.e. going around in 
  // a full circle) we have a bug. :)
  while (itCount <= maxIterations) {
    ++itCount;
  RESET: if (cursor) {
    if (!cursor->current()) {
      // the cursor should ALWAYS be on a valid element here,
      // but if something has been deleted from the list, it may
      // no longer be on one. be dumb; reset to end of list.
      cursor->toLast();
      if (!cursor->current()) {
	// shit! we are traversing a cursor on a deleted list.
	delete cursor;
	cursor = 0L;
	goto RESET;
      }
    }
    KDPEvent *anEvent = cursor->current();
    --(*cursor);
    if (!cursor->current()) {
      // we have run out of events on this day.  delete cursor!
      delete cursor;
      cursor = 0L;
    }
    return anEvent;
  } else {
    // no cursor, we must check if anything in recursList
    while (recursCursor.current() &&
	   recursCursor.current()->getDtStart().date() != cursorDate)
      --recursCursor;
    if (recursCursor.current()) {
      // we found one. :)
      KDPEvent *anEvent = recursCursor.current();
      // increment, so we are set for the next time.
      // there are more dates in the list we were last looking at
      --recursCursor;
      // check to see that we haven't run off the end.  If so, reset.
      //if (!recursCursor.current())
      //	recursCursor.toLast();
      return anEvent;
    } else {
      // we ran out of events in the recurrence cursor.  
      // Decrement cursorDate, make a new calDict cursor if dates available.
      // reset recursCursor to end of list.
      cursorDate = cursorDate.addDays(-1);
      // we may have circled the globe.
      if (cursorDate == (oldestDate->addDays(-1)))
	cursorDate = (*newestDate);
      
      if ((tmpList = calDict->find(makeKey(cursorDate)))) {
	cursor = new QListIterator<KDPEvent> (*tmpList);
	cursor->toLast();
      }
      recursCursor.toLast();
    }
  }
  } // while loop
  debug("ieee! bug in calobject::prev()");
  return (KDPEvent *) 0L;
}

KDPEvent *CalObject::current()
{
  if (!oldestDate || !newestDate)
    return (KDPEvent *) 0L;

  if (calDict->isEmpty() && recursList.isEmpty())
    return (KDPEvent *) 0L;

  RESET: if (cursor) {
    if (cursor->current()) {
      // cursor should always be current, but weird things can
      // happen if it pointed to an event that got deleted...
      cursor->toFirst();
      if (!cursor->current()) {
	// shit! we are on a deleted list.
	delete cursor;
	cursor = 0L;
	goto RESET;
      }
    }
    return cursor->current();
  } else {
    // we must be in the recursList;
    return recursList.current();
  }
}

VObject *CalObject::eventToVTodo(const KDPEvent *anEvent)
{
  VObject *vtodo;
  QString tmpStr;

  vtodo = newVObject(VCTodoProp);

  tmpStr = qDateTimeToISO(anEvent->dateCreated);
  addPropValue(vtodo, VCDCreatedProp, tmpStr.data());

  addPropValue(vtodo, VCUniqueStringProp, 
	       anEvent->getVUID().data());

  tmpStr.sprintf("%i", anEvent->getRevisionNum());
  addPropValue(vtodo, VCSequenceProp, tmpStr.data());

  tmpStr = qDateTimeToISO(anEvent->getLastModified());
  addPropValue(vtodo, VCLastModifiedProp, tmpStr.data());  

  tmpStr.sprintf("MAILTO:%s",anEvent->getOrganizer().data());
  addPropValue(vtodo, ICOrganizerProp, 
	       tmpStr.data());

  // description BL:
  if (!anEvent->getDescription().isEmpty()) {
    VObject *d = addPropValue(vtodo, VCDescriptionProp,
			      (const char *) anEvent->getDescription());
    if (strchr((const char *) anEvent->getDescription(), '\n'))
      addProp(d, VCQuotedPrintableProp);
  }

  if (!anEvent->getSummary().isEmpty())
    addPropValue(vtodo, VCSummaryProp, anEvent->getSummary().data());
  addPropValue(vtodo, VCStatusProp, anEvent->getStatusStr().data());

  tmpStr.sprintf("%i",anEvent->getPriority());
  addPropValue(vtodo, VCPriorityProp, tmpStr.data());

  // pilot sync stuff
  tmpStr.sprintf("%i",anEvent->getPilotId());
  addPropValue(vtodo, KPilotIdProp, tmpStr.data());

  tmpStr.sprintf("%i",anEvent->getSyncStatus());
  addPropValue(vtodo, KPilotStatusProp, tmpStr.data());

  return vtodo;
}

// this stuff should really be in kdpevent!??
VObject* CalObject::eventToVEvent(const KDPEvent *anEvent)
{
  VObject *vevent;
  QString tmpStr;
  QStrList tmpStrList;
  
  vevent = newVObject(VCEventProp);

  tmpStr = qDateTimeToISO(anEvent->getDtStart(),
			  !anEvent->doesFloat());
  addPropValue(vevent, VCDTstartProp, tmpStr.data());

  // events that have time associated but take up no time should
  // not have both DTSTART and DTEND.
  if (anEvent->getDtStart() != anEvent->getDtEnd()) {
    tmpStr = qDateTimeToISO(anEvent->getDtEnd(),
			    !anEvent->doesFloat());
    addPropValue(vevent, VCDTendProp, tmpStr.data());
  }

  tmpStr = qDateTimeToISO(anEvent->dateCreated);
  addPropValue(vevent, VCDCreatedProp, tmpStr.data());

  addPropValue(vevent, VCUniqueStringProp,
	       anEvent->getVUID().data());

  tmpStr.sprintf("%i", anEvent->getRevisionNum());
  addPropValue(vevent, VCSequenceProp, tmpStr.data());

  tmpStr = qDateTimeToISO(anEvent->getLastModified());
  addPropValue(vevent, VCLastModifiedProp, tmpStr.data());

  // attendee and organizer stuff
  tmpStr.sprintf("MAILTO:%s",anEvent->getOrganizer().data());
  addPropValue(vevent, ICOrganizerProp,
	       tmpStr.data());
  if (anEvent->attendeeCount() != 0) {
    QList<Attendee> al = anEvent->getAttendeeList();
    QListIterator<Attendee> ai(al);
    Attendee *curAttendee;
    
    for (; ai.current(); ++ai) {
      curAttendee = ai.current();
      if (!curAttendee->getEmail().isEmpty() && 
	  !curAttendee->getName().isEmpty())
	tmpStr.sprintf("MAILTO:%s <%s>",curAttendee->getName().data(),
		       curAttendee->getEmail().data());
      else if (curAttendee->getName().isEmpty())
	tmpStr.sprintf("MAILTO: %s",curAttendee->getEmail().data());
      else if (curAttendee->getEmail().isEmpty())
	tmpStr.sprintf("MAILTO: %s",curAttendee->getName().data());
      else if (curAttendee->getName().isEmpty() && 
	       curAttendee->getEmail().isEmpty())
	debug("warning! this kdpevent has an attendee w/o name or email!");
      VObject *aProp = addPropValue(vevent, VCAttendeeProp, tmpStr.data());
      addPropValue(aProp, VCRSVPProp, curAttendee->RSVP() ? "TRUE" : "FALSE");;
      addPropValue(aProp, VCStatusProp, curAttendee->getStatusStr().data());
    }
  }


  // recurrence rule stuff
  if (anEvent->doesRecur()) {
    // some more variables
    QList<KDPEvent::rMonthPos> tmpPositions;
    QList<int> tmpDays;
    int *tmpDay;
    KDPEvent::rMonthPos *tmpPos;
    QString tmpStr2;

    switch(anEvent->doesRecur()) {
    case KDPEvent::rDaily:
      tmpStr.sprintf("D%i ",anEvent->rFreq);
      if (anEvent->rDuration > 0)
	tmpStr += "#";
      break;
    case KDPEvent::rWeekly:
      tmpStr.sprintf("W%i ",anEvent->rFreq);
      for (int i = 0; i < 7; i++) {
	if (anEvent->rDays.testBit(i))
	  tmpStr += dayFromNum(i);
      }
      break;
    case KDPEvent::rMonthlyPos:
      tmpStr.sprintf("MP%i ", anEvent->rFreq);
      // write out all rMonthPos's
      tmpPositions = anEvent->rMonthPositions;
      for (tmpPos = tmpPositions.first();
	   tmpPos;
	   tmpPos = tmpPositions.next()) {
	
	tmpStr2.sprintf("%i", tmpPos->rPos);
	if (tmpPos->negative)
	  tmpStr2 += "- ";
	else
	  tmpStr2 += "+ ";
	tmpStr += tmpStr2;
	for (int i = 0; i < 7; i++) {
	  if (tmpPos->rDays.testBit(i))
	    tmpStr += dayFromNum(i);
	}
      } // loop for all rMonthPos's
      break;
    case KDPEvent::rMonthlyDay:
      tmpStr.sprintf("MD%i ", anEvent->rFreq);
      // write out all rMonthDays;
      tmpDays = anEvent->rMonthDays;
      for (tmpDay = tmpDays.first();
	   tmpDay;
	   tmpDay = tmpDays.next()) {
	tmpStr2.sprintf("%i ", *tmpDay);
	tmpStr += tmpStr2;
      }
      break;
    case KDPEvent::rYearlyMonth:
      tmpStr.sprintf("YM%i ", anEvent->rFreq);
      // write out all the rYearNums;
      tmpDays = anEvent->rYearNums;
      for (tmpDay = tmpDays.first();
	   tmpDay;
	   tmpDay = tmpDays.next()) {
	tmpStr2.sprintf("%i ", *tmpDay);
	tmpStr += tmpStr2;
      }
      break;
    case KDPEvent::rYearlyDay:
      tmpStr.sprintf("YD%i ", anEvent->rFreq);
      // write out all the rYearNums;
      tmpDays = anEvent->rYearNums;
      for (tmpDay = tmpDays.first();
	   tmpDay;
	   tmpDay = tmpDays.next()) {
	tmpStr2.sprintf("%i ", *tmpDay);
	tmpStr += tmpStr2;
      }
      break;
    default:
      debug("ERROR, it should never get here in eventToVEvent!");
      break;
    } // switch

    if (anEvent->rDuration > 0) {
      tmpStr2.sprintf("#%i",anEvent->rDuration);
      tmpStr += tmpStr2;
    } else if (anEvent->rDuration == -1) {
      tmpStr += "#0"; // defined as repeat forever
    } else {
      tmpStr += qDateTimeToISO(anEvent->rEndDate, FALSE);
    }
    addPropValue(vevent,VCRRuleProp, tmpStr.data());

  } // event repeats


  // exceptions to recurrence
  QDateList dateList(FALSE);
  dateList = anEvent->getExDates();
  QDate *tmpDate;
  QString tmpStr2 = "";

  for (tmpDate = dateList.first(); tmpDate; tmpDate = dateList.next()) {
    tmpStr = qDateToISO(*tmpDate) + ";";
    tmpStr2 += tmpStr;
  }
  if (!tmpStr2.isEmpty()) {
    tmpStr2.truncate(tmpStr2.length()-1);
    addPropValue(vevent, VCExDateProp, tmpStr2.data());
  }

  // description
  if (!anEvent->getDescription().isEmpty()) {
    VObject *d = addPropValue(vevent, VCDescriptionProp,
			      (const char *) anEvent->getDescription());
    if (strchr((const char *) anEvent->getDescription(), '\n'))
      addProp(d, VCQuotedPrintableProp);
  }

  if (!anEvent->getSummary().isEmpty())
    addPropValue(vevent, VCSummaryProp, anEvent->getSummary().data());

  addPropValue(vevent, VCStatusProp, anEvent->getStatusStr().data());
  addPropValue(vevent, VCClassProp, anEvent->getSecrecyStr().data());

  // categories
  tmpStrList = anEvent->getCategories();
  tmpStr = "";
  char *catStr;
  for (catStr = tmpStrList.first(); catStr; 
       catStr = tmpStrList.next()) {
    if (catStr[0] == ' ')
      tmpStr += (catStr+1);
    else
      tmpStr += catStr;
    // this must be a ';' character as the vCalendar specification requires!
    // vcc.y has been hacked to translate the ';' to a ',' when the vcal is
    // read in.
    tmpStr += ";";
  }
  if (!tmpStr.isEmpty()) {
    tmpStr.truncate(tmpStr.length()-1);
    addPropValue(vevent, VCCategoriesProp, tmpStr.data());
  }

  tmpStrList = anEvent->getAttachments();
  char *attachStr;
  for (attachStr = tmpStrList.first(); attachStr;
       attachStr = tmpStrList.next())
    addPropValue(vevent, VCAttachProp, attachStr);
  
  tmpStrList = anEvent->getResources();
  tmpStr = "";
  char *resStr;
  for (resStr = tmpStrList.first(); resStr;
       resStr = tmpStrList.next()) {
    tmpStr += resStr;
    if (tmpStrList.next())
      tmpStr += ";";
  }
  if (!tmpStr.isEmpty())
    addPropValue(vevent, VCResourcesProp, tmpStr.data());
  
  // alarm stuff
  if (anEvent->getAlarmRepeatCount()) {
    VObject *a = addProp(vevent, VCDAlarmProp);
    tmpStr = qDateTimeToISO(anEvent->getAlarmTime());
    addPropValue(a, VCRunTimeProp, tmpStr.data());
    addPropValue(a, VCRepeatCountProp, "1");
    addPropValue(a, VCDisplayStringProp, "beep!");
    if (!anEvent->getAudioAlarmFile().isEmpty()) {
      a = addProp(vevent, VCAAlarmProp);
      addPropValue(a, VCRunTimeProp, tmpStr.data());
      addPropValue(a, VCRepeatCountProp, "1");
      addPropValue(a, VCAudioContentProp, anEvent->getAudioAlarmFile().data());
    }
    if (!anEvent->getProgramAlarmFile().isEmpty()) {
      a = addProp(vevent, VCPAlarmProp);
      addPropValue(a, VCRunTimeProp, tmpStr.data());
      addPropValue(a, VCRepeatCountProp, "1");
      addPropValue(a, VCProcedureNameProp, anEvent->getProgramAlarmFile().data());
    }
  }
	    
  tmpStr.sprintf("%i",anEvent->getPriority());
  addPropValue(vevent, VCPriorityProp, tmpStr.data());
  tmpStr.sprintf("%i",anEvent->getTransparency());
  addPropValue(vevent, VCTranspProp, tmpStr.data());
  tmpStr.sprintf("%i", anEvent->getRelatedTo());
  addPropValue(vevent, VCRelatedToProp, tmpStr.data());

  // pilot sync stuff
  tmpStr.sprintf("%i",anEvent->getPilotId());
  addPropValue(vevent, KPilotIdProp, tmpStr.data());

  tmpStr.sprintf("%i",anEvent->getSyncStatus());
  addPropValue(vevent, KPilotStatusProp, tmpStr.data());

  return vevent;
}

KDPEvent *CalObject::VTodoToEvent(VObject *vtodo)
{
  KDPEvent *anEvent;
  VObject *vo;
  char *s;

  anEvent = new KDPEvent;
  anEvent->setTodoStatus(TRUE);

  if ((vo = isAPropertyOf(vtodo, VCDCreatedProp)) != 0) {
      anEvent->dateCreated = ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo)));
      deleteStr(s);
  }

  vo = isAPropertyOf(vtodo, VCUniqueStringProp);
  // while the UID property is preferred, it is not required.  We'll use the
  // default KDPEvent UID if none is given.
  if (vo) {
    anEvent->setVUID(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }

  if ((vo = isAPropertyOf(vtodo, VCLastModifiedProp)) != 0) {
    anEvent->setLastModified(ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setLastModified(QDateTime(QDate::currentDate(),
				       QTime::currentTime()));

  // if our extension property for the event's ORGANIZER exists, add it.
  if ((vo = isAPropertyOf(vtodo, ICOrganizerProp)) != 0) {
    anEvent->setOrganizer(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  } else {
    anEvent->setOrganizer(getEmail());
  }

  // BL: description for todo
  if ((vo = isAPropertyOf(vtodo, VCDescriptionProp)) != 0) {
    anEvent->setDescription(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }
  
  if ((vo = isAPropertyOf(vtodo, VCSummaryProp))) {
    anEvent->setSummary(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }
  
  if ((vo = isAPropertyOf(vtodo, VCStatusProp)) != 0) {
    anEvent->setStatus(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }
  else
    anEvent->setStatus("NEEDS ACTION");
  
  if ((vo = isAPropertyOf(vtodo, VCPriorityProp))) {
    anEvent->setPriority(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  
  /* PILOT SYNC STUFF */
  if ((vo = isAPropertyOf(vtodo, KPilotIdProp))) {
    anEvent->setPilotId(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setPilotId(0);

  if ((vo = isAPropertyOf(vtodo, KPilotStatusProp))) {
    anEvent->setSyncStatus(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setSyncStatus(KDPEvent::SYNCMOD);

  return anEvent;
}

KDPEvent* CalObject::VEventToEvent(VObject *vevent)
{
  KDPEvent *anEvent;
  VObject *vo;
  VObjectIterator voi;
  char *s;

  anEvent = new KDPEvent;

  
  if ((vo = isAPropertyOf(vevent, VCDCreatedProp)) != 0) {
      anEvent->dateCreated = ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo)));
      deleteStr(s);
  }

  vo = isAPropertyOf(vevent, VCUniqueStringProp);

  if (!vo) {
    parseError(VCUniqueStringProp);
    return 0;
  }
  anEvent->setVUID(s = fakeCString(vObjectUStringZValue(vo)));
  deleteStr(s);

  // again NSCAL doesn't give us much to work with, so we improvise...
  if ((vo = isAPropertyOf(vevent, VCSequenceProp)) != 0) {
    anEvent->setRevisionNum(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setRevisionNum(0);

  if ((vo = isAPropertyOf(vevent, VCLastModifiedProp)) != 0) {
    anEvent->setLastModified(ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setLastModified(QDateTime(QDate::currentDate(),
				       QTime::currentTime()));

  // if our extension property for the event's ORGANIZER exists, add it.
  if ((vo = isAPropertyOf(vevent, ICOrganizerProp)) != 0) {
    anEvent->setOrganizer(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  } else {
    anEvent->setOrganizer(getEmail());
  }

  // deal with attendees.
  initPropIterator(&voi, vevent);
  while (moreIteration(&voi)) {
    vo = nextVObject(&voi);
    if (strcmp(vObjectName(vo), VCAttendeeProp) == 0) {
      Attendee *a;
      VObject *vp;
      s = fakeCString(vObjectUStringZValue(vo));
      QString tmpStr = s;
      deleteStr(s);
      tmpStr = tmpStr.simplifyWhiteSpace();
      int emailPos1, emailPos2;
      if ((emailPos1 = tmpStr.find('<')) > 0) {
	// both email address and name
	emailPos2 = tmpStr.find('>');
	a = new Attendee(tmpStr.left(emailPos1 - 1).data(),
			 tmpStr.mid(emailPos1 + 1, 
				    emailPos2 - (emailPos1 + 1)).data());
      } else if (tmpStr.find('@') > 0) {
	// just an email address
	a = new Attendee(0, tmpStr.data());
      } else {
	// just a name
	a = new Attendee(tmpStr.data());
      }

      // is there an RSVP property?
      if ((vp = isAPropertyOf(vo, VCRSVPProp)) != 0)
	a->setRSVP(vObjectStringZValue(vp));
      // is there a status property?
      if ((vp = isAPropertyOf(vo, VCStatusProp)) != 0)
	a->setStatus(vObjectStringZValue(vp));
      // add the attendee
      anEvent->addAttendee(a);
    }
  }
 
  // This isn't strictly true.  An event that doesn't have a start time
  // or an end time doesn't "float", it has an anchor in time but it doesn't
  // "take up" any time.
  /*if ((isAPropertyOf(vevent, VCDTstartProp) == 0) ||
      (isAPropertyOf(vevent, VCDTendProp) == 0)) {
    anEvent->setFloats(TRUE);
    } else {
    }*/
  anEvent->setFloats(FALSE);
  
  if ((vo = isAPropertyOf(vevent, VCDTstartProp)) != 0) {
    anEvent->setDtStart(ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo))));
    //    debug("s is %s, ISO is %s",
    //	  s, ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo))).toString().data());
    deleteStr(s);
    if (anEvent->getDtStart().time().isNull())
      anEvent->setFloats(TRUE);
  }
  
  if ((vo = isAPropertyOf(vevent, VCDTendProp)) != 0) {
    anEvent->setDtEnd(ISOToQDateTime(s = fakeCString(vObjectUStringZValue(vo))));
      deleteStr(s);
      if (anEvent->getDtEnd().time().isNull())
	anEvent->setFloats(TRUE);
  }
  
  // at this point, there should be at least a start or end time.
  // fix up for events that take up no time but have a time associated
  if (!(vo = isAPropertyOf(vevent, VCDTstartProp)))
    anEvent->setDtStart(anEvent->getDtEnd());
  if (!(vo = isAPropertyOf(vevent, VCDTendProp)))
    anEvent->setDtEnd(anEvent->getDtStart());
  
  ///////////////////////////////////////////////////////////////////////////

  // repeat stuff
  if ((vo = isAPropertyOf(vevent, VCRRuleProp)) != 0) {
    QString tmpStr = (s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
    tmpStr.simplifyWhiteSpace();
    tmpStr = tmpStr.upper();

    /********************************* DAILY ******************************/
    if (tmpStr.left(1) == "D") {
      int index = tmpStr.find(' ');
      int rFreq = tmpStr.mid(1, (index-1)).toInt();
      index = tmpStr.findRev(' ') + 1; // advance to last field
      if (tmpStr.mid(index,1) == "#") index++;
      if (tmpStr.find('T', index) != -1) {
	QDate rEndDate = (ISOToQDateTime(tmpStr.mid(index, tmpStr.length()-index).data())).date();
	anEvent->setRecursDaily(rFreq, rEndDate);
      } else {
	int rDuration = tmpStr.mid(index, tmpStr.length()-index).toInt();
	if (rDuration == 0) // VEvents set this to 0 forever, we use -1
	  anEvent->setRecursDaily(rFreq, -1);
	else
	  anEvent->setRecursDaily(rFreq, rDuration);
      }
    } 
    /********************************* WEEKLY ******************************/
    else if (tmpStr.left(1) == "W") {
      int index = tmpStr.find(' ');
      int last = tmpStr.findRev(' ') + 1;
      int rFreq = tmpStr.mid(1, (index-1)).toInt();
      index += 1; // advance to beginning of stuff after freq
      QBitArray qba(7);
      QString dayStr;
      if( index == last ) {
	// e.g. W1 #0
	qba.setBit(anEvent->getDtStart().date().dayOfWeek() - 1);
      }
      else {
	// e.g. W1 SU #0
	while (index < last) {
	  dayStr = tmpStr.mid(index, 3);
	  int dayNum = numFromDay(dayStr);
	  qba.setBit(dayNum);
	  index += 3; // advance to next day, or possibly "#"
	}
      }
      index = last; if (tmpStr.mid(index,1) == "#") index++;
      if (tmpStr.find('T', index) != -1) {
	QDate rEndDate = (ISOToQDateTime(tmpStr.mid(index, tmpStr.length()-index).data())).date();
	anEvent->setRecursWeekly(rFreq, qba, rEndDate);
      } else {
	int rDuration = tmpStr.mid(index, tmpStr.length()-index).toInt();
	if (rDuration == 0)
	  anEvent->setRecursWeekly(rFreq, qba, -1);
	else
	  anEvent->setRecursWeekly(rFreq, qba, rDuration);
      }
    } 
    /**************************** MONTHLY-BY-POS ***************************/
    else if (tmpStr.left(2) == "MP") {
      int index = tmpStr.find(' ');
      int last = tmpStr.findRev(' ') + 1;
      int rFreq = tmpStr.mid(2, (index-1)).toInt();
      index += 1; // advance to beginning of stuff after freq
      QBitArray qba(7);
      short tmpPos;
      if( index == last ) {
	// e.g. MP1 #0
	tmpPos = anEvent->getDtStart().date().day()/7 + 1;
	if( tmpPos == 5 )
	  tmpPos = -1;
	qba.setBit(anEvent->getDtStart().date().dayOfWeek() - 1);
	anEvent->addRecursMonthlyPos(tmpPos, qba);
      }
      else {
	// e.g. MP1 1+ SU #0
	while (index < last) {
	  tmpPos = tmpStr.mid(index,1).toShort();
	  index += 1;
	  if (tmpStr.mid(index,1) == "-")
	    // convert tmpPos to negative
	    tmpPos = 0 - tmpPos;
	  index += 2; // advance to day(s)
	  while (numFromDay(tmpStr.mid(index,3)) >= 0) {
	    int dayNum = numFromDay(tmpStr.mid(index,3));
	    qba.setBit(dayNum);
	    index += 3; // advance to next day, or possibly pos or "#"
	  }
	  anEvent->addRecursMonthlyPos(tmpPos, qba);
	  qba.detach();
	  qba.fill(FALSE); // clear out
	} // while != "#"
      }
      index = last; if (tmpStr.mid(index,1) == "#") index++;
      if (tmpStr.find('T', index) != -1) {
	QDate rEndDate = (ISOToQDateTime(tmpStr.mid(index, tmpStr.length() - 
						    index).data())).date();
	anEvent->setRecursMonthly(KDPEvent::rMonthlyPos, rFreq, rEndDate);
      } else {
	int rDuration = tmpStr.mid(index, tmpStr.length()-index).toInt();
	if (rDuration == 0)
	  anEvent->setRecursMonthly(KDPEvent::rMonthlyPos, rFreq, -1);
	else
	  anEvent->setRecursMonthly(KDPEvent::rMonthlyPos, rFreq, rDuration);
      }
    }

    /**************************** MONTHLY-BY-DAY ***************************/
    else if (tmpStr.left(2) == "MD") {
      int index = tmpStr.find(' ');
      int last = tmpStr.findRev(' ') + 1;
      int rFreq = tmpStr.mid(2, (index-1)).toInt();
      index += 1;
      short tmpDay;
      if( index == last ) {
	// e.g. MD1 #0
	tmpDay = anEvent->getDtStart().date().day();
	anEvent->addRecursMonthlyDay(tmpDay);
      }
      else {
	// e.g. MD1 3 #0
	while (index < last) {
	  int index2 = tmpStr.find(' ', index); 
	  tmpDay = tmpStr.mid(index, (index2-index)).toShort();
	  index = index2-1;
	  if (tmpStr.mid(index, 1) == "-")
	    tmpDay = 0 - tmpDay;
	  index += 2; // advance the index;
	  anEvent->addRecursMonthlyDay(tmpDay);
	} // while != #
      }
      index = last; if (tmpStr.mid(index,1) == "#") index++;
      if (tmpStr.find('T', index) != -1) {
	QDate rEndDate = (ISOToQDateTime(tmpStr.mid(index, tmpStr.length()-index).data())).date();
	anEvent->setRecursMonthly(KDPEvent::rMonthlyDay, rFreq, rEndDate);
      } else {
	int rDuration = tmpStr.mid(index, tmpStr.length()-index).toInt();
	if (rDuration == 0)
	  anEvent->setRecursMonthly(KDPEvent::rMonthlyDay, rFreq, -1);
	else
	  anEvent->setRecursMonthly(KDPEvent::rMonthlyDay, rFreq, rDuration);
      }
    }

    /*********************** YEARLY-BY-MONTH *******************************/
    else if (tmpStr.left(2) == "YM") {
      int index = tmpStr.find(' ');
      int last = tmpStr.findRev(' ') + 1;
      int rFreq = tmpStr.mid(2, (index-1)).toInt();
      index += 1;
      short tmpMonth;
      if( index == last ) {
	// e.g. YM1 #0
	tmpMonth = anEvent->getDtStart().date().month();
	anEvent->addRecursYearlyNum(tmpMonth);
      }
      else {
	// e.g. YM1 3 #0
	while (index < last) {
	  int index2 = tmpStr.find(' ', index);
	  tmpMonth = tmpStr.mid(index, (index2-index)).toShort();
	  index = index2+1;
	  anEvent->addRecursYearlyNum(tmpMonth);
	} // while != #
      }
      index = last; if (tmpStr.mid(index,1) == "#") index++;
      if (tmpStr.find('T', index) != -1) {
	QDate rEndDate = (ISOToQDateTime(tmpStr.mid(index, tmpStr.length()-index).data())).date();
	anEvent->setRecursYearly(KDPEvent::rYearlyMonth, rFreq, rEndDate);
      } else {
	int rDuration = tmpStr.mid(index, tmpStr.length()-index).toInt();
	if (rDuration == 0)
	  anEvent->setRecursYearly(KDPEvent::rYearlyMonth, rFreq, -1);
	else
	  anEvent->setRecursYearly(KDPEvent::rYearlyMonth, rFreq, rDuration);
      }
    }

    /*********************** YEARLY-BY-DAY *********************************/
    else if (tmpStr.left(2) == "YD") {
      int index = tmpStr.find(' ');
      int last = tmpStr.findRev(' ') + 1;
      int rFreq = tmpStr.mid(2, (index-1)).toInt();
      index += 1;
      short tmpDay;
      if( index == last ) {
	// e.g. YD1 #0
	tmpDay = anEvent->getDtStart().date().dayOfYear();
	anEvent->addRecursYearlyNum(tmpDay);
      }
      else {
	// e.g. YD1 123 #0
	while (index < last) {
	  int index2 = tmpStr.find(' ', index);
	  tmpDay = tmpStr.mid(index, (index2-index)).toShort();
	  index = index2+1;
	  anEvent->addRecursYearlyNum(tmpDay);
	} // while != #
      }
      index = last; if (tmpStr.mid(index,1) == "#") index++;
      if (tmpStr.find('T', index) != -1) {
	QDate rEndDate = (ISOToQDateTime(tmpStr.mid(index, tmpStr.length()-index).data())).date();
	anEvent->setRecursYearly(KDPEvent::rYearlyDay, rFreq, rEndDate);
      } else {
	int rDuration = tmpStr.mid(index, tmpStr.length()-index).toInt();
	if (rDuration == 0)
	  anEvent->setRecursYearly(KDPEvent::rYearlyDay, rFreq, -1);
	else
	  anEvent->setRecursYearly(KDPEvent::rYearlyDay, rFreq, rDuration);
      }
    } else {
      debug("we don't understand this type of recurrence!");
    } // if
  } // repeats


  if ((vo = isAPropertyOf(vevent, VCExDateProp)) != 0) {
    anEvent->setExDates(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }


  if ((vo = isAPropertyOf(vevent, VCSummaryProp))) {
    anEvent->setSummary(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }

  if ((vo = isAPropertyOf(vevent, VCDescriptionProp)) != 0) {
    if (!anEvent->getDescription().isEmpty()) {
      anEvent->setDescription(anEvent->getDescription() + "\n" +
			      s = fakeCString(vObjectUStringZValue(vo)));
    } else {
      anEvent->setDescription(s = fakeCString(vObjectUStringZValue(vo)));
    }
    deleteStr(s);
  }

  // some stupid vCal exporters ignore the standard and use Description
  // instead of Summary for the default field.  Correct for this.
  if (anEvent->getSummary().isEmpty() && 
      !(anEvent->getDescription().isEmpty())) {
    QString tmpStr = anEvent->getDescription().simplifyWhiteSpace();
    anEvent->setDescription("");
    anEvent->setSummary(tmpStr.data());
  }  

  if ((vo = isAPropertyOf(vevent, VCStatusProp)) != 0) {
    QString tmpStr(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
    anEvent->setStatus(tmpStr);
  }
  else
    anEvent->setStatus("NEEDS ACTION");

  if ((vo = isAPropertyOf(vevent, VCClassProp)) != 0) {
    anEvent->setSecrecy(s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
  }
  else
    anEvent->setSecrecy("PUBLIC");

  QStrList tmpStrList;
  int index1 = 0;
  int index2 = 0;
  if ((vo = isAPropertyOf(vevent, VCCategoriesProp)) != 0) {
    QString categories = (s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
    //const char* category;
    QString category;
    while ((index2 = categories.find(',', index1)) != -1) {
	//category = (const char *) categories.mid(index1, (index2 - index1));
      category = categories.mid(index1, (index2 - index1));	
      tmpStrList.append(category);
      index1 = index2+1;
    }
    // get last category
    category = categories.mid(index1, (categories.length()-index1));
    tmpStrList.append(category);
    anEvent->setCategories(tmpStrList);
  }

  tmpStrList.clear();
  initPropIterator(&voi, vevent);
  while (moreIteration(&voi)) {
    vo = nextVObject(&voi);
    if (strcmp(vObjectName(vo), VCAttachProp) == 0) {
      tmpStrList.append(s = fakeCString(vObjectUStringZValue(vo)));
      deleteStr(s);
    }
  }
  anEvent->setAttachments(tmpStrList);

  
  if ((vo = isAPropertyOf(vevent, VCResourcesProp)) != 0) {
    QString resources = (s = fakeCString(vObjectUStringZValue(vo)));
    deleteStr(s);
    tmpStrList.clear();
    index1 = 0;
    index2 = 0;
    const char *resource;
    while ((index2 = resources.find(';', index1)) != -1) {
      resource = (const char *) resources.mid(index1, (index2 - index1));
      tmpStrList.append(resource);
      index1 = index2;
    }
    anEvent->setResources(tmpStrList);
  }

  /* alarm stuff */
  if ((vo = isAPropertyOf(vevent, VCDAlarmProp))) {
    VObject *a;
    if ((a = isAPropertyOf(vo, VCRunTimeProp))) {
      anEvent->setAlarmTime(ISOToQDateTime(s = fakeCString(vObjectUStringZValue(a))));
      deleteStr(s);
    }
    anEvent->setAlarmRepeatCount(1);
    if ((vo = isAPropertyOf(vevent, VCPAlarmProp))) {
      if ((a = isAPropertyOf(vo, VCProcedureNameProp))) {
	anEvent->setProgramAlarmFile(s = fakeCString(vObjectUStringZValue(a)));
	deleteStr(s);
      }
    }
    if ((vo = isAPropertyOf(vevent, VCAAlarmProp))) {
      if ((a = isAPropertyOf(vo, VCAudioContentProp))) {
	anEvent->setAudioAlarmFile(s = fakeCString(vObjectUStringZValue(a)));
	deleteStr(s);
      }
    }
  }

  if ((vo = isAPropertyOf(vevent, VCPriorityProp))) {
    anEvent->setPriority(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  
  if ((vo = isAPropertyOf(vevent, VCTranspProp)) != 0) {
    anEvent->setTransparency(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }

  if ((vo = isAPropertyOf(vevent, VCRelatedToProp)) != 0) {
    anEvent->setRelatedTo(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }

  /* PILOT SYNC STUFF */
  if ((vo = isAPropertyOf(vevent, KPilotIdProp))) {
    anEvent->setPilotId(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setPilotId(0);

  if ((vo = isAPropertyOf(vevent, KPilotStatusProp))) {
    anEvent->setSyncStatus(atoi(s = fakeCString(vObjectUStringZValue(vo))));
    deleteStr(s);
  }
  else
    anEvent->setSyncStatus(KDPEvent::SYNCMOD);

  return anEvent;
}

QList<KDPEvent> CalObject::search(const QRegExp &searchExp) const
{
  QIntDictIterator<QList<KDPEvent> > qdi(*calDict);
  char *testStr;
  QList<KDPEvent> matchList, *tmpList, tmpList2;
  KDPEvent *matchEvent;

  qdi.toFirst();
  while ((tmpList = qdi.current()) != 0L) {
    ++qdi;
    for (matchEvent = tmpList->first(); matchEvent;
	 matchEvent = tmpList->next()) {
      testStr = matchEvent->getSummary().data();
      if ((searchExp.match(testStr) != -1) && (matchList.findRef(matchEvent) == -1))
	matchList.append(matchEvent);
      // do other match tests here...
    }
  }

  tmpList2 = recursList;
  tmpList2.setAutoDelete(FALSE); // just to make sure
  for (matchEvent = tmpList2.first(); matchEvent;
       matchEvent = tmpList2.next()) {
    testStr = matchEvent->getSummary().data();
    if ((searchExp.match(testStr) != -1) && 
	(matchList.findRef(matchEvent) == -1)) 
      matchList.append(matchEvent);
    // do other match tests here...
  }

  // now, we have to sort it based on getDtStart()
  QList<KDPEvent> matchListSorted;
  for (matchEvent = matchList.first(); matchEvent; 
       matchEvent = matchList.next()) {
    if (!matchListSorted.isEmpty() &&
        matchEvent->getDtStart() < matchListSorted.at(0)->getDtStart()) {
      matchListSorted.insert(0,matchEvent);
      goto nextToInsert;
    }
    for (int i = 0; (uint) i+1 < matchListSorted.count(); i++) {
      if (matchEvent->getDtStart() > matchListSorted.at(i)->getDtStart() &&
          matchEvent->getDtStart() <= matchListSorted.at(i+1)->getDtStart()) {
        matchListSorted.insert(i+1,matchEvent);
        goto nextToInsert;
      }
    }
    matchListSorted.append(matchEvent);
  nextToInsert:
    continue;
  }

  return matchListSorted;
}

int CalObject::numEvents(const QDate &qd)
{
  QList<KDPEvent> *tmpList;
  KDPEvent *anEvent;
  int count = 0;
  int extraDays, i;

  // first get the simple case from the dictionary.
  tmpList = calDict->find(makeKey(qd));
  if (tmpList)
    count += tmpList->count();

  // next, check for repeating events.  Even those that span multiple days...
  for (anEvent = recursList.first(); anEvent; anEvent = recursList.next()) {
    if (anEvent->isMultiDay()) {
      extraDays = anEvent->getDtStart().date().daysTo(anEvent->getDtEnd().date());
      //debug("multi day event w/%d days", extraDays);
      for (i = 0; i <= extraDays; i++) {
	if (anEvent->recursOn(qd.addDays(i))) {
	  ++count;
	  break;
	}
      }
    } else {
      if (anEvent->recursOn(qd))
	++count;
    }
  }
  return count;
}

void CalObject::checkAlarms()
{
  QList<KDPEvent> alarmEvents;
  QIntDictIterator<QList<KDPEvent> > dictIt(*calDict);
  QList<KDPEvent> *tmpList;
  KDPEvent *anEvent;
  QDateTime tmpDT;

  // this function has to look at every event in the whole database
  // and find if any have an alarm pending.

  while (dictIt.current()) {
    tmpList = dictIt.current();
    for (anEvent = tmpList->first(); anEvent;
	 anEvent = tmpList->next()) {
      tmpDT = anEvent->getAlarmTime();
      if (tmpDT.date() == QDate::currentDate()) {
	if ((tmpDT.time().hour() == QTime::currentTime().hour()) &&
	    (tmpDT.time().minute() == QTime::currentTime().minute()))
	  alarmEvents.append(anEvent);
      }
    }
    ++dictIt;
  }
  for (anEvent = recursList.first(); anEvent;
       anEvent = recursList.next()) {
    tmpDT = anEvent->getAlarmTime();
    if(anEvent->recursOn(QDate::currentDate())) {
      if ((tmpDT.time().hour() == QTime::currentTime().hour()) &&
	  (tmpDT.time().minute() == QTime::currentTime().minute()))
	alarmEvents.append(anEvent);
    }
  }

  if (!alarmEvents.isEmpty())
    emit alarmSignal(alarmEvents);
}

/****************************** PROTECTED METHODS ****************************/
// after changes are made to an event, this should be called.
void CalObject::updateEvent(KDPEvent *anEvent)
{
  QIntDictIterator<QList<KDPEvent> > qdi(*calDict);
  QList<KDPEvent> *tmpList;

  anEvent->setSyncStatus(KDPEvent::SYNCMOD);
  anEvent->setLastModified(QDateTime::currentDateTime());
  // we should probably update the revision number here,
  // or internally in the KDPEvent itself when certain things change.
  // need to verify with ical documentation.

  // handle sending the event to those attendees that need it.
  // mostly broken right now.
  if (anEvent->attendeeCount()) {
    QList<Attendee> al;
    Attendee *a;
    
    al = anEvent->getAttendeeList();
    for (a = al.first(); a; a = al.next()) {
      if ((a->flag) && (a->RSVP())) {
	//debug("send appointment to %s",a->getName().data());
	a->flag = FALSE;
      }
    }
  }
      
  // we don't need to do anything to Todo events.
  if (anEvent->getTodoStatus() != TRUE) {
    // the first thing we do is REMOVE all occurances of the event from 
    // both the dictionary and the recurrence list.  Then we reinsert it.
    // We don't bother about optimizations right now.
    qdi.toFirst();
    while ((tmpList = qdi.current()) != 0L) {
      ++qdi;
      tmpList->removeRef(anEvent);
    }
    // take any instances of it out of the recurrence list
    if (recursList.findRef(anEvent) != -1)
      recursList.take();
    
    // ok the event is now GONE.  we want to re-insert it.
    insertEvent(anEvent);
  }
  emit calUpdated(anEvent);
  return;  
}

// this function will take a VEvent and insert it into the event
// dictionary for the CalObject.  If there is no list of events for that
// particular location in the dictionary, a new one will be created.
void CalObject::insertEvent(const KDPEvent *anEvent)
{
  long tmpKey;
  QString tmpDateStr;
  QList<KDPEvent> *eventList;
  int extraDays, dayCount;

  // initialize if they haven't been allocated yet;
  if (!oldestDate) {
    oldestDate = new QDate();
    (*oldestDate) = anEvent->getDtStart().date();
  } 
  if (!newestDate) {
    newestDate = new QDate();
    (*newestDate) = anEvent->getDtStart().date();
  }
    
  // update oldest and newest dates if necessary.
  if (anEvent->getDtStart().date() < (*oldestDate))
    (*oldestDate) = anEvent->getDtStart().date();
  if (anEvent->getDtStart().date() > (*newestDate))
    (*newestDate) = anEvent->getDtStart().date();

  if (anEvent->doesRecur()) {
    recursList.append(anEvent);
  } else {
    // set up the key
    extraDays = anEvent->getDtStart().date().daysTo(anEvent->getDtEnd().date());
    for (dayCount = 0; dayCount <= extraDays; dayCount++) {
      tmpKey = makeKey(anEvent->getDtStart().addDays(dayCount));
      // insert the item into the proper list in the dictionary
      if ((eventList = calDict->find(tmpKey)) != 0) {
	eventList->append(anEvent);
      } else {
	// no items under that date yet
	eventList = new QList<KDPEvent>;
	eventList->append(anEvent);
	calDict->insert(tmpKey, eventList);
      }
    }
  }
}

// make a long dict key out of a QDateTime
long int CalObject::makeKey(const QDateTime &dt)
{
  QDate tmpD;
  QString tmpStr;

  tmpD = dt.date();
  tmpStr.sprintf("%d%.2d%.2d",tmpD.year(), tmpD.month(), tmpD.day());
  return tmpStr.toLong();
}

// make a long dict key out of a QDate
long int CalObject::makeKey(const QDate &d)
{
  QString tmpStr;

  tmpStr.sprintf("%d%.2d%.2d",d.year(), d.month(), d.day());
  return tmpStr.toLong();
}

// taking a QDate, this function will look for an eventlist in the dict
// with that date attached -
// BL: an the returned list should be deleted!!!
QList<KDPEvent> CalObject::getEventsForDate(const QDate &qd, bool sorted)
{    
  QList<KDPEvent> eventList;
  QList<KDPEvent> *tmpList;
  KDPEvent *anEvent;

  tmpList = calDict->find(makeKey(qd));
  if (tmpList) {
    for (anEvent = tmpList->first(); anEvent;
	 anEvent = tmpList->next())
      eventList.append(anEvent);
  }
  int extraDays, i;
  for (anEvent = recursList.first(); anEvent; anEvent = recursList.next()) {
    if (anEvent->isMultiDay()) {
      extraDays = anEvent->getDtStart().date().daysTo(anEvent->getDtEnd().date());
      for (i = 0; i <= extraDays; i++) {
	if (anEvent->recursOn(qd.addDays(i))) {
	  eventList.append(anEvent);
	  break;
	}
      }
    } else {
      if (anEvent->recursOn(qd))
	eventList.append(anEvent);
    }
  }
  if (!sorted) {
    updateCursors(eventList.first());
    return eventList;
  }
  //  debug("Sorting getEvents for date\n");
  // now, we have to sort it based on getDtStart.time()
  QList<KDPEvent> eventListSorted;
  for (anEvent = eventList.first(); anEvent; anEvent = eventList.next()) {
    if (!eventListSorted.isEmpty() &&
	anEvent->getDtStart().time() < eventListSorted.at(0)->getDtStart().time()) {
      eventListSorted.insert(0,anEvent);
      goto nextToInsert;
    }
    for (i = 0; (uint) i+1 < eventListSorted.count(); i++) {
      if (anEvent->getDtStart().time() > eventListSorted.at(i)->getDtStart().time() &&
	  anEvent->getDtStart().time() <= eventListSorted.at(i+1)->getDtStart().time()) {
	eventListSorted.insert(i+1,anEvent);
	goto nextToInsert;
      }
    }
    eventListSorted.append(anEvent);
  nextToInsert:
    continue;
  }
  updateCursors(eventListSorted.first());
  return eventListSorted;
}

// taking a QDateTime, this function will look for an eventlist in the dict
// with that date attached.
// this list is dynamically allocated and SHOULD BE DELETED when done with!
inline QList<KDPEvent> CalObject::getEventsForDate(const QDateTime &qdt)
{    
  return getEventsForDate(qdt.date());
}

QString CalObject::getHolidayForDate(const QDate &qd)
{
  static int lastYear = 0;
  KConfig *config(kapp->getConfig());

  config->setGroup("Personal Settings");
  QString holidays(config->readEntry("Holidays", "us"));
  if (holidays == "(none)")
    return (QString(""));

  if ((lastYear == 0) || (qd.year() != lastYear)) {
      lastYear = qd.year() - 1900; // silly parse_year takes 2 digit year...
    parse_holidays(holidays.data(), lastYear, 0);
  }

  if (holiday[qd.dayOfYear()-1].string) {
    return(QString(holiday[qd.dayOfYear()-1].string));
  }

  else
    return(QString(""));
}

void CalObject::updateCursors(KDPEvent *dEvent)
{
  if (!dEvent)
    return;
  
  QDate newDate(dEvent->getDtStart().date());
  cursorDate = newDate;

  if (calDict->isEmpty() && recursList.isEmpty())
    return;
  
  if (cursor && cursor->current() && 
      (newDate == cursor->current()->getDtStart().date()))
    return;
  
  if (cursor) {
    delete cursor;
    cursor = 0L;
  }
  QList<KDPEvent> *tmpList;
  // we have to check tmpList->count(), because sometimes there are
  // empty lists in the dictionary (they had events once, but they
  // have all been deleted from that date)
  if ((tmpList = calDict->find(makeKey(newDate))) && 
      tmpList->count()) {
    cursor = new QListIterator<KDPEvent> (*tmpList);
    cursor->toFirst();
    while (cursor->current() && 
	   (cursor->current() != dEvent)) 
      ++(*cursor);
    if (cursor->current()) {
      return;
    }
  }

  // the little shit is in the recurrence list, or nonexistent.
  if (!recursCursor.current())
    // there's nothing in the recurrence list...
    return;
  if (recursCursor.current()->recursOn(newDate))
    // we are already there...
    return;
  // try to find something in the recurrence list that matches new date.
  recursCursor.toFirst();
  while (recursCursor.current() && 
	 recursCursor.current() != dEvent)
    ++recursCursor;
  if (!recursCursor.current())
    // reset to beginning, no events exist on the new date.
    recursCursor.toFirst();
  return;
}

QString CalObject::qDateToISO(const QDate &qd)
{
  QString tmpStr;

  ASSERT(qd.isValid());

  tmpStr.sprintf("%.2d%.2d%.2d",
		 qd.year(), qd.month(), qd.day());
  return tmpStr;
 
}

QString CalObject::qDateTimeToISO(const QDateTime &qdt, bool zulu)
{
  QString tmpStr;

  ASSERT(qdt.date().isValid());
  ASSERT(qdt.time().isValid());
  if (zulu) {
    QDateTime tmpDT(qdt);
    tmpDT = tmpDT.addSecs(60*(-timeZone)); // correct to GMT.
    tmpStr.sprintf("%.2d%.2d%.2dT%.2d%.2d%.2dZ",
		   tmpDT.date().year(), tmpDT.date().month(), 
		   tmpDT.date().day(), tmpDT.time().hour(), 
		   tmpDT.time().minute(), tmpDT.time().second());
  } else {
    tmpStr.sprintf("%.2d%.2d%.2dT%.2d%.2d%.2d",
		   qdt.date().year(), qdt.date().month(), 
		   qdt.date().day(), qdt.time().hour(), 
		   qdt.time().minute(), qdt.time().second());
  }
  return tmpStr;
}

QDateTime CalObject::ISOToQDateTime(const char *dtStr)
{
  QDate tmpDate;
  QTime tmpTime;
  QString tmpStr;
  int year, month, day, hour, minute, second;

  tmpStr = dtStr;
  year = tmpStr.left(4).toInt();
  month = tmpStr.mid(4,2).toInt();
  day = tmpStr.mid(6,2).toInt();
  hour = tmpStr.mid(9,2).toInt();
  minute = tmpStr.mid(11,2).toInt();
  second = tmpStr.mid(13,2).toInt();
  tmpDate.setYMD(year, month, day);
  tmpTime.setHMS(hour, minute, second);
  
  ASSERT(tmpDate.isValid());
  ASSERT(tmpTime.isValid());
  QDateTime tmpDT(tmpDate, tmpTime);
  // correct for GMT if string is in Zulu format
  if (dtStr[strlen(dtStr)-1] == 'Z')
    tmpDT = tmpDT.addSecs(60*timeZone);
  return tmpDT;
}

// take a raw vcalendar (i.e. from a file on disk, clipboard, etc. etc.
// and break it down from it's tree-like format into the dictionary format
// that is used internally in the CalObject.
void CalObject::populate(VObject *vcal)
{
  // this function will populate the caldict dictionary and other event 
  // lists. It turns vevents into KDPEvents and then inserts them.

  VObjectIterator i;
  VObject *curVO, *curVOProp;
  KDPEvent *anEvent;

  if ((curVO = isAPropertyOf(vcal, ICMethodProp)) != 0) {
    char *methodType = 0;
    methodType = fakeCString(vObjectUStringZValue(curVO));
    if (dialogsOn)
      QMessageBox::information(kapp->mainWidget(), "KOrganizer: iTIP Transaction",
			       QString("This calendar is an iTIP transaction of type \"") +
			       methodType + "\"");
    delete methodType;
  }

  // warn the user that we might have trouble reading non-known calendar.
  if ((curVO = isAPropertyOf(vcal, VCProdIdProp)) != 0) {
    char *s = fakeCString(vObjectUStringZValue(curVO));
    if (strcmp(_PRODUCT_ID, s) != 0)
      if (dialogsOn)
	QMessageBox::warning(kapp->mainWidget(), "KOrganizer: Unknown vCalendar Vendor",
			     QString("This vCalendar file was not created by KOrganizer\n"
				     "or any other product we support.  Loading anyway..."));
    deleteStr(s);
  }
  
  // warn the user we might have trouble reading this unknown version.
  if ((curVO = isAPropertyOf(vcal, VCVersionProp)) != 0) {
    char *s = fakeCString(vObjectUStringZValue(curVO));
    if (strcmp(_VCAL_VERSION, s) != 0)
      if (dialogsOn)
	QMessageBox::warning(kapp->mainWidget(), "KOrganizer: Unknown vCalendar Version",
			     QString("This vCalendar file has version ") +
			     s + ".\n We only support " + _VCAL_VERSION + ".");
    deleteStr(s);
  }
  
  // set the time zone
  if ((curVO = isAPropertyOf(vcal, VCTimeZoneProp)) != 0) {
    char *s = fakeCString(vObjectUStringZValue(curVO));
    setTimeZone(s);
    deleteStr(s);
  }

  initPropIterator(&i, vcal);

  // go through all the vobjects in the vcal
  while (moreIteration(&i)) {
    curVO = nextVObject(&i);

    /************************************************************************/

    // now, check to see that the object is an event or todo.
    if (strcmp(vObjectName(curVO), VCEventProp) == 0) {

      if ((curVOProp = isAPropertyOf(curVO, KPilotStatusProp)) != 0) {
	char *s;
	s = fakeCString(vObjectUStringZValue(curVOProp));
	// check to see if event was deleted by the kpilot conduit
	if (atoi(s) == KDPEvent::SYNCDEL) {
	  deleteStr(s);
	  debug("skipping pilot-deleted event");
	  goto SKIP;
	}
	deleteStr(s);
      }

      // this code checks to see if we are trying to read in an event
      // that we already find to be in the calendar.  If we find this
      // to be the case, we skip the event.
      if ((curVOProp = isAPropertyOf(curVO, VCUniqueStringProp)) != 0) {
	char *s = fakeCString(vObjectUStringZValue(curVOProp));
	QString tmpStr(s);
	deleteStr(s);

	if (getEvent(tmpStr)) {
	  if (dialogsOn)
	    QMessageBox::warning(kapp->mainWidget(), "KOrganizer: Possible Duplicate Event",
				 QString("Aborting merge of an event already in calendar.\n"
					 "UID is ") + tmpStr + "\n" +
				 "Please check your vCalendar file for duplicate UIDs\n" +
				 "and change them MANUALLY to be unique if you find any.\n");
	  goto SKIP;
	}
	if (getTodo(tmpStr)) {
	  if (dialogsOn)
	    QMessageBox::warning(kapp->mainWidget(), "KOrganizer: Possible Duplicate Event",
				 QString("Aborting merge of an event already in calendar.\n"
					 "UID is ") + tmpStr + "\n" +
				 "Please check your vCalendar file for duplicate UIDs\n" +
				 "and change them MANUALLY to be unique if you find any.\n");
	  
	  goto SKIP;
	}
      }

      if ((!(curVOProp = isAPropertyOf(curVO, VCDTstartProp))) &&
	  (!(curVOProp = isAPropertyOf(curVO, VCDTendProp)))) {
	debug("found a VEvent with no DTSTART and no DTEND! Skipping...");
	goto SKIP;
      }

      anEvent = VEventToEvent(curVO);
      // we now use addEvent instead of insertEvent so that the
      // signal/slot get connected.
      if (anEvent)
	addEvent(anEvent);
      else {
	// some sort of error must have occurred while in translation.
	goto SKIP;
      }
    } else if (strcmp(vObjectName(curVO), VCTodoProp) == 0) {
      anEvent = VTodoToEvent(curVO);
      addTodo(anEvent);
    } else if ((strcmp(vObjectName(curVO), VCVersionProp) == 0) ||
	       (strcmp(vObjectName(curVO), VCProdIdProp) == 0) ||
	       (strcmp(vObjectName(curVO), VCTimeZoneProp) == 0)) {
      // do nothing, we know these properties and we want to skip them.
      // we have either already processed them or are ignoring them.
      ;
    } else {
      debug("Ignoring unknown vObject \"%s\"", vObjectName(curVO));
    }
  SKIP:
    ;
  } // while
}

const char *CalObject::dayFromNum(int day)
{
  const char *days[7] = { "MO ", "TU ", "WE ", "TH ", "FR ", "SA ", "SU " };

  return days[day];
}

int CalObject::numFromDay(const QString &day)
{
  if (day == "MO ") return 0;
  if (day == "TU ") return 1;
  if (day == "WE ") return 2;
  if (day == "TH ") return 3;
  if (day == "FR ") return 4;
  if (day == "SA ") return 5;
  if (day == "SU ") return 6;

  return -1; // something bad happened. :)
}

void CalObject::loadError(const QString &fileName) 
{
  if (dialogsOn)
    QMessageBox::critical(kapp->mainWidget(), "KOrganizer: Error Loading Calendar",
			  QString("An error has occurred loading the file:\n") +
			  fileName + ".\nPlease verify that the file is in vCalendar format,\n"
			  "that it exists, and it is readeable, then try again or load another file.\n");
}

void CalObject::parseError(const char *prop) 
{
  if (dialogsOn)
    QMessageBox::critical(kapp->mainWidget(), "KOrganizer: Error Parsing Calendar",
			  QString("An error has occurred parsing this file.\n") +
			  "It is missing property '" +
			  prop + "'.\n" +
			  "Please verify that the file is in vCalendar format\n" +
			  "and try again, or load another file.\n");
}
