/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: ruleset.h,v 1.5 1999/01/13 19:24:58 mario Exp $
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

#ifndef __RULESET__H__
#define __RULESET__H__

#include <qarray.h>
#include <qdatetm.h>
#include <qstring.h>

#define CURRENCY_LEFT	1
#define CURRENCY_RIGHT	2

// this structure is used to save
// accounting rules
typedef struct RULE {
  int type;
  double costs;
  double len;
  double after;
  QTime from, until;
  struct {
    QDate from, until;
  } date;
  struct {
    int from, until;
  } weekday;
};

// this class is used for loading and parsing of rules
class RuleSet {
public:
  /// create an empty rule
  RuleSet();

  /// gcc needs a destructor (otherwise compiler error)
  ~RuleSet() {};

  /// returns the name of the ruleset
  QString name();

  /** Load a ruleset from a file. If an error occurs,
   *  returns the linenumber the error was in, 
   *  otherwise 0. If the file could not be opened,
   *  returns -1
   */
  int load(const char *filename);

  /// returns the currency symbol
  QString currencySymbol();

  /// returns the desired currency position
  int currencyPosition();

  /** returns a string representation of the
   *  of a doubleingpoint number using the
   *  currency-settings
   */
  QString currencyString(double val);

  /// sets the start time -- must be called when the connection has bee established
  void setStartTime(QDateTime dt);

  /// returns the "per-connection" costs
  double perConnectionCosts();

  /** returns the minimum number of costs (some
   *  phony companies have this
   */
  double minimumCosts();

  /// returns the currently valid rule settings
  void getActiveRule(QDateTime dt, double connect_time, double &costs, double &len);

  /// checks if a rulefile is ok (no parse errors...)
  static int checkRuleFile(const char *);

protected:
  /** converts an english name of a day to integer,
   * beginning with monday=0 .. sunday=6
   */
  int dayNameToInt(const char *s);

  /// returns the date of easter-sunday for a year
  QDate get_easter(int year);

  /// add a rule to this ruleset
  void addRule(RULE r);

  /// parses on entry of the "on(...)" fields
  bool parseEntry(RULE &ret, QString s, int year);

  /// parses the "on(...)" fields
  bool parseEntries(QString s, int year, 
		    QTime t1, QTime t2,
		    double costs, double len, double after);

  /// parses the "between(...)" time fields
  bool parseTime(QTime &t1, QTime &t2, QString s);

  /// parses the "use(...)" fields
  bool parseRate(double &costs, double &len, double &after, QString s);

  /// parses a whole line
  bool parseLine(QString &line);

  /// returns midnight time (00:00:00.000)
  QTime midnight() const;
  
  /// returns the last valid time BEFORE midnight
  QTime beforeMidnight() const;

protected:
  QString _name;
  QString _currency_symbol;
  QDateTime starttime;
  int _currency_position;
  int _currency_digits;
  double default_costs;
  double _minimum_costs;
  double  default_len;
  double pcf;
  bool have_flat_init_costs;
  double flat_init_duration;
  double flat_init_costs;

  QArray<RULE> rules;
};

#endif
