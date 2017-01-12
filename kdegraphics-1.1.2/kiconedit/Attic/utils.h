/*  
    kiconedit - a small graphics drawing program for the KDE

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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <qimage.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif // HAVE_CONFIG_H

#include <qdir.h>
#include <qfile.h>
#include <qfileinf.h>
#include <kapp.h>
#include <kfm.h>

#define OPAQUE_MASK 0xff000000

#ifdef TRANSPARENT
#undef TRANSPARENT
#endif
#define TRANSPARENT kapp->backgroundColor.rgb()

const uint iconpalette[42] = {  // kde palette
	0x303030,0x585858,0x808080,0xa0a0a0,0xc3c3c3,0xdcdcdc,
	0x000040,0x004000,0x000000,0x004040,0x404000,0x000000,
	0x000080,0x008000,0x800000,0x008080,0x808000,0x800080,
	0x0000c0,0x00c000,0xc00000,0x00c0c0,0xc0c000,0xc000c0,
	0x0000ff,0x00ff00,0xff0000,0x00ffff,0xffff00,0xff00ff,
	0xc0c0ff,0xc0ffc0,0xffc0c0,0xc0ffff,0xffffc0,0xffc0ff,
	0x0080ff,0x0058c0,0x58a8ff,0xa8dcff,0xffffff,0x000000};

struct imageFormat
{
  imageFormat(const char *f, const char *t, const char *e) { format = f; title = t, extension = e;}
  const char *format;
  const char *title;
  const char *extension;
};

typedef QList<struct imageFormat> imageFormats;
extern imageFormats *formats;

void setupImageHandlers();

bool copyFile(const QString &src, const QString &dest);
bool removeFile(const QString &file);
bool moveFile(const QString &src, const QString &dest);
void refreshDirectory(const QString &path);
uint kdeColor(uint c);

#endif //__UTILS_H__



