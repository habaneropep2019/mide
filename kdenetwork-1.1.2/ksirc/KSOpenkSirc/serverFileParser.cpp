#include "serverFileParser.h"
#include "serverDataType.h"
#include <qfile.h>
#include <qtextstream.h>
#include <stdlib.h>

extern QList<Server> Groups;

//  Opens, reads and parses server information from a server file, 
//  sets the information in the global variable and returns 1 if
//  sucessful, takes a filename as an argument.

int serverFileParser::readDatafile( const char *fileName ) 
{
  Groups.setAutoDelete( TRUE );
  Groups.clear();
  QFile serverFile( fileName );
  if ( !serverFile.open( IO_ReadOnly ) )
    return -1;

  QTextStream fileStream(&serverFile);

  // the file is layed out as follows:
  //  service:servername:serveraddress:ports:script:
  // so we parse it this way

  while( !fileStream.eof() ) {
    QString str = fileStream.readLine();
    int pos = 0;
    QString group;
    QString servername;
    QString serveraddress;
    QList<port> ports;
    QString script;

    QString buf;
    QString portbuff;
    
    for( uint loc = 0; loc <= str.length(); loc++ ) {
      if ( str[loc] == ':' || loc == str.length()) {
        switch(pos) {
          case 0: // service
            group = buf.copy();
            break;
          case 1: // server name
            servername = buf.copy();
            break;
          case 2: // server address
            serveraddress = buf.copy();
            break;
          case 3: // port listing
            for ( uint portloc = 0; portloc <= buf.length(); portloc++ ) {
              if (buf[portloc] == ',' || portloc == buf.length()) {
                if (!portbuff.isEmpty())
                  ports.inSort( new("port") port(portbuff));
                portbuff.truncate( 0 );
              } else {
                portbuff += buf[portloc];
              }
            }
            break;
          default: // script
            script = buf.copy();
        }
        pos++;
        buf.truncate( 0 );
        portbuff.truncate( 0 );
      } else {
        buf += str[loc];
      }
    } // for loop

    Groups.inSort( new("Server") Server(group, serveraddress, ports, 
                              servername, script) );
 
  } // while loop

  serverFile.close();
  return 1;

}

// Writes the data stored in the global variable to the server file,
// returns 1 if sucessful, takes a filename as an argument, uhh NOT.
// we want to write it ~/.kde/share/apps/ksirc/servers.txt

int writeDataFile() 
{
  return 1;
}
