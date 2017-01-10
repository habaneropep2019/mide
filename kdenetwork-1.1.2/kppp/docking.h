/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: docking.h,v 1.3 1998/10/25 19:22:28 mario Exp $
 *
 *              Copyright (C) 1997 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 *
 * This file was contributed by Harri Porten <porten@tu-harburg.de>
 *
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


#ifndef _DOCKING_H_
#define _DOCKING_H_

#include <stdio.h>
#include <qapp.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qpopmenu.h>
#include <qpoint.h>

extern int 	ibytes;
extern int 	obytes;

class DockWidget : public QWidget {
  Q_OBJECT
public:
  DockWidget(const char *name=0);
  ~DockWidget();

protected:
  void paintEvent(QPaintEvent *e);

private slots:
  void timeclick();
  void toggle_window_state();
  void show_stats();
  void disconnect();
  void mousePressEvent(QMouseEvent *e);

public slots:
  void dock();
  void undock();
  void paintIcon();
  void take_stats();
  void stop_stats();

public:
  const bool isDocked();
  static DockWidget *dock_widget;

private:
  bool docked;
  int toggleID;
  int ibytes_last;
  int obytes_last;

  QPopupMenu *popup_m;
  QTimer     *clocktimer;

  QPixmap dock_none_pixmap;
  QPixmap dock_left_pixmap;
  QPixmap dock_right_pixmap;
  QPixmap dock_both_pixmap;
};

#endif
