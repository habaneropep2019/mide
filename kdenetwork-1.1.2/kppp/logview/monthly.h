/*
 * kPPPlogview: a accounting log system for kPPP
 *
 *            Copyright (C) 1998 Mario Weilguni <mweilguni@kde.org>
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


#ifndef __MONTHLY__H__
#define __MONTHLY__H__

#include "log.h"
#include <qwidget.h>
#include <kapp.h>
#include <qlayout.h>
#include <qlabel.h>
#include <ktablistbox.h>
#include <qpushbt.h>
#include <kbuttonbox.h>

class MonthlyWidget : public QWidget {
  Q_OBJECT
public:
  MonthlyWidget(QWidget *parent = 0);

private slots:
  void prevMonth();
  void nextMonth();
  void currentMonth();

private:
  void layoutWidget();
  void plotMonth();

  int _month, _year;

  KButtonBox *bbox;
  KTabListBox *lb;
  QLabel *title;
  QPushButton *next, *prev, *today;

  QVBoxLayout *tl;
};

#endif
