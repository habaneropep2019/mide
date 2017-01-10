// KDat - a tar-based DAT archiver
// Copyright (C) 1998  Sean Vyain, svyain@mail.tds.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <qpixmap.h>

#include "ImageCache.h"
#include "closed.xpm"
#include "kdat_archive.xpm"
#include "kdat_backup.xpm"
#include "kdat_file.xpm"
#include "kdat_mounted.xpm"
#include "kdat_unmounted.xpm"
#include "kdat_restore.xpm"
#include "kdat_select_all.xpm"
#include "kdat_select_none.xpm"
#include "kdat_select_some.xpm"
#include "kdat_verify.xpm"
#include "open.xpm"
#include "package.xpm"

ImageCache::ImageCache()
{
    _archive       = new QPixmap( (const char**)package );
    _backup        = new QPixmap( (const char**)kdat_backup_xpm );
    _file          = new QPixmap( (const char**)kdat_file_xpm );
    _folderClosed  = new QPixmap( (const char**)closed_xpm );
    _folderOpen    = new QPixmap( (const char**)open_xpm );
    _restore       = new QPixmap( (const char**)kdat_restore_xpm );
    _selectAll     = new QPixmap( (const char**)kdat_select_all_xpm );
    _selectNone    = new QPixmap( (const char**)kdat_select_none_xpm );
    _selectSome    = new QPixmap( (const char**)kdat_select_some_xpm );
    _tape          = new QPixmap( (const char**)kdat_archive_xpm );
    _tapeMounted   = new QPixmap( (const char**)kdat_mounted_xpm );
    _tapeUnmounted = new QPixmap( (const char**)kdat_unmounted_xpm );
    _verify        = new QPixmap( (const char**)kdat_verify_xpm );
}

ImageCache::~ImageCache()
{
    delete _archive;
    delete _backup;
    delete _file;
    delete _folderClosed;
    delete _folderOpen;
    delete _restore;
    delete _tape;
    delete _tapeMounted;
    delete _tapeUnmounted;
    delete _verify;
}

ImageCache* ImageCache::_instance = 0;

ImageCache* ImageCache::instance()
{
    if ( _instance == 0 ) {
        _instance = new ImageCache();
    }

    return _instance;
}

const QPixmap* ImageCache::getArchive()
{
    return _archive;
}

const QPixmap* ImageCache::getBackup()
{
    return _backup;
}

const QPixmap* ImageCache::getFile()
{
    return _file;
}

const QPixmap* ImageCache::getFolderClosed()
{
    return _folderClosed;
}

const QPixmap* ImageCache::getFolderOpen()
{
    return _folderOpen;
}

const QPixmap* ImageCache::getRestore()
{
    return _restore;
}

const QPixmap* ImageCache::getSelectAll()
{
    return _selectAll;
}

const QPixmap* ImageCache::getSelectNone()
{
    return _selectNone;
}

const QPixmap* ImageCache::getSelectSome()
{
    return _selectSome;
}

const QPixmap* ImageCache::getTape()
{
    return _tape;
}

const QPixmap* ImageCache::getTapeMounted()
{
    return _tapeMounted;
}

const QPixmap* ImageCache::getTapeUnmounted()
{
    return _tapeUnmounted;
}

const QPixmap* ImageCache::getVerify()
{
    return _verify;
}
