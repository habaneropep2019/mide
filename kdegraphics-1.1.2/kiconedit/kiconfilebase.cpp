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

#include <qdir.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <kapp.h>
#include "kiconfilebase.h"
#include "xview.h"

QPixmap *KIconFileBase::pix_folder = 0;
QPixmap *KIconFileBase::pix_lfolder = 0;
QPixmap *KIconFileBase::pix_file = 0;
QPixmap *KIconFileBase::pix_lfile = 0;

KIconFileBase::KIconFileBase()
{
  //debug("KIconFileBase - constructor");

  if(!pix_folder) 
    pix_folder = new QPixmap(KApplication::kde_icondir() + "/folder.xpm"); 
  if(!pix_lfolder) 
    pix_lfolder = new QPixmap(KApplication::kde_icondir() + "/lockedfolder.xpm"); 
  if(!pix_file) 
    pix_file = new QPixmap(KApplication::kde_icondir() + "/unknown.xpm"); 
  if(!pix_lfile) 
    pix_lfile = new QPixmap(KApplication::kde_icondir() + "/locked.xpm"); 

}

KIconFileBase::~KIconFileBase()
{
  debug("KIconFileBase - destructor");

  if(pix_folder) 
    delete pix_folder;
  pix_folder = 0;
  if(pix_lfolder) 
    delete pix_lfolder;
  pix_lfolder = 0;
  if(pix_file) 
    delete pix_file;
  pix_file = 0;
  if(pix_lfile) 
    delete pix_lfile;
  pix_lfile = 0;

}

bool KIconFileBase::checkXVFile(const KFileInfo *i, const char *filepath, QPixmap *pixmap)
{
   bool hasimage = false;

   if( !pixmap->load( filepath ))
     return false;

   QString path = filepath;
   int index = path.find( i->fileName() );
   path.insert(index,".xvpics/");

   QFileInfo fi(path.data());

   QDir d(fi.dirPath());

   if( !d.exists() )
   {
     if( !d.mkdir(d.path()) )
     {
       /*
       if(!scalePixmap(pixmap))
         return false;
       return true;
       */
     }
   }

   if( !fi.exists() )
   {
     hasimage = scalePixmap(pixmap);
     if(!pixmap->save(path.data(), "XV"))
       debug("Error saving XV thumbnail %s", path.data());
   }

   if( fi.exists() && !hasimage )  
   {
     if( !pixmap->load( path.data(), "XV" ))
     {
       debug("Error loading XV thumbnail %s", path.data());
       return false;
     }
   }

   if(!hasimage)
     return scalePixmap(pixmap);

   return true;
}

bool KIconFileBase::scalePixmap(QPixmap *pixmap)
{
  int w, h;
    
  if ( pixmap->width() > pixmap->height() )
  {
    if ( pixmap->width() < 80 )
      w = pixmap->width();
    else
      w = 80;

    h = (int)( (float)pixmap->height() * ( (float)w / (float)pixmap->width() ) );
  }
  else
  {
    if ( pixmap->height() < 60 )
      h = pixmap->height();
    else
      h = 60;

    w = (int)( (float)pixmap->width() * ( (float)h / (float)pixmap->height() ) );
  }
    
  QWMatrix matrix;
  matrix.scale( (float)w/pixmap->width(), (float)h/pixmap->height() );
  *pixmap = pixmap->xForm( matrix );   

  if(pixmap->isNull())
    return false;
  return true;
}


