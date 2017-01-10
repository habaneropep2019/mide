/* -*- C++ -*-
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: accounting.cpp,v 1.16.2.1 1999/06/13 17:56:48 porten Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * This file contributed by: Mario Weilguni, <mweilguni@sime.com>
 * Thanks Mario!
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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <qdir.h>
#include <qfile.h>
#include <qdatetm.h>
#include <qregexp.h>
#include <qlabel.h>

#include <kapp.h>
#include <kprogress.h>
#include <time.h>

#include "accounting.h"
#include "kpppconfig.h"
#include "pppdata.h"
#include "log.h"

// defines the maximum duration until the current costs
// are saved again (to prevent loss due to "kill")
// specifying -1 disables the features
#define UPDATE_TIME    (5*60*1000)

extern PPPData gpppdata;
extern int ibytes;
extern int obytes;


/////////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////////////////
QString timet2qstring(time_t t) {
  QString s;

  s.sprintf("%lu", t);
  return s;
}


/////////////////////////////////////////////////////////////////////////////
//
// The base class for the accounting system provides a base set of usefull
// and common functions, but does not do any accounting by itself. The 
// accounting is accomplished withing it´s derived classes
//
/////////////////////////////////////////////////////////////////////////////
AccountingBase::AccountingBase(QObject *parent) :
  QObject(parent),
  _total(0),
  _session(0)
{
  QDate dt = QDate::currentDate();
  LogFileName.sprintf("%s-%4d.log",
		      dt.monthName(dt.month()),
		      dt.year());

  QString fname = QDir::homeDirPath() + "/";
  fname += ACCOUNTING_PATH;
  fname += "/Log/";
  fname += LogFileName;

  LogFileName = fname;
}

AccountingBase::~AccountingBase() {
  if(running())
    slotStop();
}


double AccountingBase::total() {
  return _total + _session;
}



double AccountingBase::session() {
  return _session;
}


// set costs back to zero ( typically once per month)
void AccountingBase::resetCosts(const char *accountname){
  QString prev_account = gpppdata.accname();

  gpppdata.setAccount(accountname);
  gpppdata.setTotalCosts("");

  gpppdata.setAccount(prev_account);
}


void AccountingBase::logMessage(QString s, bool newline) {
  int old_umask = umask(0077);

  QFile f(LogFileName.data());

  bool result = f.open(IO_ReadWrite);
  if(result) {
    // move to eof, and place \n if necessary
    if(f.size() > 0) {
      if(newline) {
	f.at(f.size() - 1);
	char c = 0;
	f.readBlock(&c, 1);
	if(c != '\n')
	  f.writeBlock("\n", 1);
      } else
	f.at(f.size() - 1);
    }

    f.writeBlock(s.data(), s.length());
    f.close();
  }

  // restore umask
  umask(old_umask);
}


QString AccountingBase::getCosts(const char* accountname) {
  QString prev_account = gpppdata.accname();

  gpppdata.setAccount(accountname);
  QString val = gpppdata.totalCosts();

  gpppdata.setAccount(prev_account);

  return val;
}



bool AccountingBase::saveCosts() {
  if(!_name.isNull() && _name.length() > 0) {
    QString val;
    val.setNum(total());

    gpppdata.setTotalCosts(val);
    gpppdata.save();

    return TRUE;
  } else
    return FALSE;
}


bool AccountingBase::loadCosts() {
  QString val = gpppdata.totalCosts();

  if(val.isNull()) // QString will segfault if isnull and toDouble called
    _total = 0.0;
  else {
    bool ok;
    _total = val.toDouble(&ok);
    if(!ok)
      _total = 0.0;
  }

  return TRUE;
}


QString AccountingBase::getAccountingFile(const char* accountname) {
  // load from home directory if file is found there
  QString d = QDir::homeDirPath() + "/";
  d += ACCOUNTING_PATH ;
  d += "/Rules/";
  d += accountname;

  if(access(d.data(), R_OK) == 0)
    return d;

  // load from KDE directory if file is found there
  d = KApplication::kde_datadir().copy();
  d += "/kppp/Rules/";
  d += accountname;
  if(access(d.data(), R_OK) == 0)
    return d;

  d = "";
  return d;
}


/////////////////////////////////////////////////////////////////////////////
//
// Accounting class for ruleset files
//
/////////////////////////////////////////////////////////////////////////////
Accounting::Accounting(QObject *parent) : 
  AccountingBase(parent),
  acct_timer_id(0),
  update_timer_id(0)
{
}


bool Accounting::running() {
  return (bool)(acct_timer_id != 0);
}


void Accounting::timerEvent(QTimerEvent *t) {
  if(t->timerId() == acct_timer_id) {

    double newCosts;
    double newLen;
    double connect_time = difftime(time(0), start_time);

    rules.getActiveRule(QDateTime::currentDateTime(), connect_time, newCosts, newLen);
    if(newLen < 1) { // changed to < 1
      slotStop();
      return; // no default rule found
    }

    // check if we have a new rule. If yes,
    // kill the timer and restart it with new
    // duration
    if((newCosts != _lastcosts) || (newLen != _lastlen)) {

      Debug("SWITCHING RULES, new costs = %0.2f, new len = %0.2f\n",
	     newCosts, newLen);

      killTimer(acct_timer_id);
      if(newLen > 0)
	acct_timer_id = startTimer((int)(newLen * 1000.0));

      _lastlen = newLen;
      _lastcosts = newCosts;
    }

    // emit changed() signal if necessary
    if(newCosts != 0) {
      _session += newCosts;
       emit changed(rules.currencyString(total()),
		    rules.currencyString(session()));


    }
  } // if(t->timerId() == acct_timer_id)...

  if(t->timerId() == update_timer_id) {
    // just to be sure, save the current costs
    // every n seconds (see UPDATE_TIME)
    saveCosts();
  }
}


void Accounting::slotStart() {
  if(!running()) {
    loadCosts();
    _lastcosts = 0.0;
    _lastlen   = 0.0;
    _session = rules.perConnectionCosts();
    rules.setStartTime(QDateTime::currentDateTime());
    acct_timer_id = startTimer(1);
     if(UPDATE_TIME > 0)
       update_timer_id = startTimer(UPDATE_TIME);

    start_time = time(0); 
    QString s;
    s = timet2qstring(start_time);
    s += ":";
    s += gpppdata.accname();
    s += ":";
    s += rules.currencySymbol();

    logMessage(s, TRUE);
  }
}


void Accounting::slotStop() {
  if(running()) {
    killTimer(acct_timer_id);
    if(update_timer_id != 0)
      killTimer(update_timer_id);
    acct_timer_id = 0;
    update_timer_id = 0;

    QString s;
    s.sprintf(":%s:%0.4e:%0.4e:%u:%u\n",
	      timet2qstring(time(0)).data(),
	      session(),
	      total(),
	      ibytes,
	      obytes);

    logMessage(s, FALSE);
    saveCosts();
  }
}


bool Accounting::loadRuleSet(const char *name) {

  if(strlen(name) == 0) {
    rules.load(""); // delete old rules
    return TRUE;
  }
  
  QString d = AccountingBase::getAccountingFile(name);

  QFileInfo fg(d.data());
   if(fg.exists()) {
     int ret = rules.load(d.data());
     _name = rules.name();
     return (bool)(ret == 0);
   }

 return FALSE;
}


double Accounting::total() {
  if(rules.minimumCosts() <= _session)
    return _total + _session;
  else
    return _total + rules.minimumCosts();
}



double Accounting::session() {
  if(rules.minimumCosts() <= _session)
    return _session;
  else
    return rules.minimumCosts();
}




ExecutableAccounting::ExecutableAccounting(QObject *parent) : 
  AccountingBase(parent),
  proc(0)
{
}


bool ExecutableAccounting::running() {
  return (proc != 0) || proc->isRunning();
}


bool ExecutableAccounting::loadRuleSet(const char *) {
  QString s = AccountingBase::getAccountingFile(gpppdata.accountingFile());
  return (access(s.data(), X_OK) == 0);
}


void ExecutableAccounting::gotData(KProcess */*proc*/, char *buffer, int /*buflen*/) {
  QString field[8];
  int nFields = 0;
  int pos, last_pos = 0;

  // split string
  QString b(buffer);
  pos = b.find(':');
  while(pos != -1 && nFields < 8) {
    field[nFields++] = b.mid(last_pos, pos-last_pos);
    last_pos = pos+1;
    pos = b.find(':', last_pos);
  }

  for(int i = 0; i < nFields;i++)
    fprintf(stderr, "FIELD[%d] = %s\n", i, field[i].data());
  
  QString __total, __session;
  QString s(buffer);
  int del1, del2, del3;

  del1 = s.find(':');
  del2 = s.find(':', del1+1);
  del3 = s.find(':', del2+1);
  if(del1 == -1 || del2 == -1 || del3 == -1) {
    // TODO: do something usefull here
    return;
  }

  provider = s.left(del1);
  currency = s.mid(del1, del2-del1);
  __total = s.mid(del2, del2-del1);
  __session = s.mid(del3, s.length()-del3+1);

  bool ok1, ok2;
  _total = __total.toDouble(&ok1);
  _session = __session.toDouble(&ok2);
  
  if(!ok1 || !ok2) {
    // TODO: do something usefull here
    return;
  }

  printf("PROVIDER=%s, CURRENCY=%s, TOTAL=%0.3e, SESSION=%0.3e\n", 
	 provider.data(),
	 currency.data(),
	 _total,
	 _session);
}


void ExecutableAccounting::slotStart() {
  if(proc != 0)
    slotStop(); // just to make sure

  loadCosts();
  QString s = AccountingBase::getAccountingFile(gpppdata.accountingFile());
  proc = new KProcess;

  QString s_total;
  s_total.sprintf("%0.8f", total());
  *proc << s.data() << s_total.data();
  connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
	  this, SLOT(gotData(KProcess *, char *, int)));
  proc->start();

  time_t start_time = time(0); 
  s = timet2qstring(start_time);
  s += ":";
  s += gpppdata.accname();
  s += ":";
  s += currency;
  
  logMessage(s, TRUE);
}


void ExecutableAccounting::slotStop() {
  if(proc != 0) {
    proc->kill();
    delete proc;
    proc = 0;

    QString s;
    s.sprintf(":%s:%0.4e:%0.4e:%u:%u\n",
	      timet2qstring(time(0)).data(),
	      session(),
	      total(),
	      ibytes,
	      obytes);

    logMessage(s, FALSE);
    saveCosts();
  }
}

#include "accounting.moc"

