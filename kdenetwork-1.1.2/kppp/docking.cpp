/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: docking.cpp,v 1.10 1998/12/20 20:45:56 porten Exp $
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

#include <qtooltip.h>
#include <qdir.h>
#include <kwm.h>
#include <kapp.h>

#include "docking.h"
#include "main.h"
#include "kpppconfig.h"

extern KPPPWidget   *p_kppp;

extern bool do_stats();
extern bool init_stats();

// static member
DockWidget *DockWidget::dock_widget = 0;

DockWidget::DockWidget(const char *name)
  : QWidget(0, name, 0) {

  docked = false;

  QString pixdir = KApplication::kde_datadir() + "/kppp/pics/";
  QString tmp;

#define PMERROR(pm) \
  tmp.sprintf(i18n("Could not load %s !"), pm); \
  QMessageBox::warning(this, i18n("Error"), tmp);

  // load pixmaps
  if (!dock_none_pixmap.load(pixdir + "dock_none.xpm")){
    PMERROR("dock_none.xpm");
  }
  if (!dock_left_pixmap.load(pixdir + "dock_left.xpm")){
    PMERROR("dock_left.xpm");
  }
  if (!dock_right_pixmap.load(pixdir + "dock_right.xpm")){
    PMERROR("dock_right.xpm");
  }
  if (!dock_both_pixmap.load(pixdir + "dock_both.xpm")){
    PMERROR("dock_both.xpm");
  }

  // popup menu for right mouse button
  popup_m = new QPopupMenu();
  toggleID = popup_m->insertItem(i18n("Restore"),
				 this, SLOT(toggle_window_state()));
  popup_m->insertItem(i18n("Details"), this, SLOT(show_stats()));
  popup_m->insertSeparator();
  popup_m->insertItem(i18n("Disconnect"),
		      this, SLOT(disconnect()));

  // timer for little modem animation
  clocktimer = new QTimer(this);
  connect(clocktimer, SIGNAL(timeout()), SLOT(timeclick()));

  DockWidget::dock_widget = this;
}


DockWidget::~DockWidget() {
  clocktimer->stop();
  DockWidget::dock_widget = 0;
}


void DockWidget::dock() {
  if (!docked) {
    // prepare panel to accept this widget
    KWM::setDockWindow (this->winId());

    // that's all the space there is
    this->setFixedSize(24, 24);

    // finally dock the widget
    this->show();

    docked = true;
  }
}


void DockWidget::undock() {
  if (docked) {
    // the widget's window has to be destroyed in order
    // to undock from the panel. Simply using hide() is
    // not enough (seems to be necessary though).
    hide();
    destroy(true, true);

    // recreate window for further dockings
    this->create(0, true, false);

    docked = false;
  }
}


const bool DockWidget::isDocked() {
  return docked;
}


void DockWidget::paintEvent (QPaintEvent *) {
  paintIcon();
}


void DockWidget::paintIcon () {
  // animate modem lights

  if((ibytes_last != ibytes) && (obytes_last != obytes)) {
    bitBlt(this, 0, 0, &dock_both_pixmap);
    ibytes_last = ibytes;
    obytes_last = obytes;
    return;
  }

  if(ibytes_last != ibytes) {
    bitBlt(this, 0, 0, &dock_left_pixmap);
    ibytes_last = ibytes;
    obytes_last = obytes;
    return;
  }

  if(obytes_last != obytes) {
    bitBlt(this, 0, 0, &dock_right_pixmap);
    ibytes_last = ibytes;
    obytes_last = obytes;
    return;
  }

  bitBlt(this, 0, 0, &dock_none_pixmap);
  ibytes_last = ibytes;
  obytes_last = obytes;
}


void DockWidget::timeclick() {
  if(this->isVisible()){
    (void) do_stats();
    paintIcon();
  }
}


void DockWidget::take_stats() {
  if (docked) {
    init_stats();
    clocktimer->start(PPP_STATS_INTERVAL);
  }
}


void DockWidget::stop_stats() {
  clocktimer->stop();
}


void DockWidget::mousePressEvent(QMouseEvent *e) {
  // open/close connect-window on right mouse button
  if ( e->button() == LeftButton ) {
    toggle_window_state();
  }

  // open popup menu on left mouse button
  if ( e->button() == RightButton ) {
    int x = e->x() + this->x();
    int y = e->y() + this->y();

    QString text;
    if(p_kppp->con_win->isVisible())
      text = i18n("Minimize");
    else
      text = i18n("Restore");

    popup_m->changeItem(text, toggleID);
    popup_m->popup(QPoint(x, y));
    popup_m->exec();
  }

}


void DockWidget::toggle_window_state() {
  // restore/hide connect-window
  if(p_kppp != 0L)  {
    if (p_kppp->con_win->isVisible())
      p_kppp->con_win->hide();
    else {
      p_kppp->con_win->show();
      KWM::activate(p_kppp->con_win->winId());
    }
  }
}


void DockWidget::show_stats() {
  // show statistics
  if(p_kppp != 0L) {
    p_kppp->stats->show();
  }
}


void DockWidget::disconnect() {
  // close ppp-connection
  if(p_kppp != 0L) {
    emit p_kppp->disconnect();
  }
}


#include "docking.moc"
