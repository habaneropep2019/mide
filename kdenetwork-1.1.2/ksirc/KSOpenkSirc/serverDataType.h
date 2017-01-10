#ifndef SERVERDATATYPE_H
#define SERVERDATATYPE_H

#include <qlist.h>
#include <qstring.h>

class port
{

public:
  port( const char *portnum ) { p=portnum; }
  const char *portnum() const { return p; }

private:
  QString p;

};


class Server
{

public:
  Server( const char *group, const char *server, QList<port> ports, 
          const char *serverdesc, const char *script ) {
          g=group; s=server; p=ports; sd=serverdesc; sc=script;
          p.setAutoDelete(TRUE);
          }
  const char *group() const      { return g; }
  const char *server() const     { return s; }
  QList<port> ports() const      { return p; }
  const char *serverdesc() const { return sd; }
  const char *script() const     { return sc; }

private:
  QString     g;
  QString     s;
  QList<port> p;
  QString     sd;
  QString     sc;

};

#endif
