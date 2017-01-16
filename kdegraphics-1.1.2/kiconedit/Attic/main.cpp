/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)
 
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

// A lot of this code is lifted from KMail. Thanks, guys!

#include <signal.h>
#include "debug.h"

#ifdef HAVE_CONFIG_H
#include <../config.h>
#endif

#include "kiconedit.h"
#include "main.h"
#include <kapp.h>

WindowList* windowList = NULL;

static void signalHandler(int sigId);
static void cleanup(void);
static void setSignalHandler(void (*handler)(int));

KSWApplication::KSWApplication ( int& argc, char** argv, const QString& rAppName )  
  : KApplication ( argc, argv, rAppName )  
{
  _ksw_save = XInternAtom(qt_xdisplay(),"KSW_SAVE",False);
  _ksw_exit = XInternAtom(qt_xdisplay(),"KSW_EXIT",False);
}

bool KSWApplication :: x11EventFilter( XEvent *ev)
{
  XClientMessageEvent* e = &ev->xclient;

  if (e->message_type == _ksw_save)
  {
    debug("Got KSW_SAVE");
    emit ksw_save();
    return true;
  }
  else if (e->message_type == _ksw_exit)
  {
    debug("Got KSW_EXIT");
    emit ksw_exit();
    return true;
  }

  return false;
}

bool getCommandLine(QStrList *list, int argc, char **argv)
{
  char *filename;
  for(int i = 1; i < argc; i++)
  {
    filename = argv[i];
    if(filename[0] != '-')
    {
      list->append(filename);
    }
  }
  if(list->count() > 0)
    return true;
  else
    return false;
}

//-----------------------------------------------------------------------------
// Crash recovery signal handler
static void signalHandler(int sigId)
{
  QWidget* win;

  if(!windowList)
    exit(1);

  debug("*** KIconEdit got signal %d\n", sigId);

  // try to cleanup all windows
  while (windowList->first() != NULL)
  {
    win = windowList->take();
    if (win->inherits("KIconEdit")) ((KIconEdit*)win)->saveGoingDownStatus();
    delete win;
  }

  setSignalHandler(SIG_DFL);
  cleanup();
  exit(1);
}


//-----------------------------------------------------------------------------
static void setSignalHandler(void (*handler)(int))
{
  signal(SIGSEGV, handler);
  signal(SIGKILL, handler);
  signal(SIGTERM, handler);
  signal(SIGHUP,  handler);
  signal(SIGFPE,  handler);
  signal(SIGABRT, handler);
}

static void cleanup(void)
{
  kapp->getConfig()->sync();
}



int main(int argc, char **argv)
{
  qInstallMsgHandler( myMessageOutput );
  KSWApplication a(argc, argv, "kiconedit");
  KIconEdit *ki = 0L;

  setSignalHandler(signalHandler);

  if (a.isRestored())
  {
    RESTORE(KIconEdit);
  }
  else
  {
    QStrList list;
    if(getCommandLine(&list, argc, argv))
    {
      for(uint i = 0; i < list.count(); i++)
      {
        ki = new KIconEdit(list.at(i), "kiconedit");
        CHECK_PTR(ki);
      }
    }
    else
      ki = new KIconEdit;
      CHECK_PTR(ki);
  }

  return a.exec();
}


