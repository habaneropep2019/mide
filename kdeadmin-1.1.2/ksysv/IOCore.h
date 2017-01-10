/*  Yo Emacs, this -*- C++ -*-

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/     

#ifndef KSV_IOCORE_H
#define KSV_IOCORE_H

#include <qfileinf.h>
#include <qdir.h>
#include <qstring.h>
#include <qobject.h>
#include <kapp.h>
#include "Data.h"
#include "ksv_conf.h"

class IOCore : public QObject
{
  Q_OBJECT


  /** 
    * static members (utility functions)
    */
 public:
  static const QString removeFile( QFileInfo* info, QDir dir );
  static const QString relToAbs( QString dir, QString rel );
  static const QString makeSymlink( KSVDragData* data, const int runlevel, const bool start);
  static void dissectFilename( QString _file, QString& _name, int& _nr );
};

#endif
