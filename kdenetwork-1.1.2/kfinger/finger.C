/*
    KFinger - ver 0.8.2

    KDE project - kdenetwork
    
    finger.C : Internet Finger protocol
    
    (c) Andrea Rizzi <rizzi@kde.org>
	27 May 1999
    
    License: GPL

*/
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <kapp.h>
#include "finger.h"
#include <unistd.h>
#include <errno.h>

FingerProtocol::FingerProtocol(/*const char * host,int port,*/char *buff) 
{
stopFlag=true ;
s=buff; 
}


FingerProtocol::~FingerProtocol() 
{
    close( sock); 
}


bool FingerProtocol::connection(  const char * host,int port) 
{
  sock=::socket(PF_INET,SOCK_STREAM,0);
  if (sock < 0) return false;        
  
  struct sockaddr_in server;
  struct hostent *hostinfo;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  hostinfo = gethostbyname(host);
  if ( !hostinfo )
    {
	  perror("Unknown host ");
	  ::close( sock );
	  sock = -1;
	  return false;	    
    }
  server.sin_addr = *(struct in_addr*) hostinfo->h_addr;      
  fcntl(sock,F_SETFL,(fcntl(sock,F_GETFL)|O_NDELAY));

  errno = 0;
  if (::connect(sock, (struct sockaddr*)(&server), sizeof(server))){
      if(errno != EINPROGRESS && errno != EWOULDBLOCK){
          ::close( sock );
          sock = -1;
          return false;   
      }
      }  else 
      {
       sockNotif = new QSocketNotifier(sock,QSocketNotifier::Read,this);
      QObject::connect(sockNotif, SIGNAL(activated(int )),this,SLOT(readBuff(int ))); 
       sockNotif->setEnabled(TRUE);
      return true;
      }
//warning("Connecting...");  
  fd_set rd, wr;
  struct timeval timeout;
  int retval=0, n;

  n = 30 ; //60 seconds of TimeOut 
  FD_ZERO(&rd);
  FD_ZERO(&wr);
  FD_SET(sock, &rd);
  FD_SET(sock, &wr);
      struct rlimit rlp;

  while(n-- && stopFlag){
      timeout.tv_usec = 0;      
      timeout.tv_sec = 2;
      getrlimit(RLIMIT_NOFILE, &rlp); 
      retval=select(rlp.rlim_cur, (fd_set *)&rd, (fd_set *)&wr, (fd_set *)0,
                   (struct timeval *)&timeout);
//      warning("retval %i, val %i",retval,val);
      if(retval){
//	warning("connected");    
           sockNotif = new QSocketNotifier(sock,QSocketNotifier::Read,this);
           QObject::connect(sockNotif, SIGNAL(activated(int )),this,SLOT(readBuff(int ))); 
           sockNotif->setEnabled(TRUE);
           return true ;
          }
      qApp->processEvents();
      qApp->flushX();
  }
if(n==0)
  perror("Connection timeout");
  ::close( sock );
  sock= -1;
  return false;       
}



void FingerProtocol::readBuff( int     ) 
{
  warning("read buff" );
  sockNotif->setEnabled(FALSE);
  char c,n;
  int mn = recv( sock,   s    , 1, MSG_PEEK );
  if ( mn <= 0 ) {
	stopFlag=false;  
        close( sock); 
        emit finish();
	}
  else
	{
	do  {
	     n=read(sock,&c,1);
	     if (c!='\r') 
	         *s++=c;
	    } while (n==1);       
	 *s=0;    
         update();
         sockNotif->setEnabled(TRUE);
         }
 
}

int FingerProtocol::writeString(QString st) 
{
    unsigned int len;

    char    val=0; 
    getsockopt(sock,SOL_SOCKET,SO_ERROR,(void*)&val,&len );                   
    //warning("error %d ",val);
    if(val==0)  
	write(sock,st,strlen(st));
    return val;  
}
#include "finger.moc"

