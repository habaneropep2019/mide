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

#include <kimgio.h>
#include "debug.h"
#include "utils.h"
#include "../config.h"

imageFormats *formats = 0L;

void setupImageHandlers()
{
  if(formats != 0L)
    return;

  kimgioRegister();

  debug("Initializing formats");
  formats = new imageFormats;
  CHECK_PTR(formats);
  formats->setAutoDelete(true);
  formats->append(new imageFormat("GIF", "Gif",  "gif"));
#ifdef HAVE_LIBJPEG
  formats->append(new imageFormat("JFIF", "JPEG", "jpg"));
#endif
  formats->append(new imageFormat("XPM", "XPM",  "xpm"));
/*
#ifdef HAVE_LIBJPEG
  QImageIO::defineIOHandler("JFIF","^\377\330\377\340", 0, read_jpeg_jfif, NULL);
#endif
*/
}

// Simple copy operation on local files (isn't there something like this in the libs?)
bool copyFile(const QString &src, const QString &dest)
{
  QFile f_src(src.data());  
  QFile f_dest(dest.data());  
  QFileInfo fi(f_src);
  uint src_size = fi.size();
  debug("Size: %u", src_size);

  if ( f_src.open(IO_ReadOnly) )
  {    // file opened successfully      
    if ( !f_dest.open(IO_WriteOnly) )
    {
      debug(i18n("copyFile - There was an error opening destination file: %s"), dest.data());
      f_src.close();
      return false;
    }
    char *data = new char[src_size];
    if(f_src.readBlock(data, src_size) == -1)
    {
      debug(i18n("copyFile - There was an error reading source file: %s"), src.data());
      f_src.close();
      f_dest.close();
      delete [] data;
      return false;
    }
    if(f_dest.writeBlock(data, src_size) == -1)
    {
      debug(i18n("copyFile - There was an error writing to destination file: %s"), dest.data());
      f_src.close();
      f_dest.close();
      delete [] data;
      return false;
    }

    f_src.close();
    f_dest.close();
    delete [] data;
    refreshDirectory(dest);
    return true;
  }
  debug(i18n("copyFile - There was an error opening source file: %s"), src.data());
  return false;
}

bool removeFile(const QString &file)
{
  if(file.length() > 0 && QFile::exists(file.data()))
  {
    QDir d;
    debug("Removing %s", file.data());
    if(!d.remove(file.data()))
    {
      debug(i18n("removeFile - There was an error removing the file: %s"), file.data());
      return false;
    }
    refreshDirectory(file);
    return true;
  }
  return false;
}

bool moveFile(const QString &src, const QString &dest)
{
  if(copyFile(src, dest))
    return removeFile(src);
  return false;
}

// lets KFM refresh the directory. "path" can be a directory or a file.
void refreshDirectory(const QString &path)
{
    KFM *k = new KFM;
    CHECK_PTR(k);
    if(k->isOK())
    {
      QFileInfo fi(path.data());
      QString d = "file:" + fi.dirPath(true) + "/";
      k->refreshDirectory(d.data());
    }
    delete k;
}

uint kdeColor(uint color)
{
  uint c = iconpalette[0]|OPAQUE_MASK;

  for(uint i = 0; i < 42; i++)
  {
    //debug("Color #%d %u", i, iconpalette[i]);
    if( (iconpalette[i]|OPAQUE_MASK) - c < (iconpalette[i]|OPAQUE_MASK) - color)
      c = iconpalette[i]|OPAQUE_MASK;
  }
  //debug("%u -> %u", color, c);
  return c;
}

