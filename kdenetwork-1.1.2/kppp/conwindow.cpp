/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: conwindow.cpp,v 1.16.2.2 1999/06/09 20:00:45 porten Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include "conwindow.h"
#include "main.h"
#include "macros.h"

extern KPPPWidget *p_kppp;
extern PPPData gpppdata;
extern int totalbytes;

ConWindow::ConWindow(QWidget *parent, const char *name,QWidget *mainwidget)
  : QWidget(parent, name,0 )
{
  main = mainwidget;
  minutes = 0;
  seconds = 0;
  hours = 0;
  days = 0;

  info1 = new QLabel(this,"infolabel1");
  info1->setText(i18n("Connected at:"));

  info2 = new QLabel(this,"infolabel");
  info2->setText("");

  timelabel1 = new QLabel(this,"timelabel1");
  timelabel1->setText(i18n("Time connected:"));

  timelabel2 = new QLabel(this,"timelabel");
  timelabel2->setText("000:00:00");

  vollabel = new QLabel(i18n("Volume:"), this);
  volinfo  = new QLabel("", this);

  // now the stuff for accounting
  session_bill_l = new QLabel(i18n("Session Bill:"), this);
  session_bill = new QLabel("", this);
  total_bill_l = new QLabel(i18n("Total Bill:"), this);
  total_bill = new QLabel("", this);

  this->setCaption("kppp");

  cancelbutton = new QPushButton(this,"cancelbutton");
  cancelbutton->setText(i18n("Disconnect"));
  connect(cancelbutton, SIGNAL(clicked()), main, SLOT(disconnect()));

  statsbutton = new QPushButton(this,"statsbutton");
  statsbutton->setText(i18n("Details"));
  statsbutton->setFocus();
  connect(statsbutton, SIGNAL(clicked()), this,SLOT(stats()));

  clocktimer = new QTimer(this);
  connect(clocktimer, SIGNAL(timeout()), SLOT(timeclick()));
  tl1 = 0;
}

ConWindow::~ConWindow() {
  stopClock();
}


void ConWindow::accounting(bool on) {
  // delete old layout
  if(tl1 != 0)
    delete tl1;

  // add layout now
  tl1 = new QVBoxLayout(this, 10, 10);
  tl1->addSpacing(5);
  QHBoxLayout *tl = new QHBoxLayout;
  tl1->addLayout(tl);
  tl->addSpacing(20);
  QGridLayout *l1;

  int vol_lines = 0;
  if(gpppdata.VolAcctEnabled())
    vol_lines = 1;

  if(on)
    l1 = new QGridLayout(4 + vol_lines, 2, 5);
  else
    l1 = new QGridLayout(2 + vol_lines, 2, 5);
  tl->addLayout(l1);
  l1->setColStretch(0, 0);
  l1->setColStretch(1, 1);

  MIN_SIZE(info1);
  MIN_SIZE(timelabel1);
  MIN_SIZE(session_bill_l);
  MIN_SIZE(total_bill_l);
  MIN_SIZE(info2);
  MIN_SIZE(timelabel2);
  MIN_SIZE(vollabel);

  info2->setAlignment(AlignRight|AlignVCenter);
  timelabel2->setAlignment(AlignRight|AlignVCenter);
  session_bill->setAlignment(AlignRight|AlignVCenter);
  total_bill->setAlignment(AlignRight|AlignVCenter);
  volinfo->setAlignment(AlignRight|AlignVCenter);

  // make sure that there's enough space for the bills
  QString s1 = session_bill->text();
  QString s2 = total_bill->text();
  QString s3 = volinfo->text();
  session_bill->setText("888888.88 XXX");
  total_bill->setText("888888.88 XXX");
  volinfo->setText("8888.8 MB");
  MIN_SIZE(session_bill);
  MIN_SIZE(total_bill);
  MIN_SIZE(volinfo);
  session_bill->setText(s1.data());
  total_bill->setText(s2.data());
  volinfo->setText(s3.data());

  l1->addWidget(info1, 0, 0);
  l1->addWidget(info2, 0, 1);
  l1->addWidget(timelabel1, 1, 0);
  l1->addWidget(timelabel2, 1, 1);
  if(on) {
    session_bill_l->show();
    session_bill->show();
    total_bill_l->show();
    total_bill->show();
    l1->addWidget(session_bill_l, 2, 0);
    l1->addWidget(session_bill, 2, 1);
    l1->addWidget(total_bill_l, 3, 0);
    l1->addWidget(total_bill, 3, 1);

    if(gpppdata.VolAcctEnabled()) {
      vollabel->show();
      volinfo->show();
      l1->addWidget(vollabel, 4, 0);
      l1->addWidget(volinfo, 4, 1);
    } else {
      vollabel->hide();
      volinfo->hide();
    }

  } else {
    session_bill_l->hide();
    session_bill->hide();
    total_bill_l->hide();
    total_bill->hide();

    if(gpppdata.VolAcctEnabled()) {
      vollabel->show();
      volinfo->show();
      l1->addWidget(vollabel, 2, 0);
      l1->addWidget(volinfo, 2, 1);
    } else {
      vollabel->hide();
      volinfo->hide();
    }
  }

  tl->addSpacing(10);
  QVBoxLayout *l2 = new QVBoxLayout(5);
  tl->addLayout(l2);
  l2->addStretch(1);
  MIN_WIDTH(cancelbutton);
  FIXED_HEIGHT(cancelbutton);
  MIN_WIDTH(statsbutton);
  FIXED_HEIGHT(statsbutton);
  l2->addWidget(statsbutton);
  l2->addWidget(cancelbutton);

  l2->addStretch(1);

  tl1->addSpacing(5);

  tl1->freeze();
  setGeometry((QApplication::desktop()->width() - width()) / 2,
	      (QApplication::desktop()->height() - height())/2,
	      width(),
	      height());
}


void ConWindow::stats() {
  p_kppp->stats->show();
}


void ConWindow::dock() {
  DockWidget::dock_widget->dock();
  this->hide();
}


void ConWindow::startClock() {
  minutes = 0;
  seconds = 0;
  hours = 0;
  QString title ;

  title = gpppdata.accname();

  if(gpppdata.get_show_clock_on_caption()){
    title += " 00:00" ;
  }
  this->setCaption(title);

  timelabel2->setText("00:00:00");
  clocktimer->start(1000);
}


void ConWindow::setConnectionSpeed(){
  if(p_kppp)
    info2->setText(p_kppp->con_speed);
}


void ConWindow::stopClock() {
  clocktimer->stop();
}


void ConWindow::timeclick() {
  // volume accounting
  if(gpppdata.VolAcctEnabled()) {
    QString s;
    if(totalbytes < 1024*10)
      s.sprintf("%d Byte", totalbytes);
    else if(totalbytes < 1024*1024)
      s.sprintf("%0.1f KB", ((float)totalbytes)/1024);
    else
      s.sprintf("%0.1f MB", ((float)totalbytes)/(1024*1024));

    volinfo->setText(s.data());
  }

  seconds++;

  if(seconds >= 60 ) {
    minutes ++;
    seconds = 0;
  }

  if (minutes >= 60){
    minutes = 0;
    hours ++;
  }

  if( hours >= 24){
    days ++;
    hours = 0;
  }

  time_string = "";
  time_string.sprintf("%02d:%02d",hours,minutes);
  time_string2 = "";
  if (days)
      time_string2.sprintf("%d d %02d:%02d:%02d",
			   days,hours,minutes,seconds);

  else
    time_string2.sprintf("%02d:%02d:%02d",hours,minutes,seconds);  

  caption_string = gpppdata.accname();
  caption_string += " ";
  caption_string += time_string;


  timelabel2->setText(time_string2);

  if(gpppdata.get_show_clock_on_caption() && (seconds == 1)){
    // we update the Caption only once per minute not every second
    // otherwise I get a flickering icon
    this->setCaption(caption_string);
  }
}


void ConWindow::closeEvent( QCloseEvent *e ){
  // we don't want to lose the
  // conwindow since this is our last connection kppp.
  // if we lost it we could only kill the program by hand to get on with life.
  e->ignore();

  if(gpppdata.get_dock_into_panel())
    dock();
}


void ConWindow::slotAccounting(QString total, QString session) {
  total_bill->setText(total.data());
  session_bill->setText(session.data());
}

#include "conwindow.moc"
