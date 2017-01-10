//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This file os part of KRN, a newsreader for the KDE project.              //
// KRN is distributed under the GNU General Public License.                 //
// Read the acompanying file COPYING for more info.                         //
//                                                                          //
// KRN wouldn't be possible without these libraries, whose authors have     //
// made free to use on non-commercial software:                             //
//                                                                          //
// MIME++ by Doug Sauder                                                    //
// Qt     by Troll Tech                                                     //
//                                                                          //
// This file is copyright 1997 by                                           //
// Roberto Alsina <ralsina@unl.edu.ar>                                      //
// Magnus Reftel  <d96reftl@dtek.chalmers.se>                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifndef NNTP_H
#define NNTP_H


#include <stdio.h>


#include "rules.h"
#include <qlist.h>

#include <qstring.h>
#include <qstrlist.h>
#include <qdict.h>
#include <qobject.h>

#include <ksock.h>
#include <ktreeview.h>
#include <kconfig.h>
#include <ksimpleconfig.h>


#include <mimelib/mimepp.h>

#include "kmmessage.h"


//These MUST match the "allkeys" combobox in sortDlg.cpp
#define KEY_SUBJECT 0
#define KEY_SCORE 1
#define KEY_SENDER 2
#define KEY_DATE 3
#define KEY_LINES 4
#define KEY_NONE 5

class NNTP;

#define PART_NONE 0
#define PART_HEAD 1
#define PART_BODY 2
#define PART_ALL (PART_HEAD | PART_BODY)

enum MessageParts {Nopart=PART_NONE,Head=PART_HEAD,Body=PART_BODY,All=PART_ALL};

class Article
{
public:
    Article ();
    Article (const char *ID);
    ~Article ();
    QString Number;
    QString desperate;
    QString Subject;
    QString Lines;
    QString From;
    QString ID;
    QString Date;
//    QStrList Refs;
    void formHeader(QString *s);
    static bool isRead(const char *ID);
    static QStrList *Refs(const char *ID);
    void setRead(bool b=true);
    bool isAvailable() {return isavail;};
    void setAvailable(bool b=true);
    void setMarked(bool b=true) ;
    bool isMarked(){return ismarked;};
    void incref();
    void decref();
    void save();
    bool load();
    int  score();
    void reScore(RuleList rules);
    void lookupAltavista();
//    bool refsLoaded;
    KMMessage *createMessage();
    
    // robert's cache stuff
    bool canExpire();
    void setExpire(bool b);
    void toggleExpire();
    // end robert's cache stuff
    
    //This value is valid only for a few moments after running
    //thread() over an article list. It's not really meant to be used...
    int  threadDepth;
    bool isread;
private:
    int lastScore;
    bool isavail;
    bool ismarked;
    int refcount;
    bool expire;  // robert's cache stuff
};

typedef QListT<Article> ArticleListBase;

class ArticleList: public ArticleListBase
{
public:
    ArticleList();
    ~ArticleList();
    void thread(bool threaded,int key1,int key2,int key3,int key4);
};

typedef QDictT<Article>ArticleDictBase;

class ArticleDict: public ArticleDictBase
{
public:
    ArticleDict::ArticleDict ();
};

typedef QListT<ArticleList> ThreadListBase;

class ThreadList: public ThreadListBase
{
public:
    int key1;
    int key2;
    int key3;
    int key4;
};

class Artdlg;

class NewsGroup
{
public:
    NewsGroup(const char *name);
    ~NewsGroup();
    void save();
    void load();

    int lastArticle(NNTP *server);
    void saveLastArticle(NNTP *server,int i);
    
    //A pointer to the article window, or 0.
    //used to be boolean, that's why the name is weird.
    Artdlg *isVisible;
    //List of articles in the newsgroup.
    ArticleList artList;
    void getList(Artdlg *dialog=0);
    void updateList();
    void getSubjects(NNTP *server);
    void getMessages(NNTP *server);
    void catchup();
    void addArticle(QString ID,bool onlyUnread=false);
    int countNew(NNTP *server);
    void clean();
    char *name;
    bool dirty;
private:
    KSimpleConfig *sconf;
};

typedef QListT<NewsGroup> GroupListBase;

class GroupList: public GroupListBase
{
public:
    GroupList();
    ~GroupList();
protected:
    virtual int compareItems(GCI item1,GCI item2);
};





class NNTP: public QObject, public DwNntpClient
{
    
    Q_OBJECT
        
public:
    NNTP(char *hostname=0);
    ~NNTP();
    
    bool    connect();
    bool    disconnect();
    bool    isConnected() {return Connected;};
    bool    isReadOnly() {return Readonly;};
    void    groupList(bool fromserver);
    bool    setGroup( const char *groupname);
    char    *group() {return GroupName.data();};
    bool    artList(int from=0,int to=0,NewsGroup *n=0);
    bool    getMissingParts(MessageParts parts,const char *id);
    QString *article(const char *id);
    QString *head(const char *id);
    QString *body(const char *id);
    MessageParts isCached(const char *id);
    int     authinfo(const char *username,const char *password);
    int     setMode (char *mode);
    QString    	hostname;
    int     first;
    int     last;
    int     howmany;
    char    *lastStatusResponse() { return Laststatus.data();};
    bool    reConnect();
    void    resetCounters( bool byte=true,bool command=true);
    void    reportCounters (bool byte=true,bool command=true);
    long     byteCounter;
    long     oldbytes;
    int     commandCounter;
signals:
    void newStatus(const char *status);
    void lostServer();

public slots:
    void refresh();
    
private:
    bool checkDisconnection();
    bool    reportBytes;
    bool    reportCommands;
    int         listOverview();
    int         listXover(int from=0,int to=0,NewsGroup *n=0);
    
    QString 	Laststatus;
    QString 	GroupName;
    bool    	Connected;
    bool       	Readonly;
    bool       	connected;
    void       	overviewFmt( QString of );
    int		OffsetArticleNumber;
    int		OffsetSubject;
    int		OffsetFrom;
    int		OffsetLines;
    int         OffsetID;
    int		OffsetDate;
    int         OffsetRef;
    FILE      	*server;
    KSocket    	*sock;
    DwString     partialResponse;
    void        PGetTextResponse();
    QString saneID(const char *id);
};

#endif
