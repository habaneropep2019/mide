/*
   Copyright (c) 1997-1998 Christian Esken (esken@kde.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


// $Id$
// User interface actions


#include <unistd.h>
#include <config.h>
#include <klocale.h>
#include <kfiledialog.h>
#include "kmediaui.h"
#include "kmediawin.h"
#include "version.h"

extern KApplication	*globalKapp;
QList<KMediaWin> KMediaWin::allMediaWins;



void KMediaWin::quitAll()
{
  KMediaWin *kmw;
  for (kmw=allMediaWins.first(); kmw != 0; kmw = allMediaWins.next() ) {
    if ( kmw != this )
      delete kmw;
  }

  delete this;
  exit(0);
}

void KMediaWin::quit()
{
  //  if ( allMediaWins.count() > 1 )
  delete this;
  //  else
  //    quitAll();
}

/* Martin Jones told me the CloseEvent trick. Thank you!
 * This */
void KMediaWin::closeEvent( QCloseEvent * )
{
  quit();
}

bool KMediaWin::removePlayer()
{
  if (!MaudioLaunched)
    return true;

  /* Press the exit "button". This tells the slave to quit */
  EventCounterRaise(&(KeysChunk->exit),1);
  /* Await end of player */
  for (int i=0; i<4; i++)
    {
      if (*StatStatPtr == MD_STAT_EXITED)
	break;
      else
	{
	  sleep(1);
	}
    }
  if (*StatStatPtr == MD_STAT_EXITED)
    {
      MaudioLaunched = false;
      return true;
    }
  else
    return false;
}


void KMediaWin::ejectClicked()
{
}

void KMediaWin::trackSelected( int /*trk*/ )
{
}




void KMediaWin::onDrop( KDNDDropZone* _zone )
{
  QStrList strlist;

  strlist = _zone->getURLList();
  KURL *url = new KURL( strlist.first() );
  QString urlQstr  = url->path();
  QString &urlQref = urlQstr;
  KURL::decodeURL(urlQref);

  FileNameSet( FnamChunk, urlQstr.data());
  launchPlayer(urlQstr.data());

  delete url;
}


void KMediaWin::TimerFunctions()
{
  if (TimerAction == NOP)
    {
      /* If nothing is to be done: Reset everything to default values */
      InitialDelay = -1;
      return;
    }

  if ( InitialDelay < 0 )
    {
      /* Here we come only, when TimerAction!=NOP, and last time it was NOP */
      InitialDelay =  4;
      RepeatSpeed  =  1;
      return;
    }
  else if ( InitialDelay > 0 )
    {
      /* Here we come, when we are in "delay" mode. */
      InitialDelay --;
      return;
    }

  RepeatSpeed--;
  if ( RepeatSpeed >0 )
    return;

  /* Default repeat speed */
  RepeatSpeed = 1;

  switch (TimerAction) {
    case FF:	
      EventCounterRaise(&(KeysChunk->forward),1);
      break;
    case REW:
      EventCounterRaise(&(KeysChunk->backward),1);
      break;
    case NEXTTRACK:
      EventCounterRaise(&(KeysChunk->nexttrack),1);
      RepeatSpeed  =  1;
      break;
    case PREVTRACK:
      EventCounterRaise(&(KeysChunk->prevtrack),1);
      RepeatSpeed  =  1;
      break;
    default:
      break;
    }
}


void KMediaWin::pllClicked()
{
   char PlayLoopInfo[]="\"KDE is an acronym for 'KDE doesn't emulate'.\n" \
        "\nKeep in mind KDE does not look like Windows\" (Christian)\n";
   QMessageBox::about( NULL, "KDE doesn't emulate (Windows)", PlayLoopInfo);
   QMessageBox::about( NULL, "Much fun with KDE", "Much fun with KDE wishes\n" \
   "               Christian :-)");
}

void KMediaWin::PosChanged( int new_pos )
{
  // Remember new position, in case it's a a user initiated position change.
  if (newPosValid)
  {
    newPos = new_pos;
    //cerr << "Changing to new position\n";
    (KeysChunk->pos_new) = newPos;
    EventCounterRaise(&(KeysChunk->posnew),1);
  }
  else {
    newPosValid = true;
    //cerr << "Ignoring changedPosition()\n";
  }
}

void KMediaWin::endSelMediapos()
{
}


void KMediaWin::newviewClicked()
{
  KMediaWin *newkmedia;
  newkmedia = new KMediaWin(NULL,"Window 2");
  newkmedia->show();
}

void KMediaWin::openClicked()
{
  QString fname( KFileDialog::getOpenFileURL(0, "*.wav") );
  if (fname.isNull() )
    return;

  KURL *url = new KURL( fname );
  QString urlQstr  = url->path();
  QString &urlQref = urlQstr;
  KURL::decodeURL(urlQref);

  FileNameSet( FnamChunk, urlQstr.data() );
  launchPlayer(urlQstr.data() );

  delete url;
}



void KMediaWin::launchPlayer(const char *filename)
{
  
  char* Opts[10];
  char MaudioText[]  ="maudio";
  char MmidiText[]   ="mplaymidi";
  char MtrackerText[]="mtracker";
  char DeviceText[]  ="-devnum";
  char MediaText[]   ="-media";
  char MediaText2[]  ="-m";
  char TempString[256];
  char TempString2[10];

  enum {myAudio,myMidi,myTracker,myOther};
  int  ftype=myAudio;

  if (MaudioLaunched)
    {
      // already launched? then wait for termination of old player first
      bool exitOK = removePlayer();
      if (!exitOK)
	fprintf(stderr,"Player does not quit!!!\n");
      else
	MaudioLaunched = false;
    }


  // Do a stupid file type recognition
  int fnlen = strlen(filename);
  const char *filename_start;
  filename_start = strrchr(filename,'/');
  if (filename_start==NULL)
    filename_start = filename;
  else
    filename_start ++;

  ftype = myOther;
  if ( fnlen > 4)
   {
      if (! strcasecmp(filename+fnlen-4, ".mid") )
	ftype=myMidi;
      else if ( (strcasecmp(filename+fnlen-4, ".mod")==0) || (strcasecmp(filename+fnlen-7, ".mod.gz")==0) || (strncasecmp(filename_start, "mod.",4)==0))
	ftype=myTracker;
      else
	ftype=myAudio;
    }
  pid_t myPid = fork();

  if ( myPid == 0 )
    { /* Child process */
      sprintf(TempString, "%i", IhdrChunk->ref);
      switch (ftype)
	{
	case myMidi:	Opts[0]=MmidiText   ; Opts[1]=MediaText2; break;
	case myTracker:	Opts[0]=MtrackerText; Opts[1]=MediaText ; break;
	case myAudio:	Opts[0]=MaudioText  ; Opts[1]=MediaText ; break;
	default:	KMsgBox::message(0, "Kmedia", "Unknown media type", KMsgBox::INFORMATION, "OK" );
	}
      int arg=2;
      Opts[arg++]=TempString;
      if ( ftype == myAudio ) {
        sprintf(TempString2, "%i", prefDL->useDevice());
        Opts[arg++]=DeviceText;
        Opts[arg++]=TempString2;
      }
      Opts[arg++]=NULL;
      execvp( Opts[0], /*(char *const )*/ Opts);

      fprintf(stderr,"Could not start maudio!\n");
      exit( 1 );
    }
  else
    {
      if (myPid>0)
	/* Process that forked the child */
	MaudioLaunched=true;
      else
	{
	  fprintf(stderr,"Could not fork!\n");
	  MaudioLaunched=false;
	}
    }    

}



void KMediaWin::launchHelp()
{
  globalKapp->invokeHTMLHelp("", "");
}

void KMediaWin::launchMixer()
{
  if( prefDL->mixerCommand.isEmpty() )
    {
      KMsgBox::message(0, "Error",\
		       "Please set mixer in preferences first.",
		       KMsgBox::INFORMATION, "OK" );
    }
  else
    {
      if ( fork() == 0 )
	{
	  execlp( prefDL->mixerCommand, prefDL->mixerCommand, 0 );
	  fprintf(stderr,"Error: Starting mixer failed.\n");
	  exit( 1 );
	}
    }
}

void KMediaWin::hideMenu()
{
}

