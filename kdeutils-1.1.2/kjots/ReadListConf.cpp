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

#include <qapp.h>
#include <qstring.h>
#include <qstrlist.h>

#include <kconfig.h>

int readListConf ( KConfig *conf, QString key, QStrList &list )
{
  if( !conf->hasKey( key ) )
    { 
      // debug("readListConf:: key does not exist");
      return 0;
    }
  QString str_list, value;
  str_list = conf->readEntry(key);
  if(str_list.isEmpty())
    {
      // debug("readListConf:: list is empty"); 
      return 0; 
    }
  list.clear();
  int i;
  int len = str_list.length();
  for( i = 0; i < len; i++ )
    {
      if( str_list[i] != ',' && str_list[i] != '\\' )
	{
	  value += str_list[i];
	  continue;
	}
      if( str_list[i] == '\\' )
	{
	  i++;
	  value += str_list[i];
	  continue;
	}
      list.append(value);
      value.truncate(0);
    }
  list.append(value);
}

void writeListConf ( KConfig *conf, QString key, QStrList &list )
{
  if( list.isEmpty() )
    {
      conf->writeEntry(key, "");      
      return;
    }
  QString str_list;
  QString value;
  int i;
  for( value = list.first(); value != ""; value = list.next() )
    {
      for( i = 0; i < (int) value.length(); i++ )
	{
	  switch( value[i] ) {
	  case ',':
	    str_list += '\\';
	    break;
	  case '\\':
	    str_list += '\\';
	    break;
	    //defaults:
	    //;
	  };
	  str_list += value[i];
	}
      str_list += ',';
    }
  if( str_list.right(1) == "," )
    str_list.truncate(str_list.length()-1);
  conf->writeEntry(key, str_list);
}

