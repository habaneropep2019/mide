/* -*- c++ -*- */
/***************************************************************************
 *                               KAQueryList.h                             *
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

#ifndef KAQUERY_LIST_H
#define KAQUERY_LIST_H

#include "KAQueryFile.h"
#include <qlist.h>


/*
class KAQueryList : public QList<KAQueryFile>
{

};
*/
typedef QList<KAQueryFile> KAQueryList;

class KAHostList : public KAQueryList
{
  /* abgeleitet von QList,
   * um die Liste nach Hostnamen zu sortieren.
   */
protected:
  virtual int compareItems( GCI item1, GCI item2 );

};

class KADomainList : public KAQueryList
{
  /* abgeleitet von QList,
   * um die Liste nach Domains zu sortieren.
   */
protected:
  virtual int compareItems( GCI item1, GCI item2 );

};

class KASizeList : public KAQueryList
{
  /* abgeleitet von QList,
   * um die Liste nach Dateigroesse zu sortieren.
   */
protected:
  virtual int compareItems( GCI item1, GCI item2 );
  
};

class KADateList : public KAQueryList
{
  /* abgeleitet von QList,
   * um die Liste nach Dateidatum zu sortieren.
   */
protected:
  virtual int compareItems( GCI item1, GCI item2 );
  
};

#endif
