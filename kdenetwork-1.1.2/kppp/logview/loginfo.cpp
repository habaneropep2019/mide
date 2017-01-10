/*
 * kPPPlogview: a accounting log system for kPPP
 *
 *            Copyright (C) 1998 Mario Weilguni <mweilguni@kde.org>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "loginfo.h"
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

char *mystrsep (char **stringp, const char *delim);

LogInfo::LogInfo(QString data) {
  parse(data);
}

QDateTime LogInfo::from() {
  QDateTime tm;
  tm.setTime_t(_from);
  return tm;
}

QDateTime LogInfo::until() {
  QDateTime tm;
  tm.setTime_t(_until);
  return tm;
}

QString LogInfo::connectionName() {
  return _conname;
}

QString LogInfo::currency() {
  return _currency;
}

double LogInfo::sessionCosts() {
  return _session_cost;
}

double LogInfo::totalCosts() {
  return _total_cost;
}

int LogInfo::bytesIn() {
  return _bytes_in;
}

int LogInfo::bytesOut() {
  return _bytes_out;
}

int LogInfo::bytes() {
  if(bytesIn() == -1 || bytesOut() == -1)
    return -1;
  else
    return bytesIn() + bytesOut();
}

int LogInfo::error() {
  return errorfield;
}

void LogInfo::parse(QString s) {
  errorfield = 0;
  char *c = (char *)malloc(s.length() + 1), *csep;
  strcpy(c, s.data());

  // init data
  _from = _until = 0;
  _conname = "";
  _currency = "";
  _bytes_in = _bytes_out = -1;
  _session_cost = _total_cost = -1;

  // start of connection time
  csep = c;
  char *p = mystrsep(&csep, ":");
  int i = 0;
  while(i < 8 && p != 0) {
    QString token = p;

    switch(i) {
    case 0:
      _from = token.toULong();
      break;

    case 1:
      _conname = token.copy();
      break;

    case 2:
      _currency = token.copy();
      break;

    case 3:
      _until = token.toULong();
      break;

    case 4:
      _session_cost = token.toFloat();
      break;

    case 5:
      _total_cost = token.toFloat();
      break;

    case 6:
      _bytes_in = token.toLong();
      break;
      
    case 7:
      _bytes_out = token.toLong();
      break;
    }

    i++;
    p = mystrsep(&csep, ":");
  }

  free(c);

  if(i == 8)
    errorfield = 0;
  else
    errorfield = i+1;
}


int LogInfo::duration() {
  if( _until - _from < 0) 
    return 0;
  else
    return _until - _from;
}

#ifdef MYDEBUG
#include <stdio.h>

void LogInfo::dump() {
  printf("LOGINFO %u\n", (unsigned)this);
  printf("\tconnection name : %s\n", connectionName().data());
  printf("\tcurrency symbol : %s\n", currency().data());
  printf("\tbegin           : %s", ctime(&_from));
  printf("\tend             : %s", ctime(&_until));
  printf("\tduration        : %d seconds\n", (int)_until - (int)_from);
  printf("\tsession cost    : %0.2e\n", sessionCosts());
  printf("\ttotal cost      : %0.2e\n", totalCosts());
  printf("\tbytes in        : %d\n", bytesIn());
  printf("\tbytes out       : %d\n", bytesOut());
  printf("\tbytes total     : %d\n", bytes());
  printf("\n");
}
#endif

char *mystrsep (char **stringp, const char *delim) {
  char *begin, *end;

  begin = *stringp;
  if (begin == 0L)
    return 0L;

  /* Find the end of the token.  */
  end = strpbrk (begin, delim);
  if (end) {
      /* Terminate the token and set *STRINGP past NUL character.  */
      *end++ = '\0';
      *stringp = end;
  } else
    /* No more delimiters; this is the last token.  */
    *stringp = 0L;

  return begin;
}


