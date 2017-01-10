/*   $Id: todoeventwin.cpp,v 1.6.2.3 1999/03/28 02:44:41 glenebob Exp $

     Requires the Qt and KDE widget libraries, available at no cost at
     http://www.troll.no and http://www.kde.org respectively

     Copyright (C) 1997, 1998 Preston Brown
     preston.brown@yale.edu

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

     -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

     This file implements a class for displaying a dialog box for
     adding or editing appointments/events.

*/

#include <qtooltip.h>
#include <qframe.h>
#include <qpixmap.h>
#include <kdatepik.h>
#include <kiconloader.h>
#include <kapp.h>
#include <stdio.h>

#include "qdatelist.h"
#include "misc.h"
#include "todowingeneral.h"
#include "todoeventwin.moc"

TodoEventWin::TodoEventWin(CalObject *cal)
  : EventWin(cal)
{
  initConstructor();
  fillInDefaults(QDateTime::currentDateTime(),
                 QDateTime::currentDateTime(),
                 FALSE);
  // anyhow, it is never called like this
  prevNextUsed = FALSE;
  Type = TYPE_TODO;

}

void TodoEventWin::editEvent( KDPEvent *event, QDate qd )
{
  // check to see if save & close should be enabled or not
  saveExit->setEnabled(! event->isReadOnly());
  btnDelete->setEnabled(true);

  currEvent = event;
  addFlag = FALSE;
  fillInFields(qd);

  unsetModified();
  setTitle();
}

void TodoEventWin::newEvent( QDateTime from, QDateTime to, bool allDay )
{
  saveExit->setEnabled(true);
  btnDelete->setEnabled(false);

  currEvent = 0L;
  addFlag = true;
  fillInDefaults( from, to, allDay );

  unsetModified();
  setTitle();
}

void TodoEventWin::initConstructor()
{
/*
  initToolBar();
  m_frame = new QFrame(this);
  tabCtl = new KTabCtl(m_frame);
  CHECK_PTR(tabCtl);
  */
  QString tmpStr;

  CHECK_PTR(tabCtl);
  General = new TodoWinGeneral(m_frame, "General");
  CHECK_PTR(General);
  connect(General, SIGNAL(modifiedEvent()),
    this, SLOT(setModified()));

  Details = new EventWinDetails(m_frame, "Details", true);
  Details->lower();
  Details->hide();
  CHECK_PTR(Details);
  connect(Details, SIGNAL(modifiedEvent()),
    this, SLOT(setModified()));

  setFrameBorderWidth(0);

  tabCtl->addTab(General, i18n("General"));  
  tabCtl->addTab(Details, i18n("Details"));

  setView(m_frame);
  // how do you resize the view to fit everything by default??
  resize(605, 455);
  
  // this is temporary until we can have the thing resize properly.
  setFixedWidth(605);
  setFixedHeight(455);

  // signal/slot stuff

  // category views on General and Details tab are synchronized
  catDlg = new CategoryDialog();
  connect(General->categoriesButton, SIGNAL(clicked()), 
	  catDlg, SLOT(show()));
  connect(Details->categoriesButton, SIGNAL(clicked()),
	  catDlg, SLOT(show()));
  connect(catDlg, SIGNAL(okClicked(QString)), 
	  this, SLOT(updateCatEdit(QString)));

  unsetModified();
}

TodoEventWin::~TodoEventWin()
{
  // delete NON-CHILDREN widgets
    /*
  delete catDlg;
  delete m_frame;
    */
}

/*************************************************************************/
/********************** PROTECTED METHODS ********************************/
/*************************************************************************/

void TodoEventWin::fillInFields(QDate qd)
{
  QString tmpStr;
  QDateTime tmpDT;
  QBitArray rDays;
  QList<KDPEvent::rMonthPos> rmp;
  QList<int> rmd;
  int i;
  bool ok;

  blockSignals(true);

  /******************** GENERAL *******************************/
  General->summaryEdit->setText(currEvent->getSummary().data());
  General->descriptionEdit->setText(currEvent->getDescription().data());
  // organizer information
  General->ownerLabel->setText(tmpStr.sprintf(i18n("Owner: %s"),
                                              currEvent->getOrganizer().data()).data());
  if (currEvent->getStatusStr() == "NEEDS ACTION")
      General->completedButton->setChecked(FALSE);
  else
      General->completedButton->setChecked(TRUE);
  General->priorityCombo->setCurrentItem(currEvent->getPriority()-1);

  blockSignals(false);
}

void TodoEventWin::fillInDefaults(QDateTime from, 
				  QDateTime to, bool allDay)
{
  QString tmpStr;


  blockSignals(true);

  // default owner is calendar owner.
  General->ownerLabel->setText(tmpStr.sprintf(i18n("Owner: %s"),
					      calendar->getEmail().data()).data());

  blockSignals(false);
}

void TodoEventWin::closeEvent(QCloseEvent *)
{
  // we clean up after ourselves...
  emit closed(this);
}

/*************************************************************************/
/********************** PROTECTED SLOTS **********************************/
/*************************************************************************/

// used to make a new event / update an old event

KDPEvent* TodoEventWin::makeEvent()
{
    
  KDPEvent *anEvent;
  anEvent = EventWin::makeEvent();
  
  currEvent->setPriority(General->priorityCombo->currentItem()+1);
  if (General->completedButton->isChecked()) {
      anEvent->setStatus(QString("COMPLETED"));
  } else {
      anEvent->setStatus(QString("NEEDS ACTION"));
  }

  emit eventChanged(anEvent, EVENTEDITED);
  return anEvent;
}

void TodoEventWin::updateCatEdit(QString _str)
{
  General->categoriesLabel->setText(_str.data());
  Details->categoriesLabel->setText(_str.data());

  setModified();
}

void TodoEventWin::setDuration()
{
  setModified();
}

/*****************************************************************************/
void TodoEventWin::prevEvent()
{
}

void TodoEventWin::nextEvent()
{
}

void TodoEventWin::deleteEvent()
{
  if (!addFlag) {
      calendar->deleteTodo(currEvent);
  }
  emit eventChanged(currEvent, EVENTDELETED);
  close();
}

