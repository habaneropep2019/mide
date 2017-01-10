#ifndef KTETRIS_DI_H
#define KTETRIS_DI_H

#include "defines.h"
#include "net.h"

#include <qlabel.h>
#include <qdialog.h>
#include <qlined.h>
#include <qtimer.h>
#include <qpushbt.h>

#include <kconfig.h>

/* high scores struture */
typedef struct
{
	int score;
	char name[10];
} hscore;

/* highscores dialog */
class WHighScores : public QDialog
{
 Q_OBJECT
		
 public:
	WHighScores(bool show, int score, QWidget *parent=0, const char *name=0);

 private slots:
	void writeName();
	
 private:
	KConfig *kconf;
	int range;
	QLineEdit *qle;
	QPushButton *pb;
	QLabel *lb;
	
	void showHS(bool show, int score);
};

/* options dialog */
class Options : public QDialog
{
 Q_OBJECT
		
 public:
	Options( QWidget *parent=0, const char *name=0 );
};

/* net defines */
#define ASK_MODE     0
#define ASK_ADD      1
#define RE_ASK_ADD   2
#define CONNECT      3
#define WAIT_PLAYERS 4

#define NET_TIMEOUT 500

/* network game dialog */
class NetDialog : public QDialog
{
 Q_OBJECT
		
 public:
	NetDialog( QWidget *parent=0, const char *name=0 );
	~NetDialog();
	
	NetObject *netObject() const { return net_obj; };
	
 private slots:
	void server() { createNetObject(TRUE); }
	void client() { createNetObject(FALSE); }
	void connecting();
	void timeout();
	void cancel();
	void play();
	
 private:
	int state;
	
	NetObject *net_obj;
	QTimer *timer;
	
	QString serror;
	int oldNbPlayers;
	
	QLabel *labTitle, *labName, *labAdd, *labPort, *labState, 
		   *labNameE, *labAddE, *labPortE;
	QPushButton *butServer, *butClient, *butConnect, *butCancel, *butPlay;
	QLineEdit *edAdd, *edPort, *edName;
	
	void showConnect(int state);
	void createNetObject(bool server);
};


#endif 
