// -*- C++ -*-

//
//  kjots
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <qapp.h>
#include <qdir.h>
#include <qfileinf.h>

#include <kapp.h>
#include <drag.h>
#include <kiconloader.h>

#include "KJotsMain.h"

KJotsMain *main_widget;
KIconLoader *global_pix_loader;

QString exec_http;
QString exec_ftp;

#include <dirent.h>
#include <sys/stat.h>

// Torben
void testDir( const char *_name )
{
  DIR *dp;
  QString c = KApplication::localkdedir();
  c += _name;
  dp = opendir( c.data() );
  if ( dp == NULL )
    ::mkdir( c.data(), S_IRWXU );
  else
    closedir( dp );
}

int main( int argc, char **argv )
{
  //debug ( "[kjots] started-------------------------" );

  KApplication a( argc, argv, "kjots" );

  // Torben
  testDir( "" );
  testDir( "/share" );  
  testDir( "/share/config" );  
  testDir( "/share/apps" );
  testDir( "/share/apps/kjots" );

  QString temp1, temp2;
  KConfig *config = a.getConfig();
  config->setGroup("kjots");
  if( !config->hasKey("execHttp") )
    config->writeEntry("execHttp", "kfmclient openURL %u");
  if( !config->hasKey("execFtp") )
    config->writeEntry("execFtp", "kfmclient openURL %u");
  if( !config->hasKey("EFontFamily") )
    config->writeEntry("EFontFamily", "helvetica");
  if( !config->hasKey("EFontSize") )
    config->writeEntry("EFontSize", 12);
  if( !config->hasKey("EFontWeight") )
    config->writeEntry("EFontWeight", 0);
  if( !config->hasKey("EFontItalic") )
    config->writeEntry("EFontItalic", 0);
  if( !config->hasKey("EFontCharset") )
    config->writeEntry("EFontCharset", 0);
  config->sync();
  global_pix_loader = new KIconLoader();
  KJotsMain jots;
  if( a.isRestored() )
    {
      if( KTopLevelWidget::canBeRestored(1) )
	{
          jots.restore(1);
	}
    }
  main_widget = &jots;
  a.setMainWidget( (QWidget *) &jots );
  a.setRootDropZone( new KDNDDropZone( (QWidget *) &jots, DndNotDnd ) );
  jots.show();
  jots.resize(jots.size());
  return a.exec();
}








