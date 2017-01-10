
#include "debug.h"
#include <kmsgbox.h>
#include <kapp.h>

void myMessageOutput( QtMsgType type, const char *msg )    
{
  switch ( type )
  {
     case QtDebugMsg:
#ifdef DEBUG
       fprintf( stderr, "Debug: %s\n", msg );
#endif
       break;
     case QtWarningMsg:
       KMsgBox::message(0, i18n("Warning"), msg );
       fprintf( stderr, "Warning: %s\n", msg );
       break;
     case QtFatalMsg:
       KMsgBox::message(0, i18n("Fatal!"), msg );
       fprintf( stderr, "Fatal: %s\n", msg );
       abort();
       // dump core on purpose
  }
}


