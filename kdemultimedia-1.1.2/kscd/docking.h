/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id$
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
    void play_pause();
    void forward();
    void backward();
    void next();
    void prev();
    void eject();
    void stop();
    void mousePressEvent(QMouseEvent *e);

public slots:
    void dock();
    void undock();
    void paintIcon();
    void toggle_window_state();

public:
    const bool isDocked();
    const bool isToggled();
    const bool setToggled(int);
    

private:
  bool docked;
  int toggleID;
  QPopupMenu *popup_m;
  QPixmap cdsmall_pixmap;
  bool toggled;

};

#endif



