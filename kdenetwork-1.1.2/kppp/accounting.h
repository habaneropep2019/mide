/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: accounting.h,v 1.7.2.1 1999/06/13 17:56:49 porten Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 * This file contributed by: Mario Weilguni, <mweilguni@sime.com>
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

#ifndef __ACCOUNTING__H__
#define __ACCOUNTING__H__

#include <qobject.h>
#include <qmsgbox.h>
#include <kprocess.h>
#include "ruleset.h"

/////////////////////////////////////////////////////////////////////////////
//
// Accounting base class
//
/////////////////////////////////////////////////////////////////////////////
class AccountingBase : public QObject {
  Q_OBJECT
public:
  AccountingBase(QObject *parent = 0);
  virtual ~AccountingBase();

  virtual double total();
  virtual double session();

  virtual bool running() { return false; };
  virtual bool loadRuleSet(const char *name) = 0;

public slots:
  virtual void slotStart() {};
  virtual void slotStop() {};

signals:
  void changed(QString total, QString session);

protected:
  void logMessage(QString, bool = FALSE);
  bool saveCosts();
  bool loadCosts();

  QString LogFileName;
  double _total, _session;
  QString _name;

  // static members
public:
  static void resetCosts(const char *accountname);
  static QString getCosts(const char* accountname);
  static QString getAccountingFile(const char* accountname);
};


/////////////////////////////////////////////////////////////////////////////
//
// Accounting based on ruleset files
//
/////////////////////////////////////////////////////////////////////////////
class Accounting : public AccountingBase {
  Q_OBJECT
public:
  Accounting(QObject *parent = 0);

  virtual double total();
  virtual double session();

  virtual bool loadRuleSet(const char *name);
  virtual bool running();

private:
  virtual void timerEvent(QTimerEvent *t);

public slots:
  virtual void slotStart();
  virtual void slotStop();

signals:
  void changed(QString total, QString session);

private:
  int acct_timer_id;
  int update_timer_id;
  time_t start_time;
  double _lastcosts;
  double _lastlen;
  RuleSet rules;
};


/////////////////////////////////////////////////////////////////////////////
//
// Accounting based on executable files
//
/////////////////////////////////////////////////////////////////////////////
class ExecutableAccounting : public AccountingBase {
  Q_OBJECT
public:
  ExecutableAccounting(QObject *parent = 0);

  virtual bool loadRuleSet(const char *);
  virtual bool running();

public slots:
  virtual void slotStart();
  virtual void slotStop();

private slots:
  void gotData(KProcess *proc, char *buffer, int buflen);

signals:
  void changed(QString total, QString session);

private:
  KProcess *proc;
  QString currency;
  QString provider;
};

#endif
