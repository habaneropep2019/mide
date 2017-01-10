/* -*- c++ -*- */
/***************************************************************************
 *                               KAQueryFile.h                             *
 *                            -------------------                          *
 *                         Header file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef KAQUERY_FILE_H
#define KAQUERY_FILE_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetm.h>
#include <qdstream.h> 

struct access {
  bool read, write, execute;
};

struct permissions {
  struct access user, group, others;
};


class KAQueryFile : public QObject
/* FileObject as a result of the Query
 * Reimplemented from VLINK to have a better maintenance
 */
{
  friend QDataStream &operator<<(QDataStream &str, KAQueryFile &item);
  friend QDataStream &operator>>(QDataStream &str, KAQueryFile &item);

Q_OBJECT
private:
  //KAQueryFile(const VLINK link);

  QString sHost, sPath, sFile, sMode;
  int nSize;
  //  permissions mode;
  QDateTime datetm;

public:
KAQueryFile() {};
KAQueryFile(const char *host, const char *path, const char *file, int size, const char *mode, const QDate &date, const QTime &time)
  : sHost( host ), sPath( path ), sFile( file ),
    sMode( mode ), nSize( size ), datetm( date, time ) {};

  const QString &getHost() const { return sHost; }
  const QString &getPath() const { return sPath; }
  const QString &getFile() const { return sFile; }
  int            getSize() const { return nSize; }
  const QString &getMode() const { return sMode; }
  //  const QDate   &getDate() const { return datetm.date(); }
  //  const QTime   &getTime() const { return datetm.time(); }
  const QDateTime &getDateTime() const { return datetm; }


};

#endif
