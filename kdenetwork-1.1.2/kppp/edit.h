/* -*- C++ -*-
 *
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: edit.h,v 1.13 1998/12/26 10:10:33 mario Exp $
 *              Copyright (C) 1997 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef _EDIT_H_
#define _EDIT_H_

#include <qdialog.h>
#include <qpushbt.h>
#include <qgrpbox.h> 
#include <qscrbar.h>
#include <qcombo.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qpainter.h>
#include <qmsgbox.h>
#include <qtabdlg.h>
#include <string.h>
#include "scriptedit.h"
#include "iplined.h"
#include "kpppconfig.h"
#include "pppdargs.h"
#include "pppdata.h"
#include "edit.h"
#include <qchkbox.h>
#include <qlabel.h>

class DialWidget : public QWidget {
Q_OBJECT
public:
  DialWidget( QWidget *parent=0, bool isnewaccount = true, const char *name=0 );
  ~DialWidget() {}

public slots:
  bool save();
  void pppdargsbutton();
  
private:
  QGroupBox *box;
  QLineEdit *connectname_l;
  QLabel *connect_label;

  QLineEdit *number_l;
  QLabel *number_label;

  QLineEdit *command;
  QLabel *command_label;

  QLineEdit *predisconnect;
  QLabel *predisconnect_label;

  QLineEdit *discommand;
  QLabel *discommand_label;

  QPushButton *pppdargs;
  QLabel *pppd_label;

  QComboBox *auth;
  QLabel *auth_l;

  QCheckBox *store_password;
};


class IPWidget : public QWidget {
Q_OBJECT
public:
  IPWidget( QWidget *parent=0, bool isnewaccount = true, const char *name=0 );
  ~IPWidget() {}

  virtual void resizeEvent(QResizeEvent *);

public slots:
  void save();


protected slots:
  void hitIPSelect( int );
  void autoname_t(bool on);

private:

  QLabel *ipaddress_label;
  QLabel *sub_label;
  QGroupBox *box;
  QGroupBox *box1;

  QButtonGroup *rb;
  QRadioButton *dynamicadd_rb;
  QRadioButton *staticadd_rb;

  IPLineEdit *ipaddress_l;
  IPLineEdit *subnetmask_l;

  QCheckBox *autoname;
};


class DNSWidget : public QWidget {
Q_OBJECT
public:
  DNSWidget( QWidget *parent=0, bool isnewaccount = true, const char *name=0 );
  ~DNSWidget() {}

public slots:
  void save();

protected slots:
  void adddns();
  void removedns();
  void DNS_Edit_Changed(const char *);
  void DNS_Entry_Selected(int);

private:
  QLabel *dns_label;
  QLabel *servers_label;
  QGroupBox *box;
  IPLineEdit *dnsipaddr;
  QPushButton *add;
  QPushButton *remove;
  QListBox *dnsservers;
  QLineEdit *dnsdomain;
  QLabel *dnsdomain_label;
  QCheckBox *exdnsdisabled_toggle;
};


class GatewayWidget : public QWidget {
Q_OBJECT
public:
  GatewayWidget( QWidget *parent=0, bool isnewaccount = true, const char *name=0 );
  ~GatewayWidget() {}

  virtual void resizeEvent(QResizeEvent *);

public slots:
  void save();

private slots:
  void hitGatewaySelect( int );

private:

  QLabel *gate_label;
  QGroupBox *box;
  QGroupBox *box1;
  QButtonGroup *rb;
  QRadioButton *defaultgateway;
  QRadioButton *staticgateway;
  IPLineEdit *gatewayaddr;
  QCheckBox *defaultroute;
};


class ScriptWidget : public QWidget {
Q_OBJECT
public:
  ScriptWidget( QWidget *parent=0, bool isnewaccount = true, const char *name=0 );
  ~ScriptWidget() {}

public slots:
  void save();
  bool check();

private slots:
  void addButton();
  void insertButton();
  void removeButton();

  //signals linked to the scroll bar
  void scrolling(int);

  //signals to keep the two listboxes highlighted in sync
  void slhighlighted(int);
  void stlhighlighted(int);

private:
  void adjustScrollBar();
  QGroupBox *box;

  ScriptEdit *se;
  QPushButton *add;
  QPushButton *remove;
  QPushButton *insert;
  QListBox *sl, *stl;
  
  QScrollBar *slb;
};

#endif
