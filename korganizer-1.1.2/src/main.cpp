/* 
   KOrganizer (c) 1997,1998 Preston Brown
   All code is covered by the GNU Public License

   $Id: main.cpp,v 1.43.2.7 1999/04/18 22:22:58 pbrown Exp $	
*/

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <qdir.h>

#include <kapp.h>

#include "topwidget.h"
#include "version.h"

KApplication *korganizer;
CalObject *cal;


// display items that are imminent over the next several days as listed
void display_imminent(int numdays, QString fileName)
{
  QDate currDate(QDate::currentDate());
  KDPEvent *currEvent;
  
  if (fileName.isEmpty()) {
    printf(i18n("No default calendar, and none was specified on the command line.\n"));
    exit(0);
  }
  if (!cal->load(fileName)) {
    printf(i18n("Error reading from default calendar.\n"));
    exit(0);
  }

  for (int i = 1; i <= numdays; i++) {
    QList<KDPEvent> tmpList(cal->getEventsForDate(currDate, TRUE));
    printf("%s\n",currDate.toString().data());
    printf("---------------------------------\n");
    if (tmpList.first())
      for (currEvent = tmpList.first(); currEvent; currEvent = tmpList.next())
	currEvent->print(KDPEvent::ASCII);
    else
      printf("(none)\n");
    printf("\n");
    currDate = currDate.addDays(1);
  }
  exit(0);
}

void print_help()
{
  printf("KOrganizer %s\n\n",korgVersion);
  printf("X11/Qt Options:\n");
  printf("-display <display>\n");
  printf("-geometry <geometry>\n"); 
  printf("-name <name>\n");
  printf("-title <title>\n");
  printf("-cmap\n\n");
  printf("KOrganizer Options:\n");
  printf("-c or --calendar <calendar>\n");
  printf("-l or --list\n");
  printf("-s or --show <numdays>\n");
  printf("-h or --help\n");
  exit(0);
}

void signalHandler(int signo)
{
  switch(signo) {
  case SIGSEGV:
    fprintf(stderr, "KOrganizer Crash Handler (signal %d)\n\n", signo);
    fprintf(stderr, "KOrganizer has crashed.  Congratulations,\n"
	   "You have found a bug! :( Please send e-mail to\n"
	   "pbrown@kde.org with the following details:\n\n"
	   "1. What you were doing when the program crashed.\n"
	   "2. What version of KOrganizer you are running.\n"
	   "3. Any other details you feel are relevant.\n\n"
	   "Remember: your bug reports help make the next\n"
	   "release of KOrganizer more bug-free!\n");
    signal(SIGSEGV, SIG_DFL);
    kill(getpid(), 11);
    break;
  default:
    break;
  }
  return;
}

void make_local_dirs(void)
{
  QString pathName;
  QDir qd;

  // make sure our directory tree exists
  pathName = KApplication::localkdedir().data();
  qd.setPath(pathName.data());
  if (!qd.exists())
    qd.mkdir(pathName.data());
  pathName += "/share";
  qd.setPath(pathName.data());
  if (!qd.exists())
    qd.mkdir(pathName.data());
  pathName = KApplication::localconfigdir().data();
  qd.setPath(pathName.data());
  if (!qd.exists())
    qd.mkdir(pathName.data());
  pathName = KApplication::localkdedir().data();
  pathName += "/apps";
  qd.setPath(pathName.data());
  if (!qd.exists())
    qd.mkdir(pathName.data());
  pathName += "/korganizer";
  qd.setPath(pathName.data());
  if (!qd.exists())
    qd.mkdir(pathName.data(), TRUE);
}

void start_alarm_daemon(const QString &fn)
{
  QString execStr;
  
  if (!fn.isEmpty())
    execStr.sprintf("%s/alarmd %s",
		    KApplication::kde_bindir().data(),
		    fn.data());
  else
    execStr.sprintf("%s/alarmd",
		    KApplication::kde_bindir().data());
  system(execStr.data());
}

int main (int argc, char **argv)
{

  KConfig *config;
  QDir qd;
  QString fn;

  cal = new CalObject; // here so we can display stuff (PGB 07/27/98)

  //  if (signal(SIGSEGV, signalHandler) == SIG_ERR)
  //    debug("warning, can't catch SIGSEGV!");

  // create the local mirror of the KOrganizer directory layout in ~/.kde
  make_local_dirs();

  // the KApplication needs to be created first before program-specific
  // argument processing  to process any Qt/X11/KDE command line arguments.
  korganizer = new KApplication(argc, argv, "korganizer");
 
  config = korganizer->getConfig();
  config->setGroup("General");

  fn = config->readEntry("Current Calendar");

  // process command line arguments
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if ((strcasecmp(argv[i], "-h") == 0) || 
	  (strcasecmp(argv[i], "--help") == 0))
	print_help();
      else if ((strcasecmp(argv[i], "-c") == 0) || 
	       (strcasecmp(argv[i], "--calendar") == 0)) {
	i++;
	if (argv[i] != 0)
	  fn = argv[i];
      } else if ((strcasecmp(argv[i], "-l") == 0) ||
		 (strcasecmp(argv[i], "--list") == 0)) {

	display_imminent(1, fn);
      } else if ((strcasecmp(argv[i], "-s") == 0) ||
		 (strcasecmp(argv[i], "--show") == 0)) {
	i++;
	if (argv[i] != 0)
	  display_imminent(atoi(argv[i]), fn);
	else
	  display_imminent(1, fn);
      } else {
	fn = argv[i];
      }
    }
  }

  
  // always try to start the alarm daemon. IT will take care of itself
  // if there is no calendar to work on yet. we may or may not have a
  // calendar filename to work with.
  start_alarm_daemon(fn);
  
  // check if korganizer is to be restored via session management
  if (korganizer->isRestored()) {
    int n = 1;
    while (TopWidget::canBeRestored(n)) {
      (new TopWidget(cal, fn))->restore(n); 
      n++;
    }
  } else {
    (new TopWidget(cal, fn))->show();
  }

  korganizer->exec();
  
  // cleanup time
  delete korganizer;

  return 0;
}

