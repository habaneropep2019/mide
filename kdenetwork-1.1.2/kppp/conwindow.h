/* -*- C++ -*-
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: conwindow.h,v 1.7.2.1 1999/06/09 20:00:46 porten Exp $
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

#ifndef _CONWINDOW_H_
#define _CONWINDOW_H_


#include <qtimer.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qframe.h>
#include <qevent.h>
#include <qlayout.h>


class ConWindow : public QWidget {
Q_OBJECT

public:
  ConWindow(QWidget *parent=0, const char *name=0,QWidget *main=0);
  ~ConWindow();

protected:
//  void timerEvent(QTimerEvent *);

  void closeEvent( QCloseEvent *e );

private slots:

//void cancelslot();

  void timeclick();
  void stats();
  void dock();

public:
  void setConnectionSpeed();
  void startClock();
  void stopClock();
  void accounting(bool); // show/ hide accounting info

public slots:
  void slotAccounting(QString, QString);

private:

  QWidget *main;
  QLabel *info1;
  QLabel *info2;
  QPushButton *cancelbutton;
  QPushButton *statsbutton;

  int minutes;
  int seconds;
  int hours;
  int days;
  QFrame *fline;
  QLabel *timelabel1;
  QLabel *timelabel2;
  QLabel *total_bill, *total_bill_l;
  QLabel *session_bill, *session_bill_l;
  QString caption_string;
  QString time_string2;
  QString time_string;
  QTimer *clocktimer;
  QVBoxLayout *tl1;
  QLabel *vollabel;
  QLabel *volinfo;
};


#endif





