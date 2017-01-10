#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <qdir.h>
#include <kapp.h>

#include "alarmdaemon.h"

AlarmDaemon *ad;
QDir qd;

static void sighandler(int signo)
{
  switch(signo) {
  case SIGINT:
  case SIGTERM:
    qd.remove("alarmd.pid");
    exit(0);
    break;
  case SIGHUP:
    ad->reloadCal();
    if (signal(SIGHUP, sighandler) == SIG_ERR)
      debug("warning, can't reset signal handler\n");
    break;
  case SIGUSR1: // ignore, used to check lockfile status.
    if (signal(SIGUSR1, sighandler) == SIG_ERR)
      debug("warning, can't reset signal handler");
    break;
  }
  return;
}

int initDaemon()
{
  pid_t pid;
  QString s;
  QString pidFileName;
  QFile pidFile;

  // we probably have a nice little race condition here that I am not
  // going to worry about right now.  
  s.sprintf("%s/share/apps/korganizer",KApplication::localkdedir().data());
  qd.setPath(s.data());

  pidFileName.sprintf("%s/alarmd.pid",s.data());
  pidFile.setName(pidFileName.data());

  // a lock file already exists, don't start up.
  if (pidFile.exists()) {
    if(pidFile.open(IO_ReadOnly)) {
      pid_t pid;
      char pidStr[25];
      pidFile.readLine(pidStr, 24);
      pidFile.close();
      pid = atoi(pidStr);
      if (kill(pid, SIGUSR1) < 0) {
	// stale lockfile
	unlink(pidFileName.data()); // get rid of old lockfile
      }
    }
  }

  // if the PID file still exists, a daemon really is running.
  // quit silently.
  if (pidFile.exists()) {
    exit(0);
  } else {
    // make ourselves a daemon
    if ((pid = fork()) < 0)
      return(-1); // we had an error forking
    else if (pid != 0) {
      // make a lock file.
      pidFile.open(IO_ReadWrite);
      s.sprintf("%d",pid);
      pidFile.writeBlock(s.data(), s.length());
      pidFile.close();
      exit(0); // parent dies silently
    }
    
    // child continues
    setsid();
    
    if (getenv("$HOME") != NULL)
      chdir(getenv("$HOME"));
    umask(0);
  }
  return(0);
}

int main(int argc, char **argv)
{
  // question -- should we log errors w/syslog?  Since we
  // are a daemon, the printf's aren't really going anywhere...

  if (signal(SIGINT, sighandler) == SIG_ERR)
    debug("warning, can't catch SIGINT");
  if (signal(SIGTERM, sighandler) == SIG_ERR)
    debug("warning, can't catch SIGTERM");
  if (signal(SIGHUP, sighandler) == SIG_ERR)
    debug("warning, can't catch SIGHUP");
  if (signal(SIGUSR1, sighandler) == SIG_ERR)
    debug("warning, can't catch SIGUSR1");

  // Only start up if we aren't running already.
  // initDaemon will exit if another is already running
  initDaemon();

  KApplication app(argc, argv, "alarmd");

  // if no filename is supplied, read from config file.
  if (argc < 2) {
    QString cfgPath = KApplication::localconfigdir();
    cfgPath += "/korganizerrc";
    KConfig *config = new KConfig(cfgPath.data());
    
    config->setGroup("General");
    QString newFileName = config->readEntry("Current Calendar");
    delete config;
    ad = new AlarmDaemon(newFileName.data());
  } else
    ad = new AlarmDaemon(argv[1]);

  ad->dock();

  app.enableSessionManagement(TRUE);
  app.setTopWidget(new QWidget);

  app.exec();
  delete ad;

  return 0;
}
