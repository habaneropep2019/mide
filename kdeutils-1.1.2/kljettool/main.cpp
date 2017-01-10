    /*

    $Id: main.cpp,v 1.4 1999/01/10 00:52:22 bieker Exp $

    Requires the Qt widget libraries, available at no cost at 
    http://www.troll.no
       
    Copyright (C) 1997 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */
 

#include <qdir.h>
#include "main.h"
#include <qfileinf.h>
#include <qstring.h>
#include <qfile.h>
#include <qtstream.h>
#include "main.moc"
#include <kapp.h>
#include <klocale.h>

KApplication* mykapp;

LJET::LJET(){


        data = new Data;
	data->setPrinter("lp");
	
	mainWindow = new QTabDialog( 0, 0, TRUE );

	mainWindow->setCaption( i18n("Ljet Tool") );
	mainWindow->setCancelButton( i18n("Cancel") );
	mainWindow->setApplyButton( i18n("Apply") );

	connect(mainWindow,SIGNAL(applyButtonPressed()), this, SLOT(apply()));
       	paper = new  Paper(mainWindow,"paper");
	paper->setData(data);
	paper->setWidgets();
       	fonts = new Fonts(mainWindow,"fonts");
	fonts->setData(data);
	fonts->setWidgets();

	printer = new Printer(mainWindow,"printer");
	printer->setData(data);
	printer->setWidgets();

	misc = new Misc(mainWindow,"misc");
	misc->setData(data);
	misc->setWidgets();

      	mainWindow->addTab( paper, i18n("Paper") );
	mainWindow->addTab( printer, i18n("Printer"));
	mainWindow->addTab( misc, i18n("Operations"));

      	mainWindow->addTab( fonts, i18n("Fonts") );
	mainWindow->resize(355,325);
	mainWindow->setFixedSize(355,325);

}

LJET::~LJET(){

}

void LJET::apply(){

  data->print();
  data->writeSettings();
}




void LJET::addPrinter(char* name){

  printer->addPrinter(name);

}

int main( int argc, char *argv[] ){


  KApplication a( argc, argv, "kljettool" );
  mykapp = &a;
  LJET* ljet = new LJET;

    
  QString progname, choice;

  /*  choice = (QString) getenv("PRINTER");
  char c;

  while ( ( c = getopt (argc, argv, "hP:")) != -1)
    {
      switch (c)
        {
        case 'h':
          debug("usage: kljettool [ [-P printername] ]");
          exit(1);
        case 'P':
          choice = (QString) optarg;
          break;
        case '?':
          break;
        default:
          exit (1);
        }
    }

  if ( optind < argc )
    {
      progname = (QString) "lpr ";
      if ( choice != "" )
	{
	  progname += "-P " + choice + " ";
	}
    }
    */
  QString temp, name;
  bool empty_printcap = TRUE;

  QFile printcap("/etc/printcap");
  if( !printcap.open(IO_ReadOnly) )
    {
      debug("Sorry -- can't open your /etc/printcap file.");
      exit(1);
    }
   
// this section 'stolen' from Christopher Neerfeld's klpq

  QTextStream st( (QIODevice *) &printcap);
  while( !st.eof() )
    {
      temp = st.readLine();
      if( temp[0] == '#' || temp.isEmpty() ) 
	continue;
      name = temp.left( temp.find(':') );
      if( name.isEmpty() ) 
	continue;
      //debug("name = %s", (const char *) name);
      if( name.contains('|') )
	name = name.left( name.find('|') );
      name.stripWhiteSpace();
      if( name.isEmpty() )
	continue;
      else
	empty_printcap = FALSE;
      while( temp.right(1) == (QString) "\\")
	{
	  temp = st.readLine();
	  if( temp[0] == '#')
	    temp = "\\";
	}

        ljet->addPrinter(name.data());

    }

  printcap.close();

  if( empty_printcap )
    {
      debug("Sorry -- unable to parse your /etc/printcap.");
      debug("Please email your printcap to wuebben@math.cornell.edu");
      exit(1);
    }

  ljet->mainWindow->exec();

  return 0;
}



