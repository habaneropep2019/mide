#include <kdebug.h>
#include <stdio.h>
#include <kapp.h>
#include <klocale.h>
#include "kpaint.h"
#include "version.h"
#include "app.h"

MyApp::MyApp( int &argc, char **argv, const QString appname)
  : KApplication( argc, argv, appname)
{
  KPaint *kp;

  clipboard_= 0;

  if (isRestored()) {
#if 0
    for (int i= 1; KTMainWindow::canBeRestored(i); i++) {
      kp= new KPaint();
      kp->restore(i);
    }
#else
    // Use new RESTORE macro
    RESTORE(KPaint);
#endif
  }
  else {
    if (argc == 1) {
      kp= new KPaint();
    }
    else if (argc == 2 &&
	     strcmp(argv[1],"-h")!=0 &&
	     strcmp(argv[1],"--help")!=0 ) {
      kp= new KPaint((const char *) (argv[1]));
    }
    else {
      usage();
      ::exit(0);
    }
    kp->show();
  }
}

void MyApp::usage()
{
  /* FIXME !!!!!!
   * No i18n available when invoked "kpaint -h" but "kpaint -h -e"
   */

  //  printf("kpaint " APPVERSTR " " APPAUTHOREMAIL);
  printf("kpaint (c) %s",APPAUTHOR);
  printf("%s\n", /*klocale->translate*/("Released under GPL see LICENSE for details"));
  printf("%s %s %s\n", /*klocale->translate*/("Usage:"),
	 APPNAME, /*klocale->translate*/("[url | filename]"));
  /*
  QString aParameter(' ');
  aParameter += APPNAME;
  aParameter += ' ';
  aParameter += klocale->translate("[url | filename]");
  aParameter += '\n';
  printf(aParameter.data());
  */
}

#include "app.moc"
