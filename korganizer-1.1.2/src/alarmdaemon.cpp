// Alarm Daemon for KOrganizer
// (c) 1998 Preston Brown
// 	$Id: alarmdaemon.cpp,v 1.24.2.3 1999/04/19 03:44:32 pbrown Exp $	

#include "config.h"

#include <qtimer.h>
#include <qdatetm.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qtooltip.h>
#include <kapp.h>
#include <kwm.h>
#include <kprocess.h>
#include <mediatool.h>
#include <kaudio.h>
#include <kprocess.h>
#include <unistd.h>
#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif
#include "alarmdaemon.h"
#include "alarmdaemon.moc"
#ifndef HAVE_BASENAME
#include "basename.c"
#endif

DockWidget::DockWidget(const char *name)
  : QWidget(0, name, 0) 
{
  docked = FALSE;
  KWM::setDecoration(winId(), KWM::tinyDecoration | KWM::noFocus);

  QString pixdir = kapp->kde_icondir() + "/mini/";
  QString tmpStr;

  if (!dPixmap1.load(pixdir + "alarmd.xpm")) {
    QMessageBox::warning(this, i18n("KOrganizer Error"),
			 i18n("Can't load docking tray icon!"));
  }
  
  if (!dPixmap2.load(pixdir + "alarmd-disabled.xpm")) {
    QMessageBox::warning(this, i18n("KOrganizer Error"),
			 i18n("Can't load docking tray icon!"));
  }

  popupMenu = new QPopupMenu();
  itemId = popupMenu->insertItem(i18n("Alarms Enabled"), this,
				 SLOT(toggleAlarmsEnabled()));
  popupMenu->setItemChecked(itemId, TRUE);
  popupMenu->insertItem(i18n("Exit Applet"), kapp, 
			SLOT(quit()));

  QToolTip::add(this, i18n("KOrganizer Control Applet"));
}

DockWidget::~DockWidget() 
{
}

void DockWidget::dock()
{
  if (!docked) {
    
    // prepare panel for docking
    KWM::setDockWindow(this->winId());

    this->setFixedSize(22, 22);

    this->show();
    docked = TRUE;
  }
}

void DockWidget::undock()
{
  if (docked) {
    this->destroy(TRUE, TRUE);

    this->create(0, TRUE, FALSE);
    docked = FALSE;
  }
}

void DockWidget::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == RightButton) {
    QPoint pt = this->mapToGlobal(QPoint(0, 0));
    pt = pt - QPoint(30, 30);
    popupMenu->popup(pt);
    popupMenu->exec();
  }
  if (e->button() == LeftButton) {
    // start up a korganizer.
    KProcess proc;
    proc << "korganizer";
    proc.start(KProcess::DontCare);
  }
}

///////////////////////////////////////////////////////////////////////////////

AlarmDaemon::AlarmDaemon(const char *fn, QObject *parent, const char *name) 
  : QObject(parent, name)
{
  docker = new DockWidget;
  docker->move(-2000,-2000);
  calendar = new CalObject;
  calendar->showDialogs(FALSE);
  fileName = fn;

  calendar->load(fileName);
  
  // set up the alarm timer
  QTimer *alarmTimer = new QTimer(this);
  
  connect(alarmTimer, SIGNAL(timeout()),
	  calendar, SLOT(checkAlarms()));
  connect(calendar, SIGNAL(alarmSignal(QList<KDPEvent> &)),
    this, SLOT(showAlarms(QList<KDPEvent> &)));
  
  alarmTimer->start(1000*60); // every minute
}

AlarmDaemon::~AlarmDaemon()
{
  calendar->close();
  delete calendar;
}

void AlarmDaemon::reloadCal()
{
  QString cfgPath = KApplication::localconfigdir();
  cfgPath += "/korganizerrc";
  KConfig *config = new KConfig(cfgPath.data());

  calendar->close();
  config->setGroup("General");
  newFileName = config->readEntry("Current Calendar");
  delete config;
  calendar->load(newFileName.data());
}

void AlarmDaemon::showAlarms(QList<KDPEvent> &alarmEvents)
{
  KDPEvent *anEvent;
  QString messageStr, titleStr;
  QDateTime tmpDT;

  // leave immediately if alarms are off
  if (!docker->alarmsOn())
    return;

  tmpDT = alarmEvents.first()->getAlarmTime();

  titleStr.sprintf(i18n("KOrganizer Alarm: %s\n"),tmpDT.toString().data());
  messageStr += i18n("The following events triggered alarms:\n\n");

  for (anEvent = alarmEvents.first(); anEvent;
       anEvent = alarmEvents.next()) {
    messageStr += anEvent->getSummary() + "\n";
    if (!anEvent->getProgramAlarmFile().isEmpty()) {
      KProcess proc;
      proc << anEvent->getProgramAlarmFile().data();
      proc.start(KProcess::DontCare);
    }
  
    if (!anEvent->getAudioAlarmFile().isEmpty()) {
      KAudio audio;
      audio.play(anEvent->getAudioAlarmFile().data());
    } else {
      qApp->beep();
    }
  }

  QMessageBox::information(0, titleStr.data(), 
			   messageStr.data(),
			   QMessageBox::Ok | QMessageBox::Default);
}



/*AlarmWindow::AlarmWindow()
  : QDialog(0, "alarmWindow", TRUE)
{
  QPushButton *okButton;
  QVBoxLayout *layout;
  QFrame *hLine;

  layout = new QVBoxLayout(this, 10);

  okButton = new QPushButton(this);
  okButton->setText(i18n("OK"));
  
}*/

