/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: ruleset.cpp,v 1.11 1998/12/16 17:08:41 mario Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * This file was contributed by Mario Weilguni <mweilguni@sime.com>
 * Thanks Mario !
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


#include <qregexp.h>
#include <qstring.h>
#include <stdlib.h>
#include <stdio.h>
#include <qfile.h>
#include <math.h>
#include <kapp.h>
#include "ruleset.h"
#include "log.h"

RuleSet::RuleSet() {
  default_costs = -1;
  default_len = -1;
  _currency_symbol = "$";
  _currency_position = CURRENCY_RIGHT;
  _currency_digits = 2;
  _minimum_costs = 0;
  flat_init_costs = 0.0;
  flat_init_duration = 0;
  have_flat_init_costs = false;

  pcf = 0.0;
}

// this function is shamelessly stolen from pppcosts 0.5 :-)
/* calculates the easter sunday in day_of_year style */
QDate RuleSet::get_easter(int year) {
  /* not optimized, I took the original names */
  signed int a,b,m,q,w,p,n,tt,mm;

  /* calculating easter is really funny */
  /* this is O'Beirne's algorithm, only valid 1900-2099 */
  n = year - 1900;
  a = n % 19;
  b = (int)((7*a+1)/19);
  m = (11*a+4-b) % 29;
  q = (int)(n/4);
  w = (n+q+31-m) % 7;
  p = 25-m-w;
  if (p>0)
    {tt=p;
    mm=4;}
  else
    {tt=p+31;
    mm=3;}

  return QDate(year, mm, tt);
}

int RuleSet::dayNameToInt(const char *s) {
  const char *name[] = {"monday", "tuesday", "wednesday",
			"thursday", "friday", "saturday",
			"sunday", NULL};

  for(int i = 0; name[i] != NULL; i++)
    if(stricmp(name[i], s) == 0)
      return i;

  return -1;
}

int RuleSet::load(const char *filename) {

  flat_init_costs = 0.0;
  flat_init_duration = 0;
  have_flat_init_costs = false;

  QFile f(filename);

  // delete old rules
  rules.resize(0);
  _name = "";

  // ignore "No Accounting"
  if(strlen(filename) == 0)
    return 0;

  if(!f.exists())
    return -1;

  if(!f.open(IO_ReadOnly))
    return -1;

  char buffer[2048]; // safe
  int lineno=0;

  while(!f.atEnd()) {
    // read continued lines
    QString line;
    bool backslashed;
    do {
      int br = f.readLine(buffer, sizeof(buffer));
      if((br > 0) && (buffer[br-1] == '\n'))
	buffer[br-1] = 0;
      else
	buffer[br] = 0;
      lineno++;
      line.append(buffer);
      backslashed = (line.right(1) == "\\");
    } while(!f.atEnd() && backslashed);

    // strip whitespace
    line = line.replace(QRegExp("[ \t]"), "");
    // skip comment lines
    if((line.left(1) == "#") || (line.length()==0))
      continue;

    // parse line
    if(!parseLine(line)) {
      f.close();
      Debug("ERROR IN LINE %d\n", lineno);
      return lineno;
    }
  }

  f.close();

  if(_name.length() > 0)
    return 0;
  else {
    Debug("NO NAME DEFINED\n");
    return -1;
  }
}

void RuleSet::addRule(RULE r) {
  // check for a default rule
  if((r.type == 2) &&
     (r.weekday.from == 0) && (r.weekday.until == 6) &&
     (r.from == midnight()) &&
     (r.until == beforeMidnight()))
    {
      default_costs = r.costs;
      default_len = r.len;
      return;
    }

  // if from < until (i.e on (monday..friday)
  // from (21:00..05:00) use (0.2,16)
  // split it into two rules
  // ... between (21:00..23:59) use ...
  // ... between (00:00..05:00) use ...
  if(r.from > r.until) {
    RULE r1, r2;
    r1 = r;
    r2 = r;
    r1.until = beforeMidnight();
    r2.from = midnight();
    rules.resize(rules.size()+2);
    rules[rules.size()-2] = r1;
    rules[rules.size()-1] = r2;
  } else {
    rules.resize(rules.size()+1);
    rules[rules.size()-1] = r;
  }
}

bool RuleSet::parseEntry(RULE &ret, QString s, int year) {
  if(s.contains(QRegExp("^[0-9]+/[0-9]+$"))) {
    int d, m;
    sscanf(s.data(), "%d/%d", &m, &d);
    ret.type = 1;
    ret.date.from = QDate(year, m, d);
    ret.date.until = QDate(year, m, d);
    return TRUE;
  }

  if(s.right(3) == "day") {
    int d = dayNameToInt(s.data());
    if(d != -1) {
      ret.type = 2;
      ret.weekday.from = d;
      ret.weekday.until = d;
      return TRUE;
    }
  }

  if(s.left(6) == "easter") {
    QDate d = get_easter(year);
    int off;
    bool ok = TRUE;
    QString val = s.mid(6, 1000);
    if(val.length() == 0)
      off = 0;
    else
      off = val.toInt(&ok);

    if(ok) {
      d = d.addDays(off);
      ret.type = 1;
      ret.date.from = d;
      ret.date.until = d;
      return TRUE;
    }
  }

  ret.type = 0;
  return FALSE;
}



bool RuleSet::parseEntries(QString s, int year,
			   QTime t1, QTime t2,
			   double costs, double len, double after)
{
  // special rule: on() is the same as on(monday..sunday)
  if(s == "")
    s = "monday..sunday";

  while(s.length()) {
    int pos = s.find(',');
    QString token;
    if(pos == -1) {
      token = s;
      s = "";
    } else {
      token = s.left(pos);
      s = s.right(s.length()-pos-1);
    }

    // we've a token, now check if it defines a
    // range
    RULE r;
    if(token.contains("..")) {
      QString left, right;
      left = token.left(token.find(".."));
      right = token.right(token.length()-2-left.length());
      RULE lr, rr;
      if(parseEntry(lr, left, year) && parseEntry(rr, right, year)) {
	if(lr.type == rr.type) {
	  r.type = lr.type;
	  switch(lr.type) {
	  case 1:
	    r.date.from = lr.date.from;
	    r.date.until = rr.date.from;
	    break;
	  case 2:
	    r.weekday.from = lr.weekday.from;
	    r.weekday.until = rr.weekday.from;
	  }
	} else
	  return FALSE;
      }
    } else
      if(!parseEntry(r, token, year))
	return FALSE;

    r.costs = costs;
    r.len = len;
    r.after = after;
    r.from = t1;
    r.until = t2;
    addRule(r);
  }

  return TRUE;
}

bool RuleSet::parseTime(QTime &t1, QTime &t2, QString s) {
  if(s.length() == 0) {
    t1 = midnight();
    t2 = beforeMidnight();
    return TRUE;
  } else {
    int t1m, t1h, t2m, t2h;
    if(sscanf(s.data(), "%d:%d..%d:%d", &t1h, &t1m, &t2h, &t2m) == 4) {
      t1.setHMS(t1h, t1m, 0);
      t2.setHMS(t2h, t2m, 0);
      return TRUE;
    } else
      return FALSE;
  }
}

bool RuleSet::parseRate(double &costs, double &len, double &after, QString s) {
  after = 0;
  int fields = sscanf(s.data(), "%lf,%lf,%lf", &costs, &len, &after);
  return (fields == 2) || (fields == 3);
}

bool RuleSet::parseLine(QString &s) {

  // for our french friends -- Bernd
  if(s.contains(QRegExp("flat_init_costs=(.*"))) {
    // parse the time fields
    QString token = s.mid(s.find("flat_init_costs=(") + 17,
			  s.find(")")-s.find("flat_init_costs=(") - 17);
    //    printf("TOKEN=%s\n",token.data());

    double after;
    if(!parseRate(flat_init_costs, flat_init_duration, after, token))
      return FALSE;

    //printf("COST %f DURATION %f\n",flat_init_costs,flat_init_duration);

    if(! (flat_init_costs >= 0.0) )
      return FALSE;
    if(! (flat_init_duration > 0.0))
      return FALSE;

    have_flat_init_costs = true;
    return TRUE;
  }


  if(s.contains(QRegExp("on(.*)between(.*)use(.*)"))) {
    // parse the time fields
    QString token = s.mid(s.find("between(") + 8,
			  s.find(")use")-s.find("between(") - 8);
    QTime t1, t2;
    if(!parseTime(t1, t2, token))
      return FALSE;

    // parse the rate fields
    token = s.mid(s.find("use(") + 4,
			  s.findRev(")")-s.find("use(") - 4);
    double costs;
    double len;
    double after;
    if(!parseRate(costs, len, after, token))
      return FALSE;

    // parse the days
    token = s.mid(s.find("on(") + 3,
		  s.find(")betw")-s.find("on(") - 3);
    if(!parseEntries(token, QDate::currentDate().year(),
		     t1, t2, costs, len, after))
      return FALSE;

    return TRUE;
  }

  // check for the name
  if(s.contains(QRegExp("name=.*"))) {
    _name = s.right(s.length()-5);
    return (bool)(_name.length() > 0);
  }


  // check default entry
  if(s.contains(QRegExp("default=(.*)"))) {
    QString token = s.mid(9, s.length() - 10);
    double after;
    if(parseRate(default_costs, default_len, after, token))
      return TRUE;
  }

  // check for "minimum costs"
  if(s.contains(QRegExp("minimum_costs=.*"))) {
    QString token = s.right(s.length() - strlen("minimum_costs="));
    bool ok;
    _minimum_costs = token.toDouble(&ok);
    return ok;
  }

  // check currency settings
  if(s.contains(QRegExp("currency_symbol=.*"))) {
     _currency_symbol = s.mid(16, s.length()-16);

      return TRUE;
  }

  if(s.contains(QRegExp("currency_digits=.*"))) {
    QString token = s.mid(16, s.length() - 16);
    bool ok;
    _currency_digits = token.toInt(&ok);
    return ok && (_currency_digits >= 0);
  }

  if(s.contains(QRegExp("currency_position=.*"))) {
    QString token = s.mid(18, s.length() - 18);
    if(token == "left") {
      _currency_position = CURRENCY_LEFT;
      return TRUE;
    } else if(token == "right") {
      _currency_position = CURRENCY_RIGHT;
      return TRUE;
    }
  }

  // check per connection fee
  if(s.contains(QRegExp("per_connection="))) {
    QString token = s.mid(15, s.length()-15);
    bool ok;
    pcf = token.toDouble(&ok);
    return ok;
  }

  return FALSE;
}

void RuleSet::setStartTime(QDateTime dt){

  starttime = dt;

}

void RuleSet::getActiveRule(QDateTime dt, double connect_time, double &costs, double &len) {
  // use default costs first
  costs = default_costs;
  len = default_len;

  //printf("In getActiveRule\n");
  if(have_flat_init_costs){

    costs = flat_init_costs;
    len = flat_init_duration;
    have_flat_init_costs = false;
    //printf("getActiveRule FLATINITCOSTS\n");
    return;
  }

  // check every rule
  for(int i = 0; i < (int)rules.size(); i++) {
    RULE r = rules[i];

    switch(r.type) {
    case 1: // a date
      {
	// since rules do not have a year's entry, use the one
	// from dt
	QDate from = r.date.from;
	QDate until = r.date.until;
	from.setYMD(dt.date().year(), from.month(), from.day());
	until.setYMD(dt.date().year(), until.month(), until.day());
	if((from <= dt.date()) && (dt.date() <= until)) {
	  // check time
	  if((r.from <= dt.time()) && (dt.time() <= r.until) && (connect_time >= r.after)) {
	    costs = r.costs;
	    len = r.len;
	  }
	}
      }
    break;

    case 2: // one or more weekdays
      // check if the range overlaps sunday.
      // (i.e. "on(saturday..monday)")
      if(r.weekday.from <= r.weekday.until) {
	if((r.weekday.from <= dt.date().dayOfWeek() - 1) &&
	   (r.weekday.until >= dt.date().dayOfWeek() - 1))
	  {
	    // check time
	    if((r.from <= dt.time()) && (dt.time() <= r.until) && (connect_time >= r.after)) {
	      costs = r.costs;
	      len = r.len;
	    }
	  }
      } else { // yes, they overlap sunday
	if((r.weekday.from >= dt.date().dayOfWeek() - 1) &&
	   (dt.date().dayOfWeek() - 1 <= r.weekday.until))
	  {
	    // check time
	    if((r.from <= dt.time()) && (dt.time() <= r.until) && (connect_time >= r.after)) {
	      costs = r.costs;
	      len = r.len;
	    }
	  }
      }
    }
  }
}


double round(double d, int digits) {
  d *= pow(10, digits);
  d = rint(d);
  d /= pow(10, digits);
  return d;
}

QString RuleSet::currencySymbol() {
  return _currency_symbol.copy();
}

QString RuleSet::currencyString(double f) {
  QString s, fmt;

  if(_currency_position == CURRENCY_RIGHT) {
    fmt.sprintf("%%0.%df %%s", _currency_digits);
    s.sprintf(fmt.data(), round(f, _currency_digits),
	      _currency_symbol.data());
  } else {
    fmt.sprintf("%%s %%0.%df", _currency_digits);
    s.sprintf(fmt.data(), _currency_symbol.data(),
	      round(f, _currency_digits));
  }
  return s;
}


double RuleSet::perConnectionCosts() {
  return pcf;
}


QString RuleSet::name() {
  return _name;
}


double RuleSet::minimumCosts() {
  return _minimum_costs;
}

QTime RuleSet::midnight() const {
  return QTime(0, 0, 0, 0);
}

QTime RuleSet::beforeMidnight() const {
  return QTime(23,59,59,999);
}

int RuleSet::checkRuleFile(const char *rulefile) {
  if(rulefile == NULL) {
    fprintf(stderr, i18n("kppp: no rulefile specified\n"));
    return 1;
  }

  QFile fl(rulefile);
  if(!fl.exists()) {
    fprintf(stderr, i18n("kppp: rulefile \"%s\" not found\n"), rulefile);
    return 1;
  }

  if(QString(rulefile).right(4) != ".rst") {
    fprintf(stderr, i18n("kppp: rulefiles must have the extension \".rst\"\n"));
    return 1;
  }

  RuleSet r;
  int err = r.load(rulefile);
  fl.close();

  if(err == -1) {
    fprintf(stderr, i18n("kppp: error parsing the ruleset\n"));
    return 1;
  }

  if(err > 0) {
    fprintf(stderr, i18n("kppp: parse error in line %d\n"), err);
    return 1;
  }

  // check for the existance of a default rule
  if((r.default_costs < 0) || (r.default_len < 0)) {
    fprintf(stderr, i18n("kppp: rulefile does not contain a default rule\n"));
    return 1;
  }

  if(r.name().length() == 0) {
    fprintf(stderr, i18n("kppp: rulefile does not contain a \"name=...\" line\n"));
    return 1;
  }

  fprintf(stderr, i18n("kppp: rulefile is ok\n"));
  return 0;
}

