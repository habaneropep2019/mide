/*
 *
 * kscd -- A simple CD player for the KDE project           
 *
 * $Id$
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben 
 * wuebben@math.cornell.edu
 *
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef _CDDB_H_
#define _CDDB_H_

#include <qtimer.h>
#include <qstring.h>
#include <qstrlist.h>

#include <ksock.h>

#define DEFAULT_SUBMIT_EMAIL "xmcd-cddb@amb.org"
#define DEFAULT_TEST_EMAIL "cddb-test@xmcd.com"
#define DEFAULT_CDDB_SERVER "www.cddb.com cddbp 8880 -"
#define DEFAULT_CDDBHTTP_SERVER "cddb.moonsoft.com http 80 /~cddb/cddb.cgi"

#define CDDB_READ_BUFFER_LEN  2024
#define CDDB_FIELD_BUFFER_LEN 256

class CDDB:public QObject
{

    Q_OBJECT

	public:

    CDDB(char *hostname=0, int port = 0, int timeout = 60);
    ~CDDB();

    typedef enum { INIT=0, ERROR_INIT=1,
                   HELLO=2, ERROR_HELLO=3,
                   READY=4,
                   QUERY=5, ERROR_QUERY=6,
                   CDDB_READ=7,CDDB_READING=70,ERROR_CDDB_READ=8, 
                   CDDB_DONE=9, CDDB_TIMEDOUT=10,
                   INEX_READ=11,
                   REQUEST_SERVER_LIST=12, SERVER_LIST_WAIT=13, GETTING_SERVER_LIST=14,
                   HTTP_REQUEST=15, HTTP_HEADER=16,
		   PROTO=17, DO_NOTHING=127
    } CDDBState;
    
    typedef enum { REGULAR=100, SERVER_LIST_GET=101 } CDDBMode;

    typedef enum {CDDBP,CDDBHTTP,SMTP,UNKNOWN} transport;
    static transport decodeTransport(const char *);

    bool  isConnected() {return connected;};

    void getData(
	QString& data,
	QStrList& titlelist,
	QStrList& extlist, 
	QString& cat, 
	QStrList& discidlist,
	int& revision,
	QStrList& playlist
    );

    void  get_inexact_list(QStrList& inexact_list);
    void  query_exact(QString line);
    void  setPathList(QStrList& paths);
    bool  checkDir(unsigned long magicID, char* dir);
    bool  getValue(QString& key,QString& value, QString& data);
    void  serverList(QStrList& list);
    void  cddbgetServerList(QString& server);
    void  close_connection();

    static void  sighandler(int sig);
    static void  setalarm();
    static void  cddb_http_xlat(QString &s);

    void        queryCD(unsigned long magicID,QStrList& querylist);

    bool local_query(
	unsigned long magicID,
	QString&  data,
	QStrList& titlelist,
	QStrList& extlist,
	QString&  category,
	QStrList& discidlist,
	int& revision,
	QStrList& playlist
    );
    void getCategoryFromPathName(char* pathname, QString& string);

    static bool normalize_server_list_entry(QString &entry);

    void setHTTPProxy(QString host, int port);
    void useHTTPProxy(bool flag);

    bool    useHTTPProxy();
    QString getHTTPProxyHost();
    int     getHTTPProxyPort();
   
 protected:
    
    void 	do_state_machine();
    void 	parse_serverlist_entry();
    void        send_http_command(QString &command);
    bool        next_token();
    
    transport protocol;
        
 public slots:

    void	 cddb_connect(QString& server);
    void	 cddb_connect_internal();
    void         cddb_read(KSocket* sock);
    void         cddb_close(KSocket* sock);
    void         cddb_timed_out_slot();

 signals:

    void        cddb_done();
    void	cddb_timed_out();
    void 	cddb_failed();
    void 	cddb_ready();
    void        cddb_inexact_read();
    void 	cddb_no_info();
    void	get_server_list_done();
    void        get_server_list_failed();

 private:

    QString     domainname;
    QString     username;

    QStrList    inexact_list;
    QTimer 	starttimer;
    QTimer 	timeouttimer;
    QTimer      expecttimer;

    QStrList    pathlist;
    QString 	hostname;

    QString     proxyhost;
    int         proxyport;
    bool        use_http_proxy;
    QString     cgi;
   
    QString     respbuffer;
    QString     tempbuffer;
    QString     lastline;
    int		timeout;
    int     	port;
    bool    	connected;
    bool	readonly;
    KSocket	*sock;

    CDDBState	state;
    CDDBMode    mode;
    CDDBState   saved_state; // I was using stack here, but I guess it's overhead

    struct passwd* pw;     
    QString     category;
    QString	title;
    QStrList 	serverlist;
    unsigned long magicID;
    int         protocol_level;

};

#endif


