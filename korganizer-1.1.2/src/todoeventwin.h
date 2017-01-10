#ifndef _APPTDLG_H
#define _APPTDLG_H

// 	$Id: todoeventwin.h,v 1.4.2.1 1999/03/12 22:26:35 pbrown Exp $	
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
  * @version $Revision: 1.4.2.1 $
  */
class TodoEventWin : public EventWin
{
  Q_OBJECT

public:
/**
  * Constructs a new appointment dialog.
  *
  */  
  TodoEventWin( CalObject *calendar);
  virtual ~TodoEventWin(void);

//  enum { EVENTADDED, EVENTEDITED, EVENTDELETED };

public slots:

/** Clear eventwin for new event, and preset the dates and times with hint */
  void newEvent( QDateTime from, QDateTime to, bool allDay = FALSE );

/** Edit an existing event. */
  void editEvent( KDPEvent *, QDate qd=QDate::currentDate());

signals:
  void eventChanged(KDPEvent *, int);
  void closed(QWidget *);

protected slots:
  KDPEvent* makeEvent();
  void updateCatEdit(QString _str);
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
};

#endif


