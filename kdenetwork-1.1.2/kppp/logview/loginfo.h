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

#ifndef __LOGINFO__H__
#define __LOGINFO__H__
// #define MYDEBUG

#include <qdatetm.h>
#include <qstring.h>
#include <time.h>

class LogInfo {
public:
  LogInfo(QString data);

  int error();

  QDateTime from();
  QDateTime until();
  int duration();
  QString connectionName();
  QString currency();
  double sessionCosts();
  double totalCosts();
  int bytesIn();
  int bytesOut();
  int bytes();

#ifdef MYDEBUG
  void dump();
#endif

private:
  void parse(QString );

  int errorfield;

  time_t _from, _until;
  QString _conname, _currency;
  double _session_cost, _total_cost;
  int _bytes_in, _bytes_out;
};


#endif
