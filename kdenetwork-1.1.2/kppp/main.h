/* 
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: main.h,v 1.22.2.1 1999/08/26 16:13:25 porten Exp $
 * 
 *            Copyright (C) 1997 Bernd Johannes Wuebben 
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
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


#ifndef _MAIN_H_
#define _MAIN_H_

// this include is needed since gcc sometimes gets 
// confused if qdir.h is included later (strange error)
#include <qdir.h>


#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <qapp.h>
#include <qchkbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qpushbt.h>
#include <qcombo.h>
#include <qtimer.h>
#include <qlcdnum.h>
#include <qpainter.h>
#include <qtabdlg.h>
#include <qradiobt.h>
#include <qchkbox.h> 
#include <qpixmap.h> 
#include <qchkbox.h>

#include <kapp.h>
#include <kwm.h>

#include <kconfig.h>

#include "accounting.h"


#include "conwindow.h"
#include "general.h"
#include "accounts.h"
#include "connect.h"
#include "debug.h"
#include "pppstatdlg.h"

class KPPPWidget : public QWidget {

Q_OBJECT

public:

  KPPPWidget( QWidget *parent=0, const char *name=0 );
  ~KPPPWidget() {}

  friend void dieppp(int);              // if the pppd daemon dies...
  friend void sigint(int);
  friend void sigchld(int);

  void setPW_Edit(const char *);

private slots:
  void newdefaultaccount(int);
  void expandbutton();
  void connectbutton();
  void quitbutton();
  void helpbutton();
  void setup();
  void rulesetLoadError();
  void usernameChanged( const char *);
  void passwordChanged( const char *);
  void enterPressedInID();
  void enterPressedInPW();

public slots:
  void resetaccounts();
  void resetCosts(const char *);
  void disconnect();
  void log_window_toggled(bool on);
  void startAccounting();
  void stopAccounting();

signals:
  void begin_connect();
  void cmdl_start();

public:
  QCheckBox *log;
  bool connected;
  DebugWidget *debugwindow;
  QString con_speed;
  ConnectWidget *con;
  ConWindow *con_win;
  PPPStatsDlg *stats;
  AccountingBase *acct;
  QPushButton *quit_b;

private:
  void prepareSetupDialog();

  QString ruleset_load_errmsg;

  QPushButton *help_b;
  QPushButton *setup_b;
  QFrame *fline;
  QFrame *fline1;
  QPushButton *connect_b;
  QComboBox *connectto_c;
  QLabel *ID_Label;
  QLabel *PW_Label;
  QLineEdit *ID_Edit;
  QLineEdit *PW_Edit;
  QLabel *label1;
  QLabel *label2;  
  QLabel *label3;
  QLabel *label4;
  QLabel *label5;
  QLabel *label6;
  QLabel *radio_label;


  QTabDialog *tabWindow;
  AccountWidget *accounts;
  GeneralWidget *general;
  ModemWidget *modem1;
  ModemWidget2 *modem2;
  GraphSetup *graph;
  AboutWidget *about;
};

class PasswordWidget : public QLineEdit {
public:
  PasswordWidget(QWidget *parent = 0, const char *name = 0);
protected:
  void leaveEvent(QEvent *) {}
  void focusOutEvent(QFocusEvent*);
};

void killpppd();
void sigint(int);
void dieppp(int);
void sigchld(int);
pid_t execute_command(const char *);
void make_directories();
pid_t create_pidfile();
bool remove_pidfile();
void shutDown(int);

#endif
