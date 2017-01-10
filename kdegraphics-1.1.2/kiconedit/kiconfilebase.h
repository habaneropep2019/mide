// -*- c++ -*-
/*  
    KDE Icon Editor - a small graphics drawing program for the KDE

    Copyright (C) 1998 Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef KICONFILEBASE_H
#define KICONFILEBASE_H

#include <qpixmap.h>
#include <kfileinfo.h>

class KIconFileBase
{
public:
    KIconFileBase();
    virtual ~KIconFileBase();
    
protected:
    bool checkXVFile(const KFileInfo *i, const char *filepath, QPixmap *pixmap);
    bool scalePixmap(QPixmap *pixmap);

    static QPixmap *pix_folder;
    static QPixmap *pix_lfolder;
    static QPixmap *pix_file;
    static QPixmap *pix_lfile;
};

#endif // KICONFILEBASE_H
