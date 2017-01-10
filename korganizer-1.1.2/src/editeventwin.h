#ifndef _EDITEVENTWIN_H
#define _EDITEVENTWIN_H

// 	$Id: editeventwin.h,v 1.32.2.1 1999/03/12 22:26:04 pbrown Exp $	
#include <ktabctl.h>
#include <ktopwidget.h>
#include <klined.h>

#include <qmlined.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qdatetm.h>

#include "calobject.h"
#include "eventwingeneral.h"
#include "eventwindetails.h"
#include "eventwinrecur.h"
#include "catdlg.h"
#include "kptbutton.h"
#include "eventwin.h"

/**
  * This is the class to add/edit a new appointment.
  *
  * @short Creates a dialog box to create/edit an appointment
  * @author Preston Brown
  * @version $Revision: 1.32.2.1 $
  */
class EditEventWin : public EventWin
{
  Q_OBJECT

public:
/**
  * Constructs a new appointment dialog.
  *
  */  
  EditEventWin( CalObject *calendar);
  virtual ~EditEventWin(void);

  //  enum { EVENTADDED, EVENTEDITED, EVENTDELETED };

public slots:

      
/** Clear eventwin for new event, and preset the dates and times with hint */
  void newEvent( QDateTime from, QDateTime to, bool allDay = FALSE );

/** Edit an existing event. */
  void editEvent( KDPEvent *, QDate qd=QDate::currentDate());

/** cancel is public so TopWidget can call it to close the dialog. */
//  void cancel();

signals:
  void eventChanged(KDPEvent *, int);
  void closed(QWidget *);

protected slots:
  KDPEvent* makeEvent();
  void updateCatEdit(QString _str);
  void recurStuffEnable(bool enable);
  void startTimeChanged(QTime newt, int wrapval);
  void endTimeChanged(QTime newt, int wrapval);
  void startDateChanged(QDate);
  void endDateChanged(QDate);
  void prevEvent();
  void nextEvent();
  void deleteEvent();
  

protected:
  // methods
  
  void initConstructor();
  void fillInFields(QDate qd);
  void fillInDefaults(QDateTime from, QDateTime to, bool allDay);
  void closeEvent(QCloseEvent *);
  void setDuration();
  // temporary variables that hold changes until it is time to commit.
  QDateTime  currStartDateTime;
  QDateTime  currEndDateTime;

};

#endif


