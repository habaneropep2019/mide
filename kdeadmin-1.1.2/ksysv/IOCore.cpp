/*

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

// KSysV Core

#include <errno.h>
#include <unistd.h>
#include "IOCore.h"
#include "DragListCanvas.h"
#include "ksv_core.h"

// include meta-object code
#include "IOCore.moc"

const QString IOCore::relToAbs( QString dir, QString rel ) {
  QString _abs;

  if (rel.left(1) != "/")
    _abs = QDir::cleanDirPath( (dir + "/").data() + rel);
  else
    _abs = QDir::cleanDirPath( rel );
  return _abs;
}

const QString IOCore::removeFile( QFileInfo* info, QDir dir ) {
  if (!dir.remove(info->fileName(), FALSE))
    return QString().sprintf(i18n("FAILED to remove %s from %s: \"%s\"\n"),
			     info->fileName().data(),
			     dir.path(),
			     strerror(errno));
  else
    return QString().sprintf(i18n("removed %s from %s\n"),
			     info->fileName().data(),
			     dir.path());
}

void IOCore::dissectFilename( QString _file, QString& _base, int& _nr ) {
  QString tmp = _file.mid(1, _file.length());

  _nr = tmp.left(2).toInt();
  _base = tmp.mid(2, tmp.length());
}

const QString IOCore::makeSymlink( KSVDragData* data, const int runlevel, const bool start ) {

  const QString symName = QString().sprintf("%s%.2i%s",
					    start ? "S" : "K", // start or stop?
					    data->number(),
					    data->currentName().data());

  const QString symPath = QString().sprintf("%s/rc%i.d/",
					  CONF->getRLPath().data(),
					  runlevel);

  const QString symbol = symPath + symName;
  QString target = data->filename();
  
  if (target.left(1) != "/")
    target = data->filenameAndPath();

  if ( !symlink(target.data(), symbol.data()) )
    return QString().sprintf(i18n("created %s in %s\n"),
			     symName.data(),
			     symPath.data());
  else
    return QString().sprintf(i18n("FAILED to create %s in %s: \"%s\"\n"),
			     symName.data(),
			     symPath.data(),
			     strerror(errno));
}
