/*************************************************************************

 Main KSirc start 

 $$Id: ksirc.cpp,v 1.33.2.1 1999/02/11 15:11:57 kulow Exp $$

 Main start file that defines 3 global vars, etc

 Does nothing to special, start create new("KApplcaiton") KApplcaiton (incorrectly,
 btw, should use the code Kalle posted so it can be session managment
 restarted, etc) loads colours then fires off the main widget.  On
 exit it calls a sync so the kConfig get written.  (Is this a bug?)

*************************************************************************/

/*
 * Needed items
 * 4. Send a /quit and/or kill dsirc on exit
 * */


#include "servercontroller.h"
#include "welcomeWin.h"

#include <iostream.h>
#include <time.h>
#include <unistd.h>

#include <qfont.h>
#include <qmessagebox.h> 

#include <kapp.h>
#include <kconfig.h>

#include "config.h"
#include "version.h"
#include "cdate.h"

KApplication *kApp;
KConfig *kConfig;
global_config *kSircConfig;
//QDict<KSircTopLevel> TopList;
//QDict<KSircMessageReceiver> TopList;

int main( int argc, char ** argv )
{
  // Start the KDE application
  kApp = new("KApplication") KApplication( argc, argv, QString("ksirc") );

  kConfig = kApp->getConfig();

  QString ver = KSIRC_VERSION;
  if(ver.contains(".") == 0){
    // This is a development version
    // 4 week timeout, warn after 2
    int ntime = time(NULL);
    if(ntime - COMPILE_DATE > 4838400){
      // To Old
      QMessageBox::critical(0, "kSirc",
			    QString(i18n("kSirc Alpha releases have an 8 week\n"
                                         "expiry date.\n\n"
                                         "kSirc-ALPHA HAS EXPIRED!!\n"
                                         "Please upgrade or use a beta release")));
      exit(1);
    }
    else if(ntime - COMPILE_DATE > 2419200){
      QMessageBox::warning(0, "kSirc",
			   QString(i18n("kSirc Alpha release have an 8 week\n"
                                        "expiry date.\n\n"
                                        "THIS VERSION WILL EXPIRE IN UNDER 4 WEEKS")));
    }
  }


  // Get config, and setup internal structure.

  kSircConfig = new("global_config") global_config;

  kConfig->setGroup("Colours");
  kSircConfig->colour_text = new("QColor") QColor(kConfig->readColorEntry("text", &black));
  kSircConfig->colour_info = new("QColor") QColor(kConfig->readColorEntry("info", &blue));
  kSircConfig->colour_chan = new("QColor") QColor(kConfig->readColorEntry("chan", &green));
  kSircConfig->colour_error = new("QColor") QColor(kConfig->readColorEntry("error", &red));
  if(kSircConfig->colour_text == 0x0)
      kSircConfig->colour_text = new("QColor") QColor("black");
  if(kSircConfig->colour_info == 0x0)
      kSircConfig->colour_info = new("QColor") QColor("blue");
  if(kSircConfig->colour_chan == 0x0)
      kSircConfig->colour_chan = new("QColor") QColor("green");
  if(kSircConfig->colour_error == 0x0)
      kSircConfig->colour_error = new("QColor") QColor("red");
  

  kSircConfig->colour_background = 0;
  kSircConfig->filterKColour = kConfig->readNumEntry("kcolour", false);
  kSircConfig->filterMColour = kConfig->readNumEntry("mcolour", false);
  kSircConfig->nickFHighlight = kConfig->readNumEntry("nickfcolour", -1);
  kSircConfig->nickBHighlight = kConfig->readNumEntry("nickbcolour", -1);
  kSircConfig->usHighlight = kConfig->readNumEntry("uscolour", -1);
  kSircConfig->transparent = kConfig->readNumEntry("Transparent", false);

  QString ld_path = getenv("LD_LIBRARY_PATH");
  ld_path += ":" + kapp->kde_datadir() + "/ksirc/:";
  ld_path.prepend("LD_LIBRARY_PATH=");
  putenv(ld_path.data());

  kConfig->setGroup("GlobalOptions");
  kSircConfig->defaultfont = kConfig->readFontEntry("MainFont", new("QFont") QFont("fixed"));
  kConfig->setGroup("General");
  kSircConfig->DisplayMode = kConfig->readNumEntry("DisplayMode", 0);
  kSircConfig->WindowLength = kConfig->readNumEntry("WindowLength", 200);
  kSircConfig->transparent = kConfig->readNumEntry("transparent", 0);
  kSircConfig->BackgroundPix = kConfig->readNumEntry("BackgroundPix", 0);
  kSircConfig->BackgroundFile = kConfig->readEntry("BackgroundFile", "");

  kConfig->setGroup("ReleaseNotes");
  if(kConfig->readNumEntry("LastRunRelease", 0) < COMPILE_DATE){
    welcomeWin ww;
    if(ww.exec())
      kConfig->writeEntry("LastRunRelease", COMPILE_DATE);
  }

  if(kApp->isRestored()){
    int n = 1;
    servercontroller *sc = new("servercontroller") servercontroller(0, "servercontroller");
    CHECK_PTR(sc);
    kApp->setMainWidget(sc);
    while (servercontroller::canBeRestored(n)) {
      sc->restore(n);
      n++;
    }
  }
  else{
    servercontroller *control = new("servercontroller") servercontroller(0, "servercontroller");
    control->show();
    kApp->setMainWidget(control);
  }
  
  try {
      kApp->exec();
  }
  catch(...){
      cerr << "Caught Unkown Exception, uhoh!!!\n";
      cerr << "Dying!!!\n";
      exit(10);
  }

  kConfig->sync();
}















