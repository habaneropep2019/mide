/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/*
    The pidfile portions of this code have been shamelessly stolen from kdehelp.
    These parts are (c) 1997 Martin Jones
*/

/****************************************************************
**
** KSysV main loop
**
****************************************************************/

#include <qmsgbox.h>
#include <qlabel.h>
#include "TopWidget.h"
#include <kapp.h>
#include <kiconloader.h>
#include <signal.h>
#include <sys/stat.h>
#include "ksv_ipc.h"
#include "error.h"
#include <kwm.h>
#include <kmsgbox.h>
#include <time.h>
#include "OldView.h"
#include "ksv_core.h"
#include <unistd.h>

static int msgqid = -1;
static QString pidFile;

// remove pid file
static void cleanup( int sig ) {
  printf( "cleanup: signal = %d\n", sig );
  if (msgqid >= 0)
    msgctl( msgqid, IPC_RMID, 0 );
  remove( pidFile );
  exit(0);
}

// segfault handler
static void segHandler( int sig )
{
  printf("Caught SIGSEGV %i, exiting...\n", sig );
  printf("Please mail a bug-report to putzer@kde.org");

  cleanup(sig);
}

// make sure the pid file is cleaned up when exiting unexpectedly.

void catchSignals() {
  //      signal(SIGHUP, cleanup);                /* Hangup */
  signal(SIGINT, cleanup);                /* Interrupt */
  signal(SIGTERM, cleanup);               /* Terminate */
  //      signal(SIGCHLD, cleanup);

  signal(SIGABRT, cleanup);
  signal(SIGALRM, cleanup);
  signal(SIGFPE, cleanup);
  signal(SIGILL, cleanup);
  signal(SIGPIPE, cleanup);
  signal(SIGQUIT, cleanup);
  signal(SIGSEGV, segHandler); // we should exit cleanly at least

#ifdef SIGBUS
  signal(SIGBUS, cleanup);
#endif
#ifdef SIGPOLL
  signal(SIGPOLL, cleanup);
#endif
#ifdef SIGSYS
  signal(SIGSYS, cleanup);
#endif
#ifdef SIGTRAP
  signal(SIGTRAP, cleanup);
#endif
#ifdef SIGVTALRM
  signal(SIGVTALRM, cleanup);
#endif
#ifdef SIGXCPU
  signal(SIGXCPU, cleanup);
#endif
#ifdef SIGXFSZ
  signal(SIGXFSZ, cleanup);
#endif
}


//  timer used to monitor msg queue for request for new window

class Timer : public QObject {
public:
  Timer() {
    startTimer( 100 );
  }

protected:
  virtual void timerEvent( QTimerEvent * );
};

void Timer::timerEvent( QTimerEvent * ) {
  KSVMsg buf, retbuf;

  if ( buf.recv( msgqid, 3L ) != -1 ) {
    retbuf.setType( 4L );
    retbuf.send( msgqid );

    KWM::activate(kapp->mainWidget()->winId());
  }
}

void myMessageOutput( QtMsgType type, const char *msg )
{
  switch ( type ) {
#if defined(DEBUG) && !(defined(NDEBUG) || defined(NO_DEBUG))
#warning debug build
  case QtDebugMsg:
    fprintf( stderr, "Debug: %s\n", msg );
    break;
  case QtWarningMsg:
    fprintf( stderr, "Warning: %s\n", msg );
    break;
#endif // ifdef DEBUG
  case QtFatalMsg:
    fprintf( stderr, "Fatal: %s\n", msg );
    abort(); // dump core on purpose
  }	
}

int main( int argc, char **argv ) {
  // install own message handler that ignores debug-msg when DEBUG is not defined
  qInstallMsgHandler(myMessageOutput);

  FILE *fp;

  // so that everything is cleaned up
  catchSignals();

  Timer *timer = new Timer;

  KApplication app( argc, argv );

  // create data directory if necessary
  QString rcDir = kapp->localkdedir() + "/share/apps";
  if ( access( rcDir, F_OK ) )
    mkdir( rcDir, 0740 );

  rcDir += "/ksysv";
  if ( access( rcDir, F_OK ) )
    mkdir( rcDir, 0740 );

  pidFile = rcDir + "/ksysv.pid";

  // if there is a pidFile then this is not the first instance of ksysv
  if ( ( fp = fopen( pidFile, "r" ) ) != NULL ) {
    KSVMsg buf;
    int pid;
    QString msg = "";
    buf.setType( 3L );
    buf.setMsg( msg );
    fscanf( fp, "%d %d", &pid, &msgqid );
    // if this fails I assume that the pid file is left over from bad exit
    // and continue on

    if ( buf.send( msgqid ) != -1) {
      // if we don't receive a reply within 3 secs assume previous
      // instance of ksysv died an unnatural death.
      // How should this stuff be handled properly?

      time_t start = time(NULL);
      while ( time(NULL) - start < 3 ) {
	if (buf.recv( msgqid, 4L, IPC_NOWAIT ) != -1) {
	  fclose( fp );
	  exit(0);
	}
      }
      msgctl( msgqid, IPC_RMID, NULL );
    }
    fclose( fp );
  }

  // This is the first instance so create a pid/msgqid file
  key_t key = ftok( getenv( "HOME" ), (char)rand() );
  msgqid = msgget( key, IPC_CREAT | 0600 );

  fp = fopen( pidFile, "w" );
  fprintf( fp, "%ld %d\n", (long)getpid(), msgqid );
  fclose( fp );

  // to signal errors to scripts... well it´s cleaner at least
  int returnValue = 1;

  if (geteuid() == 0)
  {

  // session-management
  if (kapp->isRestored())
	RESTORE(KSVTopLevel)
  else {
    KSVTopLevel* top = new KSVTopLevel("KSysV Toplevel Widget");
    kapp->setMainWidget( top );
    top->setCaption(KSVCore::getAppName());
    top->show();
  }
  // end session-management

  	returnValue = app.exec();
  } else
  {
  	KMsgBox::message( 0, KSVCore::getAppName() + i18n(": Error"),
			  i18n("Only root is allowed to manage runlevels."),
			  KMsgBox::DB_FIRST | KMsgBox::STOP, i18n("Close") );
	returnValue = 1;
  }

  // clean up
  delete timer;
  msgctl( msgqid, IPC_RMID, 0 );
  remove( pidFile );

  return returnValue;
}
