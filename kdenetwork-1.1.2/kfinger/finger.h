#ifndef __PROTOCOL
#define __PROTOCOL
#include <qstring.h>
#include <qsocketnotifier.h>
#include <qobject.h>
#include <ksock.h>
class FingerProtocol : public QObject {
Q_OBJECT
	public:
                FingerProtocol(char * buff);
		~FingerProtocol();
                int  writeString(QString st);
		bool stopFlag;
		bool connection(  const char * host,int port); 

	protected:
                QSocketNotifier *sockNotif;
		char *s;
		int  sock;          
	signals:
	  void update();
          void finish();	
        public slots:
          void readBuff( int);
      };

#endif

