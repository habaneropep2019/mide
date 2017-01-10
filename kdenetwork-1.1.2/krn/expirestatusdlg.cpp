#include "expirestatusdlg.h"

#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <gdbm.h>

#include <qlabel.h>
#include <qapp.h>
#include <qdir.h>
#include <qtimer.h>
#include <qstring.h>
#include <kconfig.h>
#include <kapp.h>

#include "NNTP.h"

#include "expirestatusdlg.moc"

#define SECOND 1
#define MINUTE SECOND*60
#define HOUR   MINUTE*60
#define DAY    HOUR*24

extern GDBM_FILE artdb;
extern KConfig *conf;
extern QDict <NewsGroup> groupDict;

extern QString krnpath,cachepath,artinfopath,groupinfopath,dbasepath,outpath;

ExpireStatusDlg::ExpireStatusDlg()
    :QDialog (0,0,true)
{
    QLabel *l=new QLabel (i18n("Please wait a minute, I am expiring!"),
                          this);
    l->adjustSize();
    adjustSize();
    QTimer::singleShot(2000,this,SLOT(doExpire()));
    show();
}

void ExpireStatusDlg::doExpire()
{
    QDictIterator <NewsGroup> it(groupDict);
    time_t threshold;
    time_t threshold2;
    
    NewsGroup *iter;

    conf->setGroup("Cache");
    threshold=time(NULL)-DAY*conf->readNumEntry("ExpireReadBodies",5);
    
    QDir d(cachepath.data());
    d.setFilter(QDir::Files);
    QStrList *files=new QStrList (*d.entryList("*"));
    
    struct stat st;
    char filename[255];

    
    for (char *fname=files->first();fname!=0;fname=files->next())
    {
        sprintf(filename, "%s%s", cachepath.data(), fname);
        
        
        if(stat(filename, &st))
        {
            debug("couldn't stat %s", filename);
        } else {
            if(st.st_atime<threshold)
            {
                Article *art = new Article();
                
                art->ID = fname;
                
                art->load();
                
                if(art->canExpire())
                    unlink(filename);
                
                delete art;
            }
        }
    }
    delete files;
    
    datum key,nextkey;
    datum content;
    int index;
    
    QStrList tl;
    tl.setAutoDelete(true);
    QString t,s;
    
    
    // Expire new articles
    
    conf->setGroup("Cache");
    threshold=time(NULL)-DAY*conf->readNumEntry("ExpireUnreadHeaders",5);
    threshold2=time(NULL)-DAY*conf->readNumEntry("ExpireReadHeaders",5);
    key= gdbm_firstkey(artdb);
    while (key.dptr)
    {
        nextkey=gdbm_nextkey (artdb,key);
        
        content=gdbm_fetch(artdb,key);
        s=(char *)content.dptr;
        
        while (1)
        {
            index=s.find("\n");
            if (index==-1)
            {
                tl.append(s);
                break;
            }
            t=s.left (index);
            s=s.right(s.length()-index-1);
            if (t.isEmpty())
                continue;
            tl.append (t.data());
        }

        if (!strcmp(tl.at(7),"1"))
        {
            time_t lastAccess=atol(tl.at(9));

            if (key.dptr[0]!='R')
            {
                if (threshold>lastAccess)
                {
                    gdbm_delete(artdb,key);
                    debug ("expire 1");
                    free (key.dptr);
                }
            }
            else
            {
                if (threshold2>lastAccess)
                {
                    gdbm_delete(artdb,key);
                    debug ("expire 1");
                    free (key.dptr);
                }
            }
        }
        tl.clear();
        key=nextkey;
    }

    for (it.toFirst();it.current(); ++it)
    {
        iter=it.current();
        if (iter->dirty)
            iter->clean();
    }

//    qApp->quit();
    delete this;
}

ExpireStatusDlg::~ExpireStatusDlg()
{
}
