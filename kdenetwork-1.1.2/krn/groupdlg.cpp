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

#include <unistd.h>

#include "groupdlg.h"

#define Inherited KTopLevelWidget

#include <qapp.h>
#include <qfile.h>
#include <qstring.h>
#include <qtstream.h>
#include <qevent.h>
#include <qdir.h>
#include <qdict.h>

#include <kapp.h>
#include <kmsgbox.h>
#include <kconfig.h>

#include "artdlg.h"
#include "identDlg.h"
#include "expireDlg.h"
#include "NNTPConfigDlg.h"
#include "rmbpop.h"
#include "aboutDlg.h"
#include "printdlg.h"
#include "asker.h"

#include "krnsender.h"
#include "kmcomposewin.h"
#include "kmreaderwin.h"
#include "kmidentity.h"
#include "kfileio.h"
#include "kbusyptr.h"
#include "printdlg.h"

#include "groupdlg.moc"


#define CONNECT 1
#define DISCONNECT 5
#define GET_ACTIVE 3
#define SUBSCRIBE 4
#define CHECK_UNREAD 6
#define CHANGE_IDENTITY 8
#define CONFIG_NNTP 9
#define EXIT 7
#define OPENGROUP 10
#define TAGGROUP 11
#define HELP_CONTENTS 12
#define HELP_ABOUT 13
#define GET_SUBJECTS 14
#define GET_ARTICLES 15
#define CATCHUP 16
#define FIND_GROUP 17
#define POST 18
#define POST_QUEUED 19
#define CONFIG_EXPIRE 20
#define CONFIG_PRINT 21

extern QString krnpath,cachepath,artinfopath,pixpath,outpath;
extern KConfig *conf;
extern QDict <NewsGroup> groupDict;

extern KMIdentity *identity;
extern KRNSender *msgSender;
extern KBusyPtr *kbp;

NNTPConfigDlg *nntpconf;

QStrList subscr;
QStrList tagged;

bool checkPixmap(KTreeViewItem *item,void *)
{
    QPixmap p;
    QString name(item->getText());
    
    if (name.right(1)==".") //it's a folder
    {
        p=kapp->getIconLoader()->loadIcon("krnfolder.xpm");
        item->setPixmap(p);
        return false;
    }
    int i=name.find(' ');
    if (i>0)
    {
        name=name.left(i);;
    };
    if (tagged.find(name.data())!=-1) //it's tagged
    {
        p=kapp->getIconLoader()->loadIcon("tagged.xpm");
        item->setPixmap(p);
        return false;
    }
    if (subscr.find(name.data())!=-1) //it's subscribed
    {
        p=kapp->getIconLoader()->loadIcon("subscr.xpm");
        item->setPixmap(p);
        return false;
    }
    p=kapp->getIconLoader()->loadIcon("followup.xpm");
    item->setPixmap(p); //it's plain
    return false;
}

Groupdlg::Groupdlg(const char *name):Inherited (name)
{
    subscr.setAutoDelete(false);
    tagged.setAutoDelete(false);
    
    QPopupMenu *file = new QPopupMenu;
    file->insertItem(klocale->translate("Connect to Server"),CONNECT);
    file->insertItem(klocale->translate("Disconnect From Server"),DISCONNECT);
    file->insertItem(klocale->translate("Get Active File"),GET_ACTIVE);
    file->insertSeparator();
    file->insertItem(klocale->translate("Post Queued Messages"),POST_QUEUED);
    file->insertSeparator();
    file->insertItem(klocale->translate("Exit"),EXIT);
    connect (file,SIGNAL(activated(int)),SLOT(currentActions(int)));
    
    setCaption (klocale->translate("KRN - Group List"));
    
    QPopupMenu *subscribed = new QPopupMenu;
    subscribed->insertItem(klocale->translate("Get Subjects"),GET_SUBJECTS);
    subscribed->insertItem(klocale->translate("Get Articles"),GET_ARTICLES);
    subscribed->insertItem(klocale->translate("Catchup"),CATCHUP);
    connect (subscribed,SIGNAL(activated(int)),SLOT(subscrActions(int)));
    
    QPopupMenu *tagged = new QPopupMenu;
    tagged->insertItem(klocale->translate("Get Subjects"),GET_SUBJECTS);
    tagged->insertItem(klocale->translate("Get Articles"),GET_ARTICLES);
    tagged->insertItem(klocale->translate("(un)Subscribe"),SUBSCRIBE);
    tagged->insertItem(klocale->translate("Untag"),TAGGROUP);
    tagged->insertItem(klocale->translate("Catchup"),CATCHUP);
    connect (tagged,SIGNAL(activated(int)),SLOT(taggedActions(int)));
    
    QPopupMenu *newsgroup = new QPopupMenu;
    newsgroup->insertItem(klocale->translate("Open"),OPENGROUP);
    newsgroup->insertItem(klocale->translate("Find"),FIND_GROUP);
    newsgroup->insertItem(klocale->translate("(un)Subscribe"),SUBSCRIBE);
    newsgroup->insertItem(klocale->translate("(Un)Tag"),TAGGROUP);
    newsgroup->insertItem(klocale->translate("Catchup"),CATCHUP);
    newsgroup->insertSeparator();
    newsgroup->insertItem(klocale->translate("Post New Article"),POST);
    newsgroup->insertItem(klocale->translate("Post All Queued Articles"),POST_QUEUED);
    newsgroup->insertSeparator();
    newsgroup->insertItem (klocale->translate("&Subscribed"), subscribed);
    newsgroup->insertItem (klocale->translate("&Tagged"), tagged);
    
    connect (newsgroup,SIGNAL(activated(int)),SLOT(currentActions(int)));
    
    QPopupMenu *options = new QPopupMenu;
    options->insertItem(klocale->translate("Identity..."),CHANGE_IDENTITY);
    options->insertItem(klocale->translate("NNTP Options..."),CONFIG_NNTP);
    options->insertItem(klocale->translate("Expire Options..."),CONFIG_EXPIRE);
    options->insertItem(klocale->translate("Printing Options..."),CONFIG_PRINT);
    connect (options,SIGNAL(activated(int)),SLOT(currentActions(int)));


    QPopupMenu *help = new QPopupMenu;
    help->insertItem(klocale->translate("Contents"),HELP_CONTENTS);
    help->insertSeparator();
    help->insertItem(klocale->translate("About Krn..."),HELP_ABOUT);
    connect (help,SIGNAL(activated(int)),SLOT(currentActions(int)));

    /*
     QPopupMenu *help = kapp->getHelpMenu(true,"Krn 0.2.5");
    */
    
    KMenuBar *menu = new KMenuBar(this);
    setMenu(menu);
    addToolBar(new KToolBar(this));
    
    menu->insertItem (klocale->translate("&File"), file);
    menu->insertItem (klocale->translate("&Newsgroup"), newsgroup);
    menu->insertItem (klocale->translate("&Options"), options);
    menu->insertSeparator();
    menu->insertItem (klocale->translate("&Help"), help);
    
    QObject::connect (toolBar(), SIGNAL (clicked (int)), this, SLOT (currentActions (int)));
    
    toolBar()->insertButton (Icon("connected.xpm"), CONNECT, true, klocale->translate("Connect to server"));
    
    toolBar()->insertButton (Icon("disconnected.xpm"), DISCONNECT, false, klocale->translate("Disconnect from server"));
    toolBar()->insertSeparator ();
    
    toolBar()->insertButton (Icon("previous.xpm"), GET_ACTIVE, true, klocale->translate("Get list of active groups"));
    toolBar()->insertButton (Icon("find.xpm"), FIND_GROUP, true, klocale->translate("Find group"));
    toolBar()->insertSeparator ();
    
    toolBar()->insertButton (Icon("subscr.xpm"), SUBSCRIBE, true, klocale->translate("(Un)Subscribe"));
    toolBar()->insertSeparator ();
    
    toolBar()->insertButton (Icon("reload.xpm"), CHECK_UNREAD, true, klocale->translate("Check for Unread Articles"));
    
    toolBar()->insertButton (Icon("filenew.xpm"), POST, true, klocale->translate("Post New Article"));
    
    list = new MyTreeList (this, "");
    QObject::connect (list, SIGNAL (selected (int)), this, SLOT (openGroup (int)));
    QObject::connect (list, SIGNAL (expanding (KTreeViewItem*,bool&)), this, SLOT (openGroup (KTreeViewItem*,bool&)));
    setView (list);
    RmbPop *filter=new RmbPop(list);
    delete (filter->pop);
    filter->pop=newsgroup;
    
    
    KStatusBar *status = new KStatusBar(this);
    setStatusBar( status );
    status->insertItem ("                               ", 1);
    status->insertItem ("", 2);
    
    conf->setGroup("NNTP");
    QString sname=conf->readEntry("NNTPServer");
    server = new NNTP (sname.data());
    connect (server,SIGNAL(lostServer()),SLOT(lostConnection()));
    server->reportCounters (true,false);
    
    msgSender->setNNTP(server);
    
    connect (server,SIGNAL(newStatus(const char *)),this,SLOT(updateCounter(const char *)));
    conf->setGroup("Geometry");
    setGeometry(conf->readNumEntry("GroupX",100),
                conf->readNumEntry("GroupY",40),
                conf->readNumEntry("GroupW",400),
                conf->readNumEntry("GroupH",400));
    show();
    loadSubscribed();
    fillTree();
    
    conf->setGroup("NNTP");
    if (conf->readNumEntry("ConnectAtStart"))
        actions(CONNECT);
    
    qApp->processEvents();
    activeloaded=false;
    //Open group windows
}

Groupdlg::~Groupdlg ()
{
    //    saveProperties(false); // Kalle: No longer needed
    QStrList openwin;
    //check for all open groups, and close them
    QDictIterator <NewsGroup> iter(groupDict);
    NewsGroup *g;
    while (iter.current())
    {
        g=iter.current();
        if (g->isVisible)
        {
//            openwin.append (g->name);
//            delete g->isVisible;
        }
        if (g->dirty)
            g->clean();
        ++iter;
    }
    conf->setGroup("ArticleListOptions");
    conf->writeEntry("OpenWindows",openwin);
    conf->setGroup("Geometry");
    conf->writeEntry("GroupX",x());
    conf->writeEntry("GroupY",y());
    conf->writeEntry("GroupW",width());
    conf->writeEntry("GroupH",height());
    conf->sync();
}

void Groupdlg::openGroup (QString name)
{
    NewsGroup *n=groupDict.find(name.data());
    if (n)
    {
        if (n->isVisible)
        {
            return;
        }
        n->load();
        Artdlg *a=0;
        QDictIterator <NewsGroup> it(groupDict);

        conf->setGroup("ArticleListOptions");
        bool singleWin=conf->readNumEntry("SingleWindow",true);

        if (singleWin)
        {
            NewsGroup *iter;
            for (;it.current(); ++it)
            {
                iter=it.current();
                if (iter->isVisible)
                {
                    a=iter->isVisible;
                    break;
                }
            }
        }

        if (!a)
        {
            a = new Artdlg (n,server);
            QObject::connect(a->messwin,SIGNAL(spawnGroup(QString)),
                             this,SLOT(openGroup(QString)));
            QObject::connect(a,SIGNAL(needConnection()),
                             this,SLOT(needsConnect()));
        }
        else
            a->init(n,server);
    }
    else
    {
        debug ("weird, the group ain't in the groupDict!");
    }
}

void Groupdlg::openGroup (KTreeViewItem *item, bool&)
{
    item->setDelayedExpanding(false);
    openGroup(item);
}

void Groupdlg::openGroup (int index)
{
    KTreeViewItem *it=list->itemAt(index);
    openGroup(it);
}

void Groupdlg::openGroup (KTreeViewItem *it)
{
    QPixmap p;
    qApp->setOverrideCursor(waitCursor);
    QString base;

    if (it->getText()[it->getText().length()-1]!='.')
    {
        QString temp=it->getText();
        int i=temp.find(' ');
        if (i>0)
        {
            temp=temp.left(i);;
        };
        openGroup(temp);
    }
    else
    {
        if (!it->hasChild())
        {
            if (!strcmp(klocale->translate("All Newsgroups."),it->getText()))
            {
                base="";
                loadActive();
            }
            else
            {
                base=it->getText();
            }
            int l=base.length();
            int c=base.contains('.');
            char tc;
            QDictIterator <NewsGroup> iterat(groupDict);
            NewsGroup *iter;

	    // preload pixmaps
	    QPixmap followup = 
	      kapp->getIconLoader()->loadIcon("followup.xpm");
	    QPixmap krnfolder = 
	      kapp->getIconLoader()->loadIcon("krnfolder.xpm");


            //First make a list of all matches with
            //the exact same beginning, and either the same or one extra
            //dots. In two sorted lists.

            // we must insert non-NULL pointer into the dict,
	    // so we take a dummy
            int nullItem = 0;
            
            QStrList items;
            items.setAutoDelete(true);
            QDict<int> dict(2423);
            for (;iterat.current(); ++iterat)
            {
                iter=iterat.current();
                //this group's name matches the base
                if (l == 0 || !strncmp(base.data(),iter->name,l))
                {
                    int c1=QString(iter->name).contains('.');
                    if (c1==c)//leaf
                        items.inSort(iter->name);
                    else if(c1>c)
                    {
                        char *nextdot=strchr(iter->name+l+1,'.')+1;
                        tc=nextdot[0];
                        nextdot[0]=0;
//                        strcpy(buffer,iter->name);
                        if (!dict.find(iter->name))
                        {
                            // It's new, so add it to the base list
                            // and add it to the branch list
                            dict.insert(iter->name, &nullItem);
                            items.inSort(iter->name);
//                            items.append(buffer);
                        }
                        nextdot[0]=tc;
                    }
                }
            }

            //Now we add all the items to the tree

            for (char *name=items.first();name!=0;name=items.next())
            {
		KTreeViewItem *itemp;
                if (name[strlen(name)-1]=='.')
                {
                    itemp=new KTreeViewItem(name,krnfolder);
                    itemp->setDelayedExpanding(true);
                    list->appendChildItem(it,itemp);
                }
                else
                {
                    itemp=new KTreeViewItem(name,followup);
                    list->appendChildItem(it,itemp);
                }
		itemp->setDeleteChildren(true);
            }

            list->expandItem(list->itemRow(it),false);
            list->forEveryVisibleItem(checkPixmap,NULL);
            list->repaint();

	    // This fixes a bug in the treeview: the scrollbars are
	    // not updated properly if the widget is not resized
	    list->resize(list->size());
        }
        else
        {
            if (it->isExpanded())
                list->collapseItem(list->itemRow(it),false);
            else
                list->expandItem(list->itemRow(it),false);
        }
    }
    qApp->restoreOverrideCursor();
}




void Groupdlg::subscribe (NewsGroup *group)
{
    QPixmap p;
    KPath path;
    int index=subscr.find (group->name);
    if (-1 != index)
    {
        subscr.remove ();
        path.push (new QString (klocale->translate("Subscribed Newsgroups.")));
        path.push (new QString (group->name));
        int l=list->currentItem();
        list->setCurrentItem(0);
        list->removeItem (path);
        if (list->itemAt(0)->isExpanded() &&
            ((unsigned int)l>list->itemAt(0)->childCount()+1))
            list->setCurrentItem(l-1);
        else
            list->setCurrentItem(l);
    }
    else
    {
        if (groupDict.find (group->name))
        {
            p=kapp->getIconLoader()->loadIcon("subscr.xpm");
            list->appendChildItem (list->itemAt(0),new KTreeViewItem(group->name, p));
            subscr.append (group->name);
            if (list->itemAt(0)->isExpanded() &&
                ((unsigned int)list->currentItem()>list->itemAt(0)->childCount()+1))
                list->setCurrentItem(list->currentItem()+1);
        }
    };
    list->forEveryVisibleItem(checkPixmap,NULL);
    list->repaint();
    saveSubscribed();
}

void Groupdlg::tag (NewsGroup *group)
{
    if(tagged.find(group->name)>-1)
    {
        tagged.remove(group->name);
    }
    else
    {
        tagged.append(group->name);
    }
    list->forEveryVisibleItem(checkPixmap,NULL);
    list->repaint();
}


void Groupdlg::offline()
{
    msgSender=0;
    toolBar()->setItemEnabled (DISCONNECT,false);
    server->disconnect();
    toolBar()->setItemEnabled (CONNECT,true);
    statusBar ()->changeItem (klocale->translate("Disconnected"), 2);
    qApp->processEvents ();
}

void Groupdlg::online()
{
    toolBar()->setItemEnabled (CONNECT,false);
    statusBar ()->changeItem (klocale->translate("Connecting to server"), 2);
    qApp->processEvents ();
    if (server->connect ())
    {
        if (server->isReadOnly ())
            statusBar ()->changeItem (klocale->translate("Connected to server - Posting not allowed"), 2);
        else
            statusBar ()->changeItem (klocale->translate("Connected to server - Posting allowed"), 2);
        toolBar()->setItemEnabled (DISCONNECT,true);
    }
    else
    {
        qApp->setOverrideCursor (arrowCursor);
        KMsgBox:: message (0, klocale->translate("Error"),klocale->translate( "Can't connect to server"), KMsgBox::INFORMATION);
        qApp->restoreOverrideCursor ();
        statusBar ()->changeItem (klocale->translate("Connection to server failed"), 2);
        qApp->processEvents ();
        toolBar()->setItemEnabled (CONNECT,true);
    }
    if (conf->readNumEntry("Authenticate")!=0)
    {
        if (299<server->authinfo(conf->readEntry("Username"),conf->readEntry("Password")))
        {
            qApp->setOverrideCursor (arrowCursor);
            KMsgBox:: message (0, klocale->translate("Error"), klocale->translate("Authentication Failed"), KMsgBox::INFORMATION);
            qApp->restoreOverrideCursor ();
            actions(DISCONNECT);
            statusBar ()->changeItem (klocale->translate("Connection to server failed: Authentication problem"), 2);
            qApp->processEvents ();
            toolBar()->setItemEnabled (CONNECT,true);
        }
    }
}

void Groupdlg::fillTree ()
{
    QPixmap p;
    p=kapp->getIconLoader()->loadIcon("krnfolder.xpm");
    KTreeViewItem *item=new KTreeViewItem(klocale->translate("Subscribed Newsgroups."), p);
    if (subscr.count()>0)
        item->setDelayedExpanding(true);
    list->insertItem (item);
    item->setDeleteChildren(true);
    QStrListIterator it(subscr);
    it.toFirst();
    NewsGroup *g;
    for (;it.current();++it)
    {
        g=groupDict.find(it.current());
        if (!g)
            continue;
        p=kapp->getIconLoader()->loadIcon("subscr.xpm");
        list->appendChildItem (item,new KTreeViewItem(g->name, p));
    }

    p=kapp->getIconLoader()->loadIcon("krnfolder.xpm");
    item=new KTreeViewItem(klocale->translate("All Newsgroups."), p);
    item->setDelayedExpanding(true);
    list->insertItem (item);
    item->setDeleteChildren(true);
}

bool Groupdlg::needsConnect()
{
    
    bool success=false;
    qApp->setOverrideCursor (arrowCursor);
    if (server->isConnected())
    {
        success=true;
    }
    else
        
    {
        conf->setGroup("NNTP");
        if (conf->readNumEntry("SilentConnect") ||
            (1==KMsgBox::yesNo(0,klocale->translate("Krn-Question"),
                               klocale->translate("The operation you requested needs a connection to the News server\nShould I attempt one?")))
           )
        {
            actions(CONNECT);
            success=true;
        }
    }
    qApp->restoreOverrideCursor ();
    return success;
}


bool Groupdlg::actions (int action,NewsGroup *group)
{
    bool success=false;
    qApp->setOverrideCursor (waitCursor);
    switch (action)
    {
    case GET_SUBJECTS:
        {
            if (!group)
                break;
            getSubjects(group);
            break;
        }
    case GET_ARTICLES:
        {
            if (!group)
                break;
            getArticles(group);
            break;
        }
    case OPENGROUP:
        {
            openGroup (group->name);
            break;
        }
    case TAGGROUP:
        {
            if (!group)
                break;
            tag(group);
            break;
        }
    case CHANGE_IDENTITY:
        {
            qApp->setOverrideCursor (arrowCursor);
            IdentDlg id;
            if (id.exec())
            {
                identity->setEmailAddr(id.address->text());
                identity->setFullName(id.realname->text());
                identity->setOrganization(id.organization->text());
                identity->writeConfig();
            }
            qApp->restoreOverrideCursor ();
            success = true;
            break;
        }
        
    case CONFIG_NNTP:
        {
            qApp->setOverrideCursor (arrowCursor);
            if (!nntpconf)
                 nntpconf=new NNTPConfigDlg();
            nntpconf->exec();
            qApp->restoreOverrideCursor ();
            success = true;
            break;
        }

    case CONFIG_PRINT:
        {
            qApp->setOverrideCursor (arrowCursor);
            PrintDlg *dlg=new PrintDlg();
            dlg->exec();
            qApp->restoreOverrideCursor ();
            success = true;
            break;
        }

    case CONFIG_EXPIRE:
        {
            qApp->setOverrideCursor (arrowCursor);
            ExpireDlg *dlg=new ExpireDlg();
            dlg->exec();
            qApp->restoreOverrideCursor ();
            success = true;
            break;
        }
        
    case EXIT:
        {
            qApp->exit();
            success = true;
            break;
        }
    case CONNECT:
        {
            online();
            break;
        }
    case DISCONNECT:
        {
            offline();
            break;
        }
    case GET_ACTIVE:
        {
            if (needsConnect())
            {
                statusBar ()->changeItem ("Getting active list from server", 2);
                qApp->processEvents ();
                
                server->resetCounters (true,true);
                server->reportCounters (true,false);
                server->groupList (true);
                server->resetCounters (true,true);
                server->reportCounters (false,false);
                loadActive();
            }
            else
            {
                success = false;
            };
            break;
        }
    case SUBSCRIBE:
        {
            if (!group)
                break;
            subscribe (group);
            break;
        }
    case CHECK_UNREAD:
        {
            if (needsConnect())
            {
                printf ("check_unread\n");
                checkUnread(group);
            }
            break;
        }
    case HELP_ABOUT:
        {
            qApp->setOverrideCursor (arrowCursor);
            aboutDlg ab;
            ab.exec();
            qApp->restoreOverrideCursor ();
            success = true;
            break;
        }
    case HELP_CONTENTS:
        {
            kapp->invokeHTMLHelp("","");
            break;
        }
    case CATCHUP:
        {
            if (!group)
                break;
            group->catchup();
            
            success = true;
            break;
        }
    case FIND_GROUP:
        {
            findGroup();
            success = true;
            break;
        }
    case POST:
        {
            if (!group)
                break;

            int mShowHeaders = 0xe0;
            conf->setGroup("Composer");
            conf->writeEntry("headers",mShowHeaders);

            KMMessage *m=new KMMessage();
            m->initHeader();
            m->setGroups(group->name);

            KMComposeWin *comp=new KMComposeWin(m);
            comp->show();
            success=true;
            break;
        }
    };
    
    
    qApp->restoreOverrideCursor ();
    return success;
}

bool Groupdlg::loadSubscribed()
{
    debug ("loading subscribed");
    QString ac;
    ac=krnpath+"subscribed";
    QFile f(ac.data ());
    subscr.clear();
    if (f.open (IO_ReadOnly))
    {
        QTextStream st(&f);
        while (1)
        {
            ac=st.readLine ();
            if (ac.isEmpty())
                break;

            if (!(groupDict.find(ac.data())))
            {
                NewsGroup *n=new NewsGroup(ac.data());
                groupDict.insert(ac.data(),n);
            }
            subscr.append (ac.data());
        };
        f.close ();
        return true;
    }
    else
    {
        warning("Can't open subscribed file for reading!");
        return false;
    }
}

bool Groupdlg::saveSubscribed()
{
    QString ac;
    ac=krnpath+"/subscribed";
    unlink(ac.data());
    QFile f(ac.data ());
    if (f.open (IO_WriteOnly))
    {
        QStrListIterator it(subscr);
        for (;it.current();++it)
        {
            f.writeBlock(it.current(),strlen(it.current()));
            f.writeBlock("\n",1);
        }
        f.close ();
        return true;
    }
    else
    {
        printf ("Can't save subscribed file\n");
        return false;
    }
}

bool Groupdlg::loadActive()
{
    bool success=false;
    QString ac=krnpath+"/active";
    QFile f (ac.data ());
    if (!f.exists())	//doesn't have an active file
    {
        
        qApp->setOverrideCursor (arrowCursor);
        int i = KMsgBox::yesNo (0, klocale->translate("Error"),
                                klocale->translate("You don't have an active groups list.\n get it from server?"));
        if (!needsConnect())
            return false;
        qApp->restoreOverrideCursor ();
        if (1 == i)
        {
	    server->groupList(true);
            success=true;
        }
        else
        {
            return false;
        }
    }
    else			// active file exists
    {
        statusBar ()->changeItem (klocale->translate("Listing active newsgroups"), 2);
        qApp->processEvents ();
        server->groupList (false);
        success=true;
    };
    statusBar ()->changeItem ("", 2);
    activeloaded=true;
    return success;
};

void Groupdlg::findGroup()
{
    if (!activeloaded)
        loadActive();
    Asker ask;
    ask.setCaption (klocale->translate("KRN - Find a Newsgroup"));
    ask.label->setText(klocale->translate("Enter the name of the Newsgroup"));
    ask.entry->setText("");
    qApp->setOverrideCursor (arrowCursor);
    ask.exec();
    qApp->restoreOverrideCursor ();
    if (QString (ask.entry->text()).isEmpty())
        return;
    int index=-1;
    NewsGroup n(ask.entry->text());
    index=subscr.find (n.name);
    if (index!=-1)
    {
        //It exists in subscribed
        list->itemAt(0)->setExpanded(true);
        list->setCurrentItem(index+1);
        return;
    }
    NewsGroup *ng=groupDict.find (n.name);
    if (ng)
    {
        list->setAutoUpdate(false);
        //It exists and not in subscribed (ugh).
        
        KPath p;
        
        p.push (new QString(klocale->translate("All Newsgroups.")));
        
        if (!list->itemAt(p)->isExpanded())
            openGroup(list->itemAt(p));
        
        char *s=qstrdup(ask.entry->text());
        char *s2;
        
        while (1)
        {
            if (!strchr(s,'.'))
            {
                QString *ss;
                QString *s1=p.pop();
                ss=new QString(qstrdup(s1->data()));
                p.push(s1);
                ss->append(s);
                p.push (ss);
		KTreeViewItem* it = list->itemAt(p);
                list->setCurrentItem(list->itemRow(it));
                list->scrollVisible(it,false);
                break;
            }
            else
            {
                s2=strchr(s,'.')+1;
                *strchr(s,'.')=0;
                QString *ss;
                if (p.count()>1)
                {
                    QString *s1=p.pop();
                    ss=new QString(qstrdup(s1->data()));
                    p.push(s1);
                }
                else
                {
                    ss=new QString("");
                }
                ss->append(s);
                ss->append(".");
                p.push(ss);
                KTreeViewItem *it=list->itemAt(p);
                if (!it)
                {
                    debug ("no fscking item!!!!");
                    break;
                }
                if (!it->isExpanded())
                {
                    openGroup(it);
                }
                s=s2;
            }
        }
        list->repaint();
        list->setAutoUpdate(true);
        return;
    }
    qApp->setOverrideCursor (arrowCursor);
    KMsgBox::message(0,klocale->translate("Krn - Message"),
                     klocale->translate("I can't find the group you want!"));
    qApp->restoreOverrideCursor ();
}

bool Groupdlg::postQueued()
{
    debug ("entered postqueued");
    bool success=false;
    if (needsConnect())
    {
        debug ("outpath--%s",outpath.data());
        debug ("have connected");
        QDir d(outpath.data());
        d.setFilter(QDir::Files);
        QStrList *files=new QStrList (*d.entryList("*"));

        debug ("%d files waiting",files->count());
        for (char *fname=files->first();fname!=0;fname=files->next())
        {
            debug ("Sending %s",fname);
            updateCounter(QString("Sending ")+fname);
            if (!msgSender->sendQueued(fname))
            {
                success=false;
                break;
            }
        }
        success=true;
    }
    return success;
}

bool Groupdlg::currentActions(int action)
{
    bool success=true;
    //Lets try to find the newsgroup
    if (action==OPENGROUP)
    {
        openGroup(list->currentItem());
        return success;
    }
    else if (action==POST_QUEUED)
    {
        bool success=postQueued();
        return success;
    }
    else
    {
        NewsGroup *g=0;
        if (list->getCurrentItem())
        {
            QString temp = list->getCurrentItem ()->getText ();
            int i=temp.find(' ');
            if (i>0)
            {
                temp=temp.left(i);;
            };
            char *text=temp.data();
            g=groupDict.find(text);
        }
        actions(action,g);
    }
    return success;
}

bool Groupdlg::listActions(int action ,QStrList _list)
{
    bool success=true;
    QStrListIterator it(_list);
    for (;it.current();++it)
    {
        actions(action,groupDict.find(it.current()));
    }
    statusBar ()->changeItem ("Done.", 2);
    return success;
}


bool Groupdlg::taggedActions(int action)
{
    return listActions (action,tagged);
}

bool Groupdlg::subscrActions(int action)
{
    return listActions (action,subscr);
}

void Groupdlg::getArticles(NewsGroup *group)
{
    if (needsConnect())
    {
        QString s;
        s=klocale->translate("Getting messages in ");
        s+=group->name;
        statusBar ()->changeItem (s.data(), 2);
        qApp->processEvents();
        group->getMessages(server);
    }
}

void Groupdlg::getSubjects(NewsGroup *group)
{
    if (needsConnect())
    {
        QString s;
        s=klocale->translate("Getting list of messages in ");
        s+=group->name;
        statusBar ()->changeItem (s.data(), 2);
        qApp->processEvents();
        group->getSubjects(server);
    }
}

void Groupdlg::checkUnread(NewsGroup *group)
{
    QString l,status,howmany,first,last,gname;
    KPath p;
    KTreeViewItem *base=list->itemAt(0); //The "Subscribed Newsgroups" item.
    p.push(new QString(base->getText()));
    char countmessage[255];
    
    int c=base->childCount();
    for (int i=0;i<c;i++)
    {
        const char *it=base->childAt(i)->getText();
        gname=it;
        p.push(new QString(it));
        gname=gname.left(gname.find(' '));
        sprintf(countmessage, " [%d]", groupDict.find(subscr.at(i))->countNew(server));
        
        l=gname+countmessage;
        // Updating the test this way doesn't repaint properly,
        // so I have to do do the path hack.
        //        base->childAt(i)->setText(l.data());
        list->changeItem(l.data(),0,p);
        delete p.pop();
    }
    list->repaint();
}


void Groupdlg::updateCounter(const char *s)
{
    statusBar()->changeItem (s, 1);
    qApp->processEvents();
}


void Groupdlg::lostConnection()
{
    debug ("lost connection!");
    offline();
    qApp->setOverrideCursor (arrowCursor);
    int i=KMsgBox::yesNo(0,"KRN - Error",
                         "Lost the connection to the server\n"
                         "Want to reconnect?\n");
    if (i==1)
        online();
    qApp->restoreOverrideCursor ();
}

