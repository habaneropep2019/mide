// -*- C++ -*-

//
//  klpq
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

extern "C" {
#include <unistd.h>
#include <signal.h>
};

#include <qapp.h>
#include <qdir.h>
#include <qfileinf.h>
#include <qstring.h>
#include <qfile.h>
#include <qtstream.h>
#include <qstrlist.h>

#include <kapp.h>
#include <drag.h>

#include "klpq.h"

Klpq *main_widget;

int main( int argc, char **argv )
{
  KApplication a( argc, argv, "klpq" );

  KConfig *config = a.getConfig();
  config->setGroup("klpq");

  QString progname, choice, version, last_printer;
  QStrList file_list;
  choice = (QString) getenv("PRINTER");
  last_printer = config->readEntry("lastQueue", "lp");
  char c;
  bool popup = TRUE;
  while ( ( c = getopt (argc, argv, "nvhP:")) != -1)
    {
      switch (c)
        {
        case 'h':
          debug("usage: klpq [ [-n] [-P printername] file ]");
	  debug("-n: don't popup klpq; just send the file to spooler.");
          exit(1);
        case 'P':
          choice = (QString) optarg;
          break;
        case 'n':
	  popup = FALSE;
          break;
        case '?':
          break;
	case 'v':
	  version = "Klpq v";
	  version += (QString) KLPQ_VERSION;
	  debug("%s Copyright 1997,98 Christoph Neerfeld.\n"
"Christoph.Neerfeld@home.ivm.de or chris@kde.org", (const char *) version);
	  exit(1);
          break;
        default:
	  exit(1); 
        }
    }
  if( optind < argc )
    {
      int i;
      QString file;
      for( i = optind; i < argc; i++ )
	{
	  progname = (QString) "lpr ";
	  if ( !choice.isNull() )
	    {
	      progname += "-P \"" + choice + "\" ";
	    }
	  file = (QString) argv[i];
	  if( file.left(5) == "file:" )
	    {
	      progname += file.remove(0,5);
	      system((const char *) progname);
	    }
	  else if( file.left(5) == "http:" || file.left(4) == "ftp:" )
	    {
	      file_list.append( (QString) choice + ":" + file);
	    }
	  else
	    {	    
	      progname += (QString) argv[i];
	      system((const char *) progname);
	    }
	}
    }
  else
    popup = TRUE;
  Klpq klpq;
  if( file_list.count() != 0 )
    {
      klpq.printRemote(file_list);
    }
  else if( !popup )
    exit(0);
  main_widget = &klpq;

  QString temp, name;
  bool empty_printcap = TRUE;

  bool no_printcap = TRUE;
  int iprintcap=0,nprintcap=1;
  char pcpath[3][80]={"/etc/printers.conf", "/etc/printcap",
                      "/usr/local/etc/printcap"};
  while (iprintcap <= nprintcap)
    {
      QFile printcap(pcpath[iprintcap]);
      if( !printcap.open(IO_ReadOnly) )
	{
	  iprintcap++;
	  continue;
	}
      no_printcap = FALSE;
      QTextStream st( (QIODevice *) &printcap);
      while( !st.eof() )
	{
	  temp = st.readLine();
	  temp = temp.stripWhiteSpace();
	  if( temp[0] == '#' || temp.isEmpty() )
	    continue;
	  name = temp.left( temp.find(':') );
	  if( name.isEmpty() )
	    continue;
	  //debug("name = %s", (const char *) name);
	  if( name.contains('|') )
	    name = name.left( name.find('|') );
	  name = name.stripWhiteSpace();
	  if( name.isEmpty() )
	    continue;
	  else
	    empty_printcap = FALSE;
	  /*
	  do {
	    temp = st.readLine();
	    temp = temp.stripWhiteSpace();
	    if( temp[0] == '#')
	      temp = ":";
	  } while( temp.simplifyWhiteSpace().left(1) == (QString) ":");
	  */
          if(name.right(1) == "\\" )
            name = name.left(name.length() - 1);
	  klpq.addPrintQueue(name);
	  if( choice.isEmpty() && name == last_printer )
	    klpq.setLastPrinterCurrent();
	  if( name == choice)
	    klpq.setLastPrinterCurrent();
	}
      printcap.close();
      iprintcap++;
    }
  if( no_printcap )
    {
      debug(klocale->translate("Sorry, can't open your printcap file."));
      exit(1);
    }
  if( empty_printcap )
    {
      debug(klocale->translate("Sorry, unable to parse your printcap."));
      debug(klocale->translate("Please email your printcap to Christoph.Neerfeld@home.ivm.de"));
      exit(1);
    }

  a.setMainWidget( (QWidget *) &klpq );
  a.setRootDropZone( new KDNDDropZone( (QWidget *) &klpq, DndNotDnd ) );
  klpq.callUpdate();
  if( popup )
    klpq.show();
  klpq.resize(klpq.size());
  return a.exec();
}





