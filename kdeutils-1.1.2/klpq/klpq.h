// -*- C++ -*-

//
//  klpq
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef klpq_included
#define klpq_included

#include <qapp.h>
#include <qwidget.h>
#include <qstring.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qmenubar.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qaccel.h>

#include <kapp.h>
#include <ktopwidget.h>

#include "ConfAutoUpdate.h"
#include "SpoolerConfig.h"

class KProcess;
class KMenuBar;
class KFM;
class KHeader;
class MyRowTable;
class Spooler;
class KDNDropZone;
class QStrList;
class QPopupMenu;

#define KLPQ_VERSION "1.0"

class Klpq : public KTopLevelWidget
{
  Q_OBJECT;
public:
  Klpq(const char* name=NULL);
  ~Klpq();

  void addPrintQueue( QString name ) { cb_printer->insertItem(name); }
  void setLastPrinterCurrent();
  void callUpdate() { update(); }
  void printRemote( QStrList file_list );

protected slots:
  void startHelp();
  void update();
  void setAuto();
  void printerSelect(int) { update(); }
  void queuing();
  void printing();
  void remove();
  void makeTop();
  void prevPrinter();
  void nextPrinter();
  void configureAuto();
  void configureSpooler();
  void recvLpq( KProcess *, char *buffer, int buflen );
  void exitedLpq(KProcess *) { lpq_running = FALSE; updateList(); enable(); }
  void recvLpc( KProcess *, char *buffer, int buflen );
  void exitedLpc(KProcess *);
  void urlDroped(KDNDDropZone *);
  void slotKFMJobDone();
  void popupMenu();

protected:
  enum LpcCommand { NotRunning = 0, Status, Queuing, Printing, MakeTop };
  void enable();
  void disable();
  void updateList();

  KMenuBar    *menubar;
  QFrame      *f_main;
  QFrame      *f_top;
  QComboBox   *cb_printer;
  QLabel      *l_printer;
  QCheckBox   *c_queuing;
  QCheckBox   *c_printing;
  QPushButton *b_update;
  QPushButton *b_quit;
  QFrame      *f_list;
  KHeader     *h_list;
  MyRowTable  *lb_list;
  QLabel      *l_list;
  QFrame      *f_bottom;
  QPushButton *b_remove;
  QPushButton *b_make_top;
  QListBox    *lb_status;
  QPushButton *b_auto;
  QPopupMenu  *popup_menu;

  QGridLayout *top2bottom;
  QBoxLayout  *top_layout;
  QGridLayout *list_layout;
  QGridLayout *bottom_layout;

  QAccel      *accel;

  KProcess    *lpc;
  KProcess    *lpq;
  Spooler     *spooler;
  LpcCommand   lpc_com;
  bool         lpq_running;
  QString      lpq_in_buffer;

  ConfAutoUpdate *conf_auto;
  SpoolerConfig  *conf_spooler;
  int          update_delay;

  QStrList     remote_files;
  KFM         *kfm;

};

#endif /* klpq_included */


