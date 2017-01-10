#ifndef serverFileParser_included
#define serverFileParser_included

#include <qobject.h>

class serverFileParser 
{
  public:
    static int readDatafile( const char *fileName );
    static int writeDataFile();
};

#endif
