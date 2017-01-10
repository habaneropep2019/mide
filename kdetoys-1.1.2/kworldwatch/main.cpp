#include <qapp.h>
#include <qpushbt.h>
#include <kapp.h>
#include <qimage.h>
#include <klocale.h>

#include "toplevel.h"


int main( int argc, char **argv )
{
  KApplication app(argc, argv, "kworldwatch");
  TopLevel *toplevel;

  if (kapp->isRestored())
    RESTORE(TopLevel)
  else
    {    
      toplevel = new TopLevel();
      toplevel->setCaption(klocale->translate("KDE World Wide Watch"));

      app.setMainWidget(toplevel);
    }
	
  return app.exec();
}
