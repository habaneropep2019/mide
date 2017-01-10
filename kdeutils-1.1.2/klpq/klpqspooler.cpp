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

#include <qfileinf.h>
#include <qlistbox.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qchkbox.h>

#include <kapp.h>
#include <kprocess.h>

#include "queueview.h"
#include "klpqspooler.h"


void Spooler::setPrintingCommand( KProcess *proc, QString queue, bool b )
{
  if( b )
    *proc << lpc_path << "start" << queue;
  else
    *proc << lpc_path << "stop" << queue;
}

void Spooler::statusCommand( KProcess *proc, QString queue )
{
  *proc << lpc_path << "status" << queue;
}

void Spooler::updateCommand( KProcess *proc, QString queue )
{
  *proc << lpq_path << "-P" << queue;
}

void Spooler::setQueuingCommand( KProcess *proc, QString queue, bool b )
{
  if( b )
    *proc << lpc_path << "enable" << queue;
  else
    *proc << lpc_path << "disable" << queue;
}

QString Spooler::removeCommand( QString queue, QString id )
{
  QString temp;
  temp = lprm_path + " -P " + queue + " " + id;
  return temp;
}

void Spooler::makeTopCommand( KProcess *proc, QString queue, QString id )
{
  *proc << lpc_path << "topq" << queue << id;
}

void Spooler::parseUpdate( MyRowTable *lb_list, QListBox *lb_status, QString lpq_in_buffer )
{
  QString temp;
  unsigned int i = 0, j = 0;
  unsigned char c;
  for( j = 0; j < lpq_in_buffer.length(); j++ )
    {
      c = lpq_in_buffer[j];
      if ( c != '\n' )
	temp += c;
      else
	{
	  if(temp.contains("Rank"))
	     { i = 1; temp = ""; continue; }
	  if( temp.length() == 0 )
	    { temp = ""; continue; }
	  if(i == 0) 
	    {
	      lb_status->insertItem(temp);
	    }
	  else
	    {
	      lb_list->insertItem(temp);
	    }
	  temp = "";
	}
    }
}

int Spooler::parsePrinting( char *buffer, int len )
{
  unsigned char c;
  int j = 0;
  QString temp;
  while( (c = *(buffer+j)) && j < len )
    {
      j++;
      if ( c >= 128) break;
      if ( c != '\n' )
	temp += c;
      else
	{
	  if(temp.contains("Privileged"))
	    return -1;
	  temp = "";
	  }
    }
  return 0;
}

int Spooler::parseQueuing( char *buffer, int len )
{
  unsigned char c;
  int j = 0;
  QString temp;
  while( (c = *(buffer+j)) && j < len )
    {
      j++;
      if ( c >= 128) break;
      if ( c != '\n' )
	temp += c;
      else
	{
	  if(temp.contains("Privileged"))
	    return -1;
	  temp = "";
	}
    }
  return 0;
}

void Spooler::parseStatus( char *buffer, int len, QCheckBox *c_queuing, QCheckBox *c_printing )
{
  unsigned char c;
  int j = 0;
  QString temp;
  while( (c = *(buffer+j)) && j < len )
    {
      j++;
      if ( c >= 128) break;
      if ( c != '\n' )
	temp += c;
      else
	{
	  if(temp.contains("queuing"))
	    { 
	      if(temp.contains("disabled"))
		{ c_queuing->setChecked(FALSE); }
	      else
		{ c_queuing->setChecked(TRUE); }
	      temp = ""; continue; 
	    }
	  else if (temp.contains("printing"))
	    { 
	      if(temp.contains("disabled"))
		{ c_printing->setChecked(FALSE); }
		else
		  { c_printing->setChecked(TRUE); }
	      temp = ""; continue; 
	    }
	  temp = "";
	}
    }
}

// ------------------------------------------

SpoolerBsd::SpoolerBsd()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpqBsd");
  lpc_path = config->readEntry("lpcCommand");
  QFileInfo fi;
  fi.setFile(lpc_path);
  if( !fi.isExecutable() )
    {
      fi.setFile("/usr/sbin/lpc");
      lpc_path = "/usr/sbin/lpc";
      if( !fi.isExecutable() )
	{
	  fi.setFile("/usr/bin/lpc");
	  lpc_path = "/usr/bin/lpc";
	  if( !fi.isExecutable() )
	    {
	      QMessageBox::warning( 0, klocale->translate("Can't find your lpc program !"),
				    klocale->translate("Sorry, I can't find your lpc program.\n"
"Please check the configuration under Config->Spooler.") );
	      lpc_path = "/usr/sbin/lpc";
	    }
	}
    }
  lpq_path = config->readEntry("lpqCommand", "lpq" );
  lprm_path = config->readEntry("lprmCommand", "lprm" );
  config->writeEntry("lpcCommand", lpc_path);
}

//----------------------------------------------------------------

SpoolerPpr::SpoolerPpr()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpqPpr");
  lpc_path = config->readEntry("lpcCommand", "ppop" );
  lpq_path = config->readEntry("lpqCommand", "ppop" );
  lprm_path = config->readEntry("lprmCommand", "ppop" );
  config->writeEntry("lpcCommand", lpc_path);
}

void SpoolerPpr::updateCommand( KProcess * proc, QString queue )
{
  *proc << lpq_path << "lpq" << queue;
}

void SpoolerPpr::setQueuingCommand( KProcess *proc, QString queue, bool b )
{
  if( b )
    *proc << lpc_path << "accept" << queue;
  else
    *proc << lpc_path << "reject" << queue;
}

QString SpoolerPpr::removeCommand( QString queue, QString id )
{
  QString temp;
  temp = lprm_path + " cancel " + queue + "-" + id;
  return temp;
}

void SpoolerPpr::makeTopCommand( KProcess *proc, QString queue, QString id )
{
  QString temp;
  temp = (QString) "rush " + queue + "-" + id;
  *proc << lpc_path << (const char *) temp;
}

// ------------------------------------------

SpoolerLprng::SpoolerLprng()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpqLprng");
  lpc_path = config->readEntry("lpcCommand");
  QFileInfo fi;
  fi.setFile(lpc_path);
  if( !fi.isExecutable() )
    {
      fi.setFile("/usr/sbin/lpc");
      lpc_path = "/usr/sbin/lpc";
      if( !fi.isExecutable() )
	{
	  fi.setFile("/usr/bin/lpc");
	  lpc_path = "/usr/bin/lpc";
	  if( !fi.isExecutable() )
	    {
	      QMessageBox::warning( 0, klocale->translate("Can't find your lpc program !"),
				    klocale->translate("Sorry, I can't find your lpc program.\n"
"Please check the configuration under Config->Spooler.") );
	      lpc_path = "/usr/sbin/lpc";
	    }
	}
    }
  lpq_path = config->readEntry("lpqCommand", "lpq" );
  lprm_path = config->readEntry("lprmCommand", "lprm" );
  config->writeEntry("lpcCommand", lpc_path);
}

void SpoolerLprng::parseUpdate( MyRowTable *lb_list, QListBox *lb_status, QString lpq_in_buffer )
{
  QString temp;
  unsigned int i = 0, j = 0, k = 0, start = 0;
  unsigned char c;
  for( j = 0; j < lpq_in_buffer.length(); j++ )
    {
      c = lpq_in_buffer[j];
      if ( c != '\n' )
	temp += c;
      else
	{
	  if( temp.contains("Rank") )
	    { i = 1; temp = ""; continue; }
	  if( temp.length() == 0 )
	    { temp = ""; continue; }
	  if( i == 0 ) 
	    {
	      lb_status->insertItem(temp);
	    }
	  else
	    {
	      temp = temp.simplifyWhiteSpace();
	      k = start = 0;
	      while( temp[k] != ' ' && temp[k] != 0 ) // skip rank
		k++;
	      k++;
	      while( temp[k] != ' ' && temp[k] != 0 ) // skip owner
		k++;
	      k++;
	      start = k;
	      while( temp[k] != ' ' && temp[k] != 0 )
		k++;
	      temp = temp.remove(start, k-start); // remove class
	      lb_list->insertItem(temp);
	    }
	  temp = "";
	}
    }
}

int SpoolerLprng::parsePrinting( char *buffer, int len )
{
  unsigned char c;
  int j = 0;
  QString temp;
  while( (c = *(buffer+j)) && j < len )
    {
      j++;
      if ( c >= 128) break;
      if ( c != '\n' )
	temp += c;
      else
	{
	  if(temp.contains("permission"))
	    return -1;
	  temp = "";
	  }
    }
  return 0;
}

int SpoolerLprng::parseQueuing( char *buffer, int len )
{
  unsigned char c;
  int j = 0;
  QString temp;
  while( (c = *(buffer+j)) && j < len )
    {
      j++;
      if ( c >= 128) break;
      if ( c != '\n' )
	temp += c;
      else
	{
	  if(temp.contains("permission"))
	    return -1;
	  temp = "";
	}
    }
  return 0;
}

void SpoolerLprng::parseStatus(char *buffer, int len, QCheckBox *c_queuing, QCheckBox *c_printing)
{
  unsigned char c;
  int j = 0;
  int i = 0;
  QString temp;
  while( (c = *(buffer+j)) && j < len )
    {
      j++;
      if ( c >= 128) break;
      if ( c != '\n' )
	temp += c;
      else
	{
	  if( !temp.contains("Printer") )
	    {
	      temp = temp.simplifyWhiteSpace();
	      i = temp.find(' ')+1;
	      if( temp.mid(i, 2) == "di" )
		{ c_printing->setChecked(FALSE); }
	      else
		{ c_printing->setChecked(TRUE); }
	      if( temp.mid(temp.find(' ', i)+1, 2 ) == "di" )
		{ c_queuing->setChecked(FALSE); }
	      else
		{ c_queuing->setChecked(TRUE); }
	    }
	  temp = "";
	}
    }
}
