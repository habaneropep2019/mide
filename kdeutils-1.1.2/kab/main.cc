/* The main-function for the addressbook application 
 *
 * the KDE addressbook
 * copyright:  (C) Mirko Sucker, 1998, 1999
 * license:    GNU Public License, Version 2
 * mail to:    Mirko Sucker <mirko.sucker@unibw-hamburg.de>
 * requires:   C++-compiler, STL, string class, Qt > 1.33,
 *             NANA (for debugging)
 * $Revision: 1.10.2.1 $
 */

#include <qimage.h>
#include "toplevelwidget.h"
#include <kapp.h>
#include <kimgio.h>
#include "debug.h"

extern "C" {
#include <signal.h>
	   }

void exit_handler()
{
  register bool GUARD; GUARD=true;
  LG(GUARD, "addressbook exit_handler: called.\n");
  // ########################################################
  LG(GUARD, "addressbook exit_handler: clearing ConfigDB "
     "lockfiles.\n");
  ConfigDB::CleanLockFiles(0);
  LG(GUARD, "addressbook exit_handler: done.\n");
  // ########################################################  
}

void signal_handler(int sig)
{
  register bool GUARD; GUARD=true;
  LG(GUARD, "addressbook signal_handler: called.\n");
  // ########################################################  
  exit(sig);
  // ########################################################  
}
  
int main(int argc, char** argv)
{
  register bool GUARD; GUARD=true;
  LG(GUARD, "addressbook main: starting.\n");
  // ########################################################  
  KApplication *app=new KApplication(argc, argv, "kab");
  int rc;
  // -----
  AuthorEmailAddress="mirko@kde.org"; // static, public
  kimgioRegister();
  LG(GUARD, "addressbook main: installing signal "
     "handler.\n");
  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);
  signal(SIGTERM, signal_handler);
  LG(GUARD, "addressbook main: installing exit handler.\n");
  atexit(exit_handler);
  LG(GUARD, "addressbook main: creating addressbook.\n");
  ABTLWidget* db=new ABTLWidget;
  app->setMainWidget(db);
  db->show();
  LG(GUARD, "addressbook main: executing X application.\n");
  rc=app->exec();
  // delete db;
  // -----
  return rc;
  // ########################################################
}


