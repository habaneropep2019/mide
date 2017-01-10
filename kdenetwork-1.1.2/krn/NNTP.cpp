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

#include <stdlib.h>
#include <unistd.h>

#include "NNTP.h"

#include <kmsgbox.h>
#include <kconfig.h>
#include <kapp.h>

#include <qlist.h>
#include <qfile.h>
#include <qtstream.h>
#include <qapp.h>
#include <qregexp.h>
#include <qdict.h>

#include <kalarmtimer.h>

#include <gdbm.h>

extern QString krnpath,cachepath,artinfopath;
extern GDBM_FILE artdb;
extern GDBM_FILE refsdb;
extern QDict <char> unreadDict;

#include <mimelib/mimepp.h>

char debugbuf[1024];

#include "kfileio.h"

#include "TooManydlg.h"

#include "NNTP.moc"


extern KConfig *conf;
extern QDict <NewsGroup> groupDict;
TooManyDlg *toomanydlg;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
//  NNTP class. Implements the NNTP protocol (I hope)
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


NNTP::NNTP(char *host): DwNntpClient()
{
    hostname=host;
    Connected=false;
    Readonly=false;
    KDEBUG (KDEBUG_INFO,3300,hostname);
    reportBytes=false;
    reportCommands=false;
    byteCounter=0;
    commandCounter=0;
}

void NNTP::PGetTextResponse()
{
    mTextResponse = "";

    // Get a line at a time until we get CR LF . CR LF

    while (1) {
        char* ptr=0;
        int len;
        int err = PGetLine(&ptr, &len);

        // Check for an error

        if (err) {
            mReplyCode = 0;
            return;
        }
        if (!ptr) {
            mReplyCode = 0;
            return;
        }

        // Check for '.' on a line by itself, which indicates end of multiline
        // response

        if (len >= 3 && ptr[0] == '.' && ptr[1] == '\r' && ptr[2] == '\n') {
            break;
        }

        // Remove '.' at beginning of line

        if (*ptr == '.') ++ptr;

        mTextResponse.append(ptr, len);
        byteCounter=mTextResponse.length();
        qApp->processEvents();

        if (reportBytes && (byteCounter - oldbytes)>1024 )
        {
            char *buffer=new char[100];
            sprintf (buffer,klocale->translate("Received %.2f Kb"),
                    ((double)byteCounter)/1024);
            emit newStatus(buffer);
            qApp->processEvents();
            delete[] buffer;
            oldbytes=byteCounter;
         }
    }
}

void NNTP::resetCounters( bool byte,bool command)
{
    if (byte)
        byteCounter=0;
    if (command)
        commandCounter=0;
}
void NNTP::reportCounters (bool byte,bool command)
{
    reportBytes=byte;
    reportCommands=command;
}


NNTP::~NNTP()
{
}
bool NNTP::connect()
{
    if (Connected)
        return true;
    int status;
    conf->setGroup("NNTP");
    hostname=conf->readEntry("NNTPServer");

    if (!hostname) // no server in the env, nowhere to connect
        return false;
    else
    {
        KDEBUG (KDEBUG_INFO,3300,"Connecting to ...")
        KDEBUG (KDEBUG_INFO,3300,hostname.data());
        status=Open(hostname.data());
    }
    if (status!=200 && status!=201)
        return false;
    if (status==201)
        Readonly=true;
    if (!status)
        return false;
    
    Connected=true;
    
    //this is needed sometimes for some versions of INN
    status=setMode("reader");
    if (!status)
        return false;
    
    status=listOverview();
    if (!status)
        return false;

    return Connected;
}
bool NNTP::reConnect()
{
    return true;
}
bool NNTP::disconnect()
{
    int status=Quit();
    status=Close();
    Connected=false;
    return true;
}

int NNTP::authinfo(const char *username,const char *password)
{
    if (username)
    {
        sprintf (mSendBuffer,"authinfo user %s\r\n",username);
        cout << "C: " << mSendBuffer << endl;
        
        mReplyCode = -1;
        int bufferLen = strlen(mSendBuffer);
        int numSent = PSend(mSendBuffer, bufferLen);
        if (numSent == bufferLen)
        {
            PGetStatusResponse();
            cout <<"S: " << StatusResponse() << endl;
        }
    }
    if (password)
    {
        sprintf (mSendBuffer,"authinfo pass %s\r\n",password);

        // This prints the password. Not really a smart thing :-)
        // cout << "C: " << mSendBuffer << endl;
        
        mReplyCode = -1;
        int bufferLen = strlen(mSendBuffer);
        int numSent = PSend(mSendBuffer, bufferLen);
        if (numSent == bufferLen)
        {
            PGetStatusResponse();
            cout <<"S: " << StatusResponse() << endl;
        }
    }
    return mReplyCode;
}

int NNTP::listOverview()
{
    strcpy(mSendBuffer, "LIST overview.fmt\r\n");
    
    // for debugging
    cout << "C: " << mSendBuffer << endl;
    
    mReplyCode = -1;
    int bufferLen = strlen(mSendBuffer);
    int numSent = PSend(mSendBuffer, bufferLen);
    if (numSent == bufferLen) {
        PGetStatusResponse();
        cout <<"S: " << StatusResponse() << endl;
        if (mReplyCode/100%10 == 2)
        {
            PGetTextResponse();
            QString of=TextResponse().data();
            int index;
            
            index=of.find ("Subject");
            if (index!=-1)
                OffsetSubject=of.left(index).contains('\n')+1;
            else
                OffsetSubject=0;
            
            index=of.find ("From");
            if (index!=-1)
                OffsetFrom=of.left(index).contains('\n')+1;
            else
                OffsetFrom=0;
            
            index=of.find ("Lines");
            if (index!=-1)
                OffsetLines=of.left(index).contains('\n')+1;
            else
                OffsetLines=0;
            
            index=of.find ("Message-ID");
            if (index!=-1)
                OffsetID=of.left(index).contains('\n')+1;
            else
                OffsetID=0;
            
            index=of.find ("Date");
            if (index!=-1)
                OffsetDate=of.left(index).contains('\n')+1;
            else
                OffsetDate=0;
            
            index=of.find ("References");
            if (index!=-1)
                OffsetRef=of.left(index).contains('\n')+1;
            else
                OffsetRef=0;
        }
        else
        {
            OffsetSubject=1;
            OffsetFrom=2;
            OffsetDate=3;
            OffsetID=4;
            OffsetRef=5;
            OffsetLines=7;
        }
        sprintf (debugbuf,"Offsets:%d,%d,%d,%d,%d,%d",OffsetSubject,OffsetFrom,OffsetLines,OffsetID,OffsetDate,OffsetRef);
        KDEBUG(KDEBUG_INFO,3300,debugbuf);
    }
    return mReplyCode;
}


int NNTP::setMode (char *mode)
{
    sprintf (mSendBuffer,"mode %s\r\n",mode);
    cout << "C: " << mSendBuffer << endl;

    mReplyCode = -1;
    int bufferLen = strlen(mSendBuffer);
    int numSent = PSend(mSendBuffer, bufferLen);
    if (numSent == bufferLen)
    {
        PGetStatusResponse();
        cout <<"S: " << StatusResponse() << endl;
    }
    return mReplyCode;
}

int NNTP::listXover(int from,int to,NewsGroup *n)
{
    datum key;
    char *buffer=new char[1024];
    unsigned int counter=0;
    counter=n->artList.count();
    DwString gi;
    reportCounters (true,false);
    setGroup(n->name);
    debug ("first-->%d,last-->%d,from-->%d,to-->%d",first,last,from,to);
    if (from <first)
        from=first;
    if (to > last)
        to=last;
    if (to<from)
    {
        debug ("weird xover with to <from");
        to=from;
    }

    conf->setGroup("NNTP");
    int toomany=conf->readNumEntry("TooMany");
    if ((to-from)>toomany)
    {
        debug ("Way too many!");
        if (!toomanydlg)
            toomanydlg=new TooManyDlg();
        toomanydlg->exec();

        if (toomanydlg->frombottom)
        {
            from=(n->lastArticle(this)>?first);
            to=from+toomanydlg->howmany;
        }
        else
        {
            to=last;
            from=to-toomanydlg->howmany;
        }
    }

    int reallast=from-1;

    if (to)
        sprintf(mSendBuffer, "xover %d-%d\r\n",from-1,to);
    else
        sprintf(mSendBuffer, "xover %d-\r\n",from-1);
    
    // for debugging
    cout << "C: " << mSendBuffer << endl;
    
    mReplyCode = 0;
    mStatusResponse = mTextResponse = "";

    int bufferLen = strlen(mSendBuffer);
    int numSent = PSend(mSendBuffer, bufferLen);
    if (numSent == bufferLen)
    {
        PGetStatusResponse();
        cout <<"S: " << StatusResponse() << endl;
        if (mReplyCode/100%10 == 2)
        {
            PGetTextResponse();
            
            QString p,qit;
            p=krnpath+GroupName;
            QFile f(p);
            if (!f.open(IO_Append | IO_WriteOnly))
            {
                printf ("Can't open file %s for writing!\n",p.data());
            }
            else
            {
                if (!mTextResponse.length())
                {
                    f.close();
                }
                //First break it up in an article list
                class Article art;
                QString xoverdata(mTextResponse.c_str());
                int index=0;
                int oldindex=0;
                while (1)
                {
                    index=xoverdata.find('\n',oldindex);
                    if (index==-1)
                        break;
                    QString line=xoverdata.mid (oldindex,index-oldindex);
                    oldindex=index+1;
                    if (line.isEmpty())
                        continue;
                    QStrList templ;
                    templ.setAutoDelete (true);
                    qit=line;
                    int index;
                    do
                    {
                        index=qit.find('\t');
                        if (index==-1)
                        {
                            templ.append(qit);
                            break;
                        }
                        templ.append(qit.left(index));
                        qit=qit.right(qit.length()-index-1);
                    }
                    while (!qit.isEmpty());
                    reallast=QString (templ.at(0)).toInt();
                    art.ID=templ.at(OffsetID);

                    // check if it exists read
                    QString k="R";
                    k+=art.ID;
                    key.dptr=k.data();
                    key.dsize=k.length()+1;
                    if ((!gdbm_exists(artdb,key)))
                    {
                        key.dptr=art.ID.data();
                        key.dsize=art.ID.length()+1;
                        if ((!gdbm_exists(artdb,key)))
                        {
                            art.Subject=templ.at(OffsetSubject);
                            art.From=templ.at(OffsetFrom);
                            art.Date=templ.at(OffsetDate);
                            art.Lines=templ.at(OffsetLines);
                            art.desperate=group();
                            art.desperate+="@";
                            art.desperate+=templ.at(0);

                            QString refsdata=templ.at(OffsetRef);
                            datum refs;
                            refs.dptr=refsdata.data();
                            refs.dsize=refsdata.length();
                            
                            gdbm_store(refsdb,key,refs,GDBM_REPLACE);
                            
                            unreadDict.replace(art.ID.data(),art.ID.data());
                            art.save();
                        }
                    }
                    if (!art.ID.data())
                        debug ("broken article in listXover");
                    n->addArticle(art.ID);
                    counter++;
                    //Write the article ID to the newsgroup file
                    gi+=templ.at(OffsetID);
                    gi+="\n";
                    if (!(counter%10))
                        sprintf (buffer,"Received %d articles",counter);
                    emit newStatus(buffer);
                }
                f.writeBlock(gi.data(),gi.length());
                f.close();
                gdbm_sync(artdb);
                mTextResponse.clear();
                n->saveLastArticle(this,reallast);
            }
        }
        else
        {
            warning ("Can't get XOVER data from your server");
            warning ("Server said %s",StatusResponse().data());
        }
    }
    delete[] buffer;
    resetCounters (true,true);
    return mReplyCode;
}

void NNTP::groupList(bool fromserver)
{
    reportCounters (true,false);
    QString ac;
    ac=krnpath+"/active";
    QFile f(ac.data());
    
    if (fromserver)
    {
        int status=List();
        if (status!=215)
        {
            sprintf (debugbuf,"error getting group list\nServer said %s\n",
                     StatusResponse().c_str());
            KDEBUG (KDEBUG_ERROR,3300,debugbuf);
            return;
        };
        TextResponse();
        if (!mTextResponse.length())
        {
            return;
        }

        QString tstr;
        if (QFile::exists(ac.data()))
            tstr=kFileToString(ac.data());
        tstr+=mTextResponse.c_str();
        if(kStringToFile(tstr,ac.data(),false,true))
        {
            QString command="cat ";
            command=command+ac+"| cut -d\" \" -f1 |sort|uniq >"+ac+"1; mv "+ac+"1 "+ac;
            system (command.data());
        }
        mTextResponse.clear();
        groupList(false);
    }
    else //read it from the active file
    {
        if(f.open (IO_ReadOnly))
        {
            QTextStream st(&f);
            while (1)
            {
                QString s=st.readLine();
                if (s.isEmpty())
                    break;
//                s=s.left(s.find(' '));
                NewsGroup *gr=groupDict.find(s.data());
                if (!gr)
                {
                    gr=new NewsGroup(s);
                    groupDict.insert(s.data(),gr);
                }
            }
            f.close();
        }
    };
    resetCounters (true,true);
}

bool NNTP::setGroup(const char *groupname)
{
    bool success=false;
    GroupName=groupname;
    
    int status=Group(GroupName.data());
    
    if (status!=211)
    {
        sprintf (debugbuf,"can't change group!\nserver said: %s\n",
                 StatusResponse().data());
        KDEBUG(KDEBUG_ERROR,3300,debugbuf);
        GroupName="";
    }
    else
    {
        int j;
        QString l=StatusResponse().data();
        j=l.find(' ');
        l=l.right(l.length()-j-1);
        j=l.find(' ');
        howmany=l.left(j).toInt();
        l=l.right(l.length()-j-1);
        
        j=l.find(' ');
        first=l.left(j).toInt();
        l=l.right(l.length()-j-1);
        
        j=l.find(' ');
        last=l.left(j).toInt();
        success=true;
    }
    return success;
}



bool NNTP::artList(int from,int to,NewsGroup *n)
{
    int status=listXover(from,to,n);
    return (status>199);
}

MessageParts NNTP::isCached (const char *_id)
{
    QString id=saneID(_id);
    int result=0;
    QString path=cachepath+"/"+id;
    if (QFile::exists(path.data()))
        result=result|PART_ALL;
    path=cachepath+"/"+id+".head";
    if (QFile::exists(path.data()))
        result=result|PART_HEAD;
    path=cachepath+"/"+id+".body";
    if (QFile::exists(path.data()))
        result=result|PART_BODY;
    return (MessageParts)result;
}

QString *NNTP::article(const char *_id)
{
    emit newStatus(klocale->translate("Getting Article"));
    QString id=saneID(_id);
    QString p=cachepath;
    QString *data=new QString("");
    p=p+id;
    if (isCached (id) == PART_ALL)//it exists and is fully cached
    {
        debug ("has all, getting nothing");
        if (QFile::exists(p)) //old style cache
            data->setStr(kFileToString(p).data());
        else
        {
            data->setStr(kFileToString(p+".head"));
            data->append("\n\n");
            data->append(kFileToString(p+".body"));
        }
        return data;
    }

    else if (isCached (id)==PART_NONE) //get all of it, write it and return it
    {
        debug ("has nothing, getting all");
        bool success=false;
        int status=Article (id);

        if (!status)
        {
            emit lostServer();
            return data;
        }

        debug ("status-->%d",status);
        if (status==220)
        {
            QString a(TextResponse().c_str());
            int limit=a.find("\r\n\r\n");
            debug ("limit-->%d",limit);
            kStringToFile(a.left(limit),p+".head",FALSE,FALSE,TRUE);
            kStringToFile(a.right(a.length()-limit-4),p+".body",FALSE,FALSE,TRUE);
            delete data;
            data=article(id);
            success=true;
        }
        //for some reason some INN servers force
        //readers to get head and body separately!
        else if (status==223)
        {
            status=Head(id);
            if (status==221)
            {
                QString s(TextResponse().c_str());
                kStringToFile(s,p+".head",FALSE,FALSE,TRUE);
                status=Body(id);
                if (status==222)
                {
                    s=(TextResponse().c_str());
                    kStringToFile(s,p+".body",FALSE,FALSE,TRUE);
                }
                else
                    success=false;
            }
            else success=false;
        }
        if (!success) //desperate way to get articles from broken servers
                      //that don't support "article <ID>"
        {
            class Article artie(_id);
            artie.load();
            debug ("desper->%s",artie.desperate.data());
            if (artie.desperate.isEmpty())
            {
                 success=false;
            }
            else
            {
                 int sep=artie.desperate.find('@');
                 if (sep==-1)
                 {
                     success=false;
                 }
                 else
                 {
                     QString g=artie.desperate.left(sep);
                     QString ind=artie.desperate.right(artie.desperate.length()-sep-1);
                     debug ("q-->%s+++ind-->%s+++",g.data(),ind.data());
                     //if not in the right group and can't go to it
                     //we're screwed
                     if (GroupName!=g && (!setGroup(g)))
                     {
                             success=false;
                     }
                     else //I got to the right group, soI get it by number
                     {
                         int status=Article (ind);

                         if (!status)
                         {
                             emit lostServer();
                             return data;
                         }

                         debug ("status-->%d",status);
                         if (status==220)
                         {
                             QString a(TextResponse().c_str());
                             int limit=a.find("\r\n\r\n");
                             debug ("limit-->%d",limit);
                             kStringToFile(a.left(limit),p+".head",FALSE,FALSE,TRUE);
                             kStringToFile(a.right(a.length()-limit-4),p+".body",FALSE,FALSE,TRUE);
                             delete data;
                             data=article(id);
                             success=true;
                         }
                         else
                             success=false;
                     }
                 }
            }

        }
        if (!success)
        {
            warning ("error getting data\nserver said %s\n",StatusResponse().c_str());
            unlink (p.data());
        }
    }

    else if (isCached(id)==PART_HEAD)
    {
        debug ("has head, getting body");
        data=body(id);
        delete data;
        data=article(id);
    }

    else if (isCached(id)==PART_BODY)
    {
        debug ("has body, getting head");
        data=head(id);
        delete data;
    }
    
    emit newStatus(klocale->translate("Article Received"));
    return data;
}
QString *NNTP::head(const char *_id)
{
    QString id=saneID(_id);
    QString *data=new QString ("");
    QString p=cachepath+id;
    if (isCached(id) & PART_HEAD)
        data->append(kFileToString(p+".head"));
    else
    {
        int status=Head(id);
        if (status==221)
        {
            data->append(TextResponse().c_str());
            kStringToFile(*data,p+".head",FALSE,FALSE,TRUE);
        }
    }
    return data;
}
QString *NNTP::body(const char *_id)
{
    QString id=saneID(_id);
    QString *data=new QString ("");
    QString p=cachepath+id;
    if (isCached(id) & PART_BODY)
        data->append(kFileToString(p+".body"));
    else
    {
        int status=Body(id);
        if (status==222)
        {
            data->append(TextResponse().c_str());
            kStringToFile(*data,p+".body",FALSE,FALSE,TRUE);
        }
    }
    return data;
}

bool NNTP::getMissingParts(MessageParts parts,const char *id)
{
    bool success=false;

    if (parts & PART_NONE)
        return true;
    QString *data=0;
    if (parts == PART_ALL)
    {
        if (isCached(id)!=PART_ALL)
            data=article(id);
    }
    
    else if (parts == PART_BODY)
    {
        if (!(isCached(id)&PART_BODY))
            data=body(id);
    }
    else if (parts == PART_HEAD)
    {
        if (!(isCached(id)&PART_HEAD))
            data=head(id);
    }
    if (data)
        success=true;
    delete data;
    return success;
}


QString NNTP::saneID(const char *id)
{
    QString r(id);
    // Why some braindead newsreader create ID's with slashes?????
    // A virtual cookie to who guess which one does this...
    return r.replace (QRegExp("/"),"\\#slash#\\");
}

void NNTP::refresh()
{
    debug ("refreshing");
    qApp->processEvents();
    debug ("refreshed");
}

bool NNTP::checkDisconnection()
{
    if (!mReplyCode)
        return false;
    return true;
}

