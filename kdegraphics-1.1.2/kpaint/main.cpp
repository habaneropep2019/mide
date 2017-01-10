#include <kdebug.h>
#include <qstring.h>
#include "version.h"
#include "app.h"
#include "formats.h"
#include <stdlib.h>
#include <string.h>

MyApp *kpaintApp;
FormatManager *formatMngr;
int openwins= 0;

int main( int argc, char **argv )
{
  if ((argc == 2) && (strcmp(argv[1], "-h") == 0)) {
    MyApp::usage();
    exit(0);
  }

  formatMngr= new FormatManager();
  kpaintApp= new MyApp ( argc, argv, APPNAME);

  QObject::connect(kpaintApp, SIGNAL(lastWindowClosed()),
		   kpaintApp, SLOT(quit()));

  return kpaintApp->exec();
}

