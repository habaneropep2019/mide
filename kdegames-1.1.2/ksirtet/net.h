#ifndef KTETRIS_NE_H
#define KTETRIS_NE_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include "defines.h"

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>

#include <qstring.h>
#include <qlabel.h>
#include <qsocknot.h>

#define CLIENT_MODE 1
#define SERVER_MODE 2

#define END_MSG   "end"
#define PLAY_MSG  "play"
#define READY_MSG "ready"

#define NAME_LENGTH 10

#define CLIENT_TIMEOUT 400
#define SERVER_TIMEOUT 500

typedef struct {
	char prev[NAME_LENGTH];
	char next[NAME_LENGTH];
} NamesMessage; 

typedef struct {
	int height, lines, end;
} _ClientMessage;

typedef struct {
	int height_prev, height_next, lines;
	char name_prev[NAME_LENGTH], name_next[NAME_LENGTH];
	bool win;
} ServerMessage;


class NetObject;
class ClientNetObject;
class ServerNetObject;

class Player
{
 friend NetObject;
 friend ClientNetObject;
 friend ServerNetObject;
	
 protected:
	Player( NetObject *parent );
	~Player();
	
	NetObject *parent;
	
	int sock;
	QString address, name;
	QLabel *label;
	int height, lines;
};

class NetObject
{
 friend Player;
	
 public:
	NetObject( const char *address = 0,
			  const char *name = 0, const char *port = 0);
	virtual ~NetObject() { clean(); }
	
	bool checkParam( const char *name, const char *address,
					const char *nport, QString& serror );
	bool createSocket( QString& serror );
	bool connectSocketExists() { return pl[0]->sock!=-1; }
	bool samePort()            { return port==t_port.toInt(0); }
	void setPort()             { port = t_port.toInt(0); }
	virtual bool connectSocket( QString& serror ) = 0;
	virtual bool dialogTimeout( QString& serror ) = 0;
	virtual bool initGame( QString& serror) = 0;
	virtual bool playTimeout( QString& serror ) = 0;
	virtual void endClientConnection() { clean(); }
	
	void deleteLabel(int index);
	void createLabel(int index, QLabel *label)
		{ pl[index]->label = label; }

	void ownStatus(int height, int lines);
	void gameOver( QString &serror );
	
	void init();
	void clean();

	bool isClient()    { return mode==CLIENT_MODE; }
	bool isServer()    { return mode==SERVER_MODE; }
	int getTimeout()   { return timeout; }
	int getNbPlayers() { return nb_pl; }
    void getPlayerInfos(QString& infos, int index);
	
	QString getPrevName() { return pl[nb_pl]->name; }
	QString getNextName() { return pl[1]->name; }
	int getPrevHeight()   { return pl[nb_pl]->height; }
	int getNextHeight()   { return pl[1]->height; }

	int getOpponentGift() { return pl[nb_pl]->lines; }
	
	QString t_address, t_name, t_port, client_name;
	
 protected:
	void readSelect(int time=0);
	void writeSelect(int time=0);
	bool readIsset(int sock) { return FD_ISSET(sock, &read_tmp); }
	bool writeIsset(int sock) { return FD_ISSET(sock, &write_tmp); }
	
	void initSelect(int time=0);
	bool checkData( const char *serror1, const char *serror2, QString& serror);
	bool hasReceivedMsg(const char *msg);
	
	int port, timeout, mode;
	
	/* tmp variables */
	long netaddr;
	struct sockaddr_in sin;
	int nb_b, new_sock;
	ksize_t addrlen;
	char buff[MAX_BUFF], name[NAME_LENGTH];
	QString addr;
	Player **new_pl;
	struct hostent *host;
	NamesMessage names_msg;
	bool firstTime;
	_ClientMessage c_msg;
	ServerMessage s_msg;
	
	int max_fd;
	fd_set read_set, write_set, read_tmp, write_tmp;
	struct timeval tv;
	
	/* number of players minus 1 */
	int nb_pl;
    Player **pl;
};

class ClientNetObject : public NetObject
{
 public:
	ClientNetObject( const char *address = 0,
					const char *name = 0, const char *port = 0 );
	bool connectSocket( QString& serror );
	bool dialogTimeout( QString& serror );
	bool initGame( QString& serror );
	bool playTimeout( QString& serror );
};

class ServerNetObject : public NetObject
{
 public:
	ServerNetObject( const char *address = 0,
					const char *name = 0, const char *port = 0 );
	bool connectSocket( QString& serror );
	bool dialogTimeout( QString& serror );
	bool initGame( QString& serror );
	bool playTimeout( QString& serror );
	void endClientConnection();
	
 private:
	bool isConnected;
};


#endif
