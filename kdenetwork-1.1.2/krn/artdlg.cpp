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
#include "artdlg.h"


#define Inherited KTopLevelWidget


#include <kapp.h>
#include <qfile.h>
#include <qstring.h>
#include <qtstream.h>
#include <qclipbrd.h>
#include <qregexp.h>
#include <qmsgbox.h>

#include <kapp.h>
#include <kfm.h>
#include <kspinbox.h>

#include <kmsgbox.h>
#include <html.h>
#include <kconfig.h>
#include <kprocess.h>

#include <mimelib/mimepp.h>

#include "groupdlg.h"
#include "kdecode.h"
#include "rmbpop.h"
#include "fontsDlg.h"
#include "findArtDlg.h"
#include "rulesDlg.h"
#include "sortDlg.h"
#include "rules.h"

#include "kmcomposewin.h"
#include "kmreaderwin.h"
#include "kfileio.h"


#include <kfiledialog.h>


#include "artdlg.moc"

#define REP_MAIL 1
#define FOLLOWUP 2
#define PREV 3
#define NEXT 4
#define ARTLIST 5
#define PAGE_DOWN_ARTICLE 6
#define PAGE_UP_ARTICLE 7
#define DECODE_ARTICLE 8
#define TAG_ARTICLE 9
#define SAVE_ARTICLE 10
#define SCROLL_UP_ARTICLE 11
#define SCROLL_DOWN_ARTICLE 12
#define DECODE 13
#define NO_READ 14
#define PRINT_ARTICLE 15
#define CONFIG_FONTS 16
#define CATCHUP 17
#define TOGGLE_EXPIRE 18  // robert's cache stuff
#define NO_LOCKED 19
#define POSTANDMAIL 20
#define FORWARD 21
#define POST 22
#define FIND_ARTICLE 23
#define EXPUNGE 24
#define DOWNLOAD_ARTICLE 25
#define NO_CACHED 26
#define EDIT_RULES 27
#define UPDATE_SCORES 28
#define CONFIG_SORTING 29
#define MARK_READ 30
#define MARK_UNREAD 31
#define LOOKUP_ALTAVISTA 32
#define QUIT 33
#define SCOREFRAME 34
#define FILL_TREE 35
#define CLOSE_WINDOW 36
#define MULTI_SAVE 37

extern QString pixpath,cachepath;

extern KConfig *conf;

extern Groupdlg *main_widget;

extern QList <Rule> ruleList;

QString MultiSavePath;

QStrList artsToDecode;

findArtDlg *FindDlg=0;
rulesDlg *RulesDlg=0;
sortDlg *SortDlg=0;

void decode()
{
    debug ("entered decode()");
    KProcess *proc=new KProcess();
    *proc << "kdecode";
    for (char *art=artsToDecode.first();art!=0;art=artsToDecode.next())
    {
         *proc << art;
    }
    proc->start();
    artsToDecode.clear();
}

Artdlg::Artdlg (NewsGroup *_group, NNTP* _server)
    :Inherited (_group->name)
{
    IDList.setAutoDelete(true);
    depths.setAutoDelete(true);
    group=0;
    server=0;
    
    conf->setGroup("Sorting");
    threaded=conf->readNumEntry("Threaded",true);
    key1=conf->readNumEntry("key1",KEY_SUBJECT);
    key2=conf->readNumEntry("key2",KEY_SCORE);
    key3=conf->readNumEntry("key3",KEY_DATE);
    key4=conf->readNumEntry("key4",KEY_SENDER);

    conf->setGroup("ArticleListOptions");
    unread=conf->readNumEntry("ShowOnlyUnread",1);
    showlocked=conf->readNumEntry("ShowLockedArticles");
    showcached=conf->readNumEntry("ShowCachedArticles");

    taggedArticle=new QPopupMenu;
    taggedArticle->insertItem(klocale->translate("Save"),SAVE_ARTICLE);
    taggedArticle->insertItem(klocale->translate("Download"),DOWNLOAD_ARTICLE);
    taggedArticle->insertSeparator();
    taggedArticle->insertItem(klocale->translate("Mark Read"),MARK_READ);
    taggedArticle->insertItem(klocale->translate("Mark Unread"),MARK_UNREAD);
    taggedArticle->insertSeparator();
    taggedArticle->insertItem(klocale->translate("Print"),PRINT_ARTICLE);
    taggedArticle->insertItem(klocale->translate("Decode"),DECODE_ARTICLE);
    taggedArticle->insertItem(klocale->translate("Untag"),TAG_ARTICLE);
    taggedArticle->insertItem(klocale->translate("Don't expire"), TOGGLE_EXPIRE);  // robert's cache stuff
    connect (taggedArticle,SIGNAL(activated(int)),SLOT(taggedActions(int)));
    
    
    article=new QPopupMenu;
    article->setCheckable(true);
    article->insertItem(klocale->translate("Save"),SAVE_ARTICLE);
    article->insertItem(klocale->translate("Download"),DOWNLOAD_ARTICLE);
    article->insertItem(klocale->translate("Find"),FIND_ARTICLE);
    article->insertItem(klocale->translate("Lookup in Altavista"),LOOKUP_ALTAVISTA);
    article->insertSeparator();
    article->insertItem(klocale->translate("Print"),PRINT_ARTICLE);
    article->insertItem(klocale->translate("Post New Article"),POST);
    article->insertItem(klocale->translate("Reply by Mail"),REP_MAIL);
    article->insertItem(klocale->translate("Post Followup"),FOLLOWUP);
    article->insertItem(klocale->translate("Post & Reply"),POSTANDMAIL);
    article->insertItem(klocale->translate("Forward"),FORWARD);
    article->insertSeparator();
    article->insertItem(klocale->translate("Decode"),DECODE);
    article->insertItem(klocale->translate("(Un)Tag"),TAG_ARTICLE);
    article->insertItem(klocale->translate("Don't expire"), TOGGLE_EXPIRE);  // robert's cache stuff
    article->insertSeparator();
    article->insertItem(klocale->translate("Tagged"),taggedArticle);
    article->insertSeparator(); // robert
    article->setItemChecked(TOGGLE_EXPIRE, false);
    article->insertItem(klocale->translate("Close"), CLOSE_WINDOW);
    article->insertItem(klocale->translate("Quit"), QUIT);
    connect (article,SIGNAL(activated(int)),SLOT(defaultActions(int)));
    
    
    
    options=new QPopupMenu;
    options->setCheckable(true);
    options->insertItem(klocale->translate("Show Only Unread Messages"), NO_READ);
    options->setItemChecked(NO_READ,unread);
    options->insertItem(klocale->translate("Show Only Cached Messages"), NO_CACHED);
    options->setItemChecked(NO_CACHED,showcached);
    options->insertItem(klocale->translate("Show Locked Messages"), NO_LOCKED);
    options->setItemChecked(NO_LOCKED,showlocked);
    options->insertItem(klocale->translate("Expunge"), EXPUNGE);
    options->insertItem(klocale->translate("Appearance..."),CONFIG_FONTS);
    options->insertItem(klocale->translate("Sorting..."),CONFIG_SORTING);
    connect (options,SIGNAL(activated(int)),SLOT(defaultActions(int)));

    QPopupMenu *scoring=new QPopupMenu;
    scoring->insertItem(klocale->translate("Edit Rules"),EDIT_RULES);
    scoring->insertItem(klocale->translate("Update"),UPDATE_SCORES);
    connect (scoring,SIGNAL(activated(int)),SLOT(defaultActions(int)));

    menu=new KMenuBar (this);
    setMenu(menu);
    menu = menuBar();
    menu->insertItem (klocale->translate("&File"), article);
    menu->insertItem (klocale->translate("&Tagged"), taggedArticle);
    menu->insertItem (klocale->translate("&Options"), options);
    menu->insertItem (klocale->translate("&Scoring"), scoring);

    KToolBar *t1=new KToolBar(this);
    addToolBar(t1,0);
    QObject::connect (t1, SIGNAL (clicked (int)), this, SLOT (defaultActions (int)));

    
    t1->insertButton (Icon("left.xpm"), PREV, true, klocale->translate("Previous Message"));
    
    t1->insertButton (Icon("right.xpm"), NEXT, true, klocale->translate("Next Message"));

    t1->insertButton(Icon("find.xpm"),FIND_ARTICLE,true,klocale->translate("Find Article"));
    t1->insertSeparator ();

    t1->insertButton (Icon("filenew.xpm"), POST, true, klocale->translate("Post New Article"));

    t1->insertButton (Icon("previous.xpm"), ARTLIST, true, klocale->translate("Get Article List"));

    t1->insertButton (Icon("reload.xpm"), FILL_TREE, true, klocale->translate("Refresh Display"));
    t1->alignItemRight(FILL_TREE);
    t1->insertFrame(SCOREFRAME,100);
    t1->alignItemRight(SCOREFRAME);
    minScore= new KNumericSpinBox(t1->getFrame(SCOREFRAME));
    minScore->show();
    minScore->setRange(-25000,25000);
    minScore->setStep(100);
    minScore->setValue(0);
    minScore->setFixedHeight(t1->getFrame(SCOREFRAME)->height()-2);

    KToolBar *t2=new KToolBar(this);
    addToolBar(t2,1);
    QObject::connect (t2, SIGNAL (clicked (int)), this, SLOT (defaultActions (int)));
    
    QStrList *comboContents=new QStrList();
    comboContents->append(klocale->translate("Current"));
    comboContents->append(klocale->translate("Tagged"));
    comboContents->append(klocale->translate("All"));
    comboContents->append(klocale->translate("Read"));
    comboContents->append(klocale->translate("UnRead"));
    t2->insertCombo(comboContents,1,false,SIGNAL(activated(int)),this,SLOT(setTarget(int)));
    
    t2->insertButton(Icon("save.xpm"),SAVE_ARTICLE,true,klocale->translate("Save Article"));
    
    t2->insertButton(Icon("fileprint.xpm"),PRINT_ARTICLE,true,klocale->translate("Print Article"));

    t2->insertButton (Icon("filemail.xpm"), REP_MAIL, true, klocale->translate("Reply by Mail"));
    
    t2->insertButton (Icon("followup.xpm"), FOLLOWUP, true, klocale->translate("Post a Followup"));
    
    t2->insertButton (Icon("mailpost.xpm"), POSTANDMAIL, true, klocale->translate("Post & Mail"));
    
    t2->insertButton (Icon("fileforward.xpm"), FORWARD, true, klocale->translate("Forward"));
    
    t2->insertSeparator ();
    
    t2->insertButton (Icon("tagged.xpm"), TAG_ARTICLE, true, klocale->translate("Tag Article"));
    
    t2->insertButton (Icon("locked.xpm"), TOGGLE_EXPIRE, true, klocale->translate("Lock (keep in cache)"));
    
    t2->insertButton (Icon("deco.xpm"), DECODE_ARTICLE, true, klocale->translate("Decode Article"));
    
    t2->insertButton (Icon("red-bullet.xpm"), MARK_READ, true, klocale->translate("Mark Read"));

    t2->insertButton (Icon("green-bullet.xpm"), MARK_UNREAD, true, klocale->translate("Mark UnRead"));
    
    if (conf->readNumEntry("VerticalSplit",false))
    {
        debug ("vertical");
        panner=new KPanner (this,"panner",KPanner::O_VERTICAL,33);
    }
    else
    {
        debug ("horizontal");
        panner=new KPanner (this,"panner",KPanner::O_HORIZONTAL,33);
    }
    panner->setSeparator(50);
    setView (panner);
    
    gl = new QVBoxLayout( panner->child0() );
    list=new KTabListBox (panner->child0(),"",5);
    list->clearTableFlags(Tbl_hScrollBar);
    list->clearTableFlags(Tbl_autoHScrollBar);
    list->setTableFlags(Tbl_autoVScrollBar);
    list->setSeparator('\n');
    
    list->setColumn(0, klocale->translate("Sender"), 150);
    list->setColumn(1, klocale->translate("Date"), 75);
    list->setColumn(2, klocale->translate("Lines"), 50);
    list->setColumn(3, klocale->translate("Score"), 50);
    list->setColumn(4, klocale->translate("Subject"), 1000,KTabListBox::MixedColumn);
    
    list->dict().insert("N",new QPixmap(kapp->getIconLoader()->loadIcon("green-bullet.xpm")));  //Unread message
    list->dict().insert("R",new QPixmap(kapp->getIconLoader()->loadIcon("red-bullet.xpm")));    //Read message
    list->dict().insert("T",new QPixmap(kapp->getIconLoader()->loadIcon("black-bullet.xpm")));    //Read message
    list->dict().insert("M",new QPixmap(kapp->getIconLoader()->loadIcon("tagged.xpm")));    //Read message
    list->dict().insert("L",new QPixmap(kapp->getIconLoader()->loadIcon("locked.xpm")));    //Read message
    
    list->setTabWidth(25);

    gl->addWidget( list, 1);
    connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    connect (list,SIGNAL(midClick(int,int)),this,SLOT(markReadArt(int,int)));
    connect (list,SIGNAL(popupMenu(int,int)),this,SLOT(popupMenu(int,int)));
    connect (list,SIGNAL(headerClicked(int)),this,SLOT(sortHeaders(int)));
    
    RmbPop *filter=new RmbPop(list);
    delete (filter->pop);
    filter->pop=article;
    
    gl = new QVBoxLayout( panner->child1());
    messwin=new KMReaderWin(panner->child1(),"messwin");
    messwin->setMsg(0);
    gl->addWidget( messwin, 1);
    QObject::connect(messwin,SIGNAL(urlClicked(const char *,int)),this,SLOT(openURL(const char*)));
    
    RmbPop *filter2=new RmbPop(messwin);
    delete (filter2->pop);
    filter2->pop=article;
    

    status=new KStatusBar(this);
    setStatusBar(status);
    status->insertItem ("                 ", 1);
    status->insertItem ("", 2);
    
    acc=new QAccel (this);
    acc->insertItem(Key_N,NEXT);
    acc->insertItem(Key_P,PREV);
    acc->insertItem(Key_T,TAG_ARTICLE);
    acc->insertItem(Key_R,REP_MAIL);
    acc->insertItem(Key_F,FOLLOWUP);
    acc->insertItem(Key_G,ARTLIST);
    acc->insertItem(Key_C,CATCHUP);
    acc->insertItem(Key_Space,PAGE_DOWN_ARTICLE);
    acc->insertItem(Key_Next,PAGE_DOWN_ARTICLE);
    acc->insertItem(Key_Backspace,PAGE_UP_ARTICLE);
    acc->insertItem(Key_Prior,PAGE_UP_ARTICLE);
    acc->insertItem(ALT + Key_Up, PREV);
    acc->insertItem(ALT + Key_Down, NEXT);
    acc->insertItem(Key_Up, SCROLL_UP_ARTICLE);
    acc->insertItem(Key_Down, SCROLL_DOWN_ARTICLE);
    acc->insertItem(Key_X, EXPUNGE);
    acc->insertItem(Key_Slash, FIND_ARTICLE);
    acc->insertItem(Key_S, FIND_ARTICLE);
    acc->insertItem(CTRL+Key_F, FIND_ARTICLE);
    
    QObject::connect (acc,SIGNAL(activated(int)),this,SLOT(defaultActions(int)));
    QObject::connect (messwin,SIGNAL(statusMsg(const char*)),this,SLOT(updateCounter(const char*)));
    conf->setGroup("Geometry");
    setGeometry(conf->readNumEntry("ArtX",100),
                conf->readNumEntry("ArtY",40),
                conf->readNumEntry("ArtW",400),
                conf->readNumEntry("ArtH",400));
    show();
    
    qApp->processEvents ();
    
    init(_group,_server);
}


void Artdlg::init (NewsGroup *_group, NNTP* _server)
{
    IDList.clear();
    if (group) 
    {
        if (group->dirty)
            group->clean();
        group->isVisible=0; //make old group know I'm not showing him
    }
    
    group=_group;
    group->isVisible=this;
    
    setCaption (group->name);
    groupname=group->name;
    statusBar()->changeItem("Reading Article List",2);
    qApp->processEvents ();
    group->getList(this);
    
    if (server)
    {
        disconnect (server,SIGNAL(newStatus(const char *)),
                    this,SLOT(updateCounter(const char *)));
    }
    
    server = _server;
    QObject::connect (server,SIGNAL(newStatus(const char *)),
                      this,SLOT(updateCounter(const char *)));

    list->clear();
    list->setCurrentItem(0);
    
    if (server->isConnected())
    {
        actions(ARTLIST);
    }
    else
    {
        fillTree();
    }
    list->setTopItem(0);
}

void Artdlg::copyText(bool)
{
}

Artdlg::~Artdlg ()
{
    group->artList.clear();
    IDList.clear();
    group->isVisible=0;
    conf->setGroup("Geometry");
    conf->writeEntry("ArtX",x());
    conf->writeEntry("ArtY",y());
    conf->writeEntry("ArtW",width());
    conf->writeEntry("ArtH",height());
    conf->sync();
}

void Artdlg::fillTree ()
{
    group->getList();
    ArticleList artList;

    int minCoolness=minScore->getValue();
    
    //save current ID if there is one
    char *currArt=0;
    int curr=list->currentItem();
    if (curr>-1)
        currArt=IDList.at(curr);
    
    qApp->setOverrideCursor(waitCursor);
    statusBar()->changeItem("Reading Article List",2);
    qApp->processEvents ();
    
    list->setAutoUpdate(false);
    list->clear();
    
    Article *iter;
    bool thiscached;
    for (iter=group->artList.first();iter!=0;iter=group->artList.next())
    {
        thiscached=server->isCached(iter->ID.data())==PART_ALL;
        if(
           (((thiscached && showcached) || (!showcached)) &&
            (!(unread && iter->isread))) ||
           (showlocked && (!iter->canExpire())))
        {
            artList.append(iter);
        }
    }
    
    statusBar()->changeItem(klocale->translate("Threading..."),2);
    qApp->processEvents ();
    artList.thread(threaded,key1,key2,key3,key4);
    
    //had to split this in two loops because the order of articles is not
    //the same in both article lists
    
    IDList.clear();
    depths.clear();
    statusBar()->changeItem(klocale->translate("Showing Article List"),2);
    qApp->processEvents ();
    int i=0;
    for (iter=artList.first();iter!=0;iter=artList.next(),i++)
    {
        if (iter->score()<minCoolness)
            continue;
        QString formatted;
        iter->formHeader(&formatted);
        list->insertItem (formatted.data());
        if (server->isCached(iter->ID.data())==PART_ALL)
            list->changeItemColor(QColor(0,0,255),i);
        IDList.append(iter->ID.data());
        depths.append(new int(iter->threadDepth));
    }

    // For some reason this wasn't working
    /*
    //restore current message
    if (curr>-1)
    {
        int i=0;
        for (iter=artList.first();iter!=0;iter=artList.next(),i++)
        {
            if (iter->ID==currArt)
                break;
        }
        list->setCurrentItem(i);
        list->setTopItem(i);
        }
        */
    
    list->setAutoUpdate(true);
    list->repaint();
    qApp->restoreOverrideCursor();
    statusBar()->changeItem("",2);
    QString s;
    s.sprintf ("%d/%d",list->currentItem()+1,IDList.count());
    statusBar()->changeItem(s.data(),1);
    qApp->processEvents ();
    
    artList.clear();
    group->artList.clear();
}

bool Artdlg::taggedActions (int action)
{
    if (action==SAVE_ARTICLE)
    {
        action=MULTI_SAVE;
        MultiSavePath="";
    }
    bool success=false;
    qApp->setOverrideCursor (waitCursor);
    list->setUpdatesEnabled(FALSE);
    int c=0;
    
    if (action!=PRINT_ARTICLE)
    {
        disconnect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    for (char *iter=IDList.first();iter!=0;iter=IDList.next())
    {
        if (Article(iter).isMarked())
        {
            success=actions(action,c);
        }
        if (!(c%5))
            qApp->processEvents();
        c++;
    }
    
    if (action!=PRINT_ARTICLE)
    {
        connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    qApp->restoreOverrideCursor ();
    list->setUpdatesEnabled(TRUE);
    list->update();
    if (action==DECODE_ARTICLE)
    {
        decode();
    }
    return success;
}

bool Artdlg::readActions (int action)
{
    if (action==SAVE_ARTICLE)
    {
        action=MULTI_SAVE;
        MultiSavePath="";
    }
    bool success=false;
    qApp->setOverrideCursor (waitCursor);
    list->setUpdatesEnabled(FALSE);
    int c=0;
    
    if (action!=PRINT_ARTICLE)
    {
        disconnect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    for (char *iter=IDList.first();iter!=0;iter=IDList.next())
    {
        if (Article::isRead(iter))
        {
            success=actions(action,c);
        }
        c++;
        if (!(c%5))
            qApp->processEvents();
    }

    if (action!=PRINT_ARTICLE)
    {
        connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    qApp->restoreOverrideCursor ();
    list->setUpdatesEnabled(TRUE);
    list->update();
    if (action==DECODE_ARTICLE)
    {
        decode();
    }
    return success;
}

bool Artdlg::unreadActions (int action)
{
    if (action==SAVE_ARTICLE)
    {
        action=MULTI_SAVE;
        MultiSavePath="";
    }
    bool success=false;
    qApp->setOverrideCursor (waitCursor);
    list->setUpdatesEnabled(FALSE);
    int c=0;
    
    if (action!=PRINT_ARTICLE)
    {
        disconnect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    for (char *iter=IDList.first();iter!=0;iter=IDList.next())
    {
        if (!(Article::isRead(iter)))
        {
            success=actions(action,c);
        }
        c++;
        if (!(c%5))
            qApp->processEvents();
    }
    
    if (action!=PRINT_ARTICLE)
    {
        connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    qApp->restoreOverrideCursor ();
    list->setUpdatesEnabled(TRUE);
    list->update();
    if (action==DECODE_ARTICLE)
    {
        decode();
    }
    return success;
}

bool Artdlg::allActions (int action)
{
    if (action==SAVE_ARTICLE)
    {
        action=MULTI_SAVE;
        MultiSavePath="";
    }
    bool success=false;
    qApp->setOverrideCursor (waitCursor);
    list->setUpdatesEnabled(FALSE);
    int c=0;
    
    if (action!=PRINT_ARTICLE)
    {
        disconnect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    for (char *iter=IDList.first();iter!=0;iter=IDList.next())
    {
        success=actions(action,c);
        c++;
        if (!(c%5))
            qApp->processEvents();
    }
    
    if (action!=PRINT_ARTICLE)
    {
        connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    }
    
    qApp->restoreOverrideCursor ();
    list->setUpdatesEnabled(TRUE);
    list->update();
    if (action==DECODE_ARTICLE)
    {
        decode();
    }
    return success;
}

void Artdlg::updateScores()
{
    Article art;
    int i=0;
    QString s;
    Rule::updateGlobals(); 
    for (char *ID=IDList.first();ID!=0;ID=IDList.next(),i++)
    {
        art.ID=ID;
        art.load();
        art.reScore(ruleList);
        if (!(i%10))
        {
            s.sprintf ("Calculating score for article %d",i);
            statusBar()->changeItem(s.data(),2);
            qApp->processEvents();
        }
    }
}

bool Artdlg::defaultActions(int action)
{
    return actions(action);
}


bool Artdlg::actions (int action,int index)
{
    setEnabled (false);
    acc->setEnabled(false);
    list->setEnabled(false);
    messwin->setEnabled(false);
    bool success=false;
    qApp->setOverrideCursor (waitCursor);
    if (index==-1)
        index=list->currentItem();
    switch (action)
    {
    case FILL_TREE:
        {
            fillTree();
            break;
        }
    case UPDATE_SCORES:
        {
            updateScores();
            fillTree();
            break;
        }
    case CLOSE_WINDOW:
        {
            this->close();
            break;
        }
    case QUIT:
        {
            main_widget->close();
            break;
        }
    case LOOKUP_ALTAVISTA:
        {
            Article art(IDList.at(index));
            art.lookupAltavista();
            break;
        }
    case MARK_READ:
        {
            Article art(IDList.at(index));
            art.threadDepth=*depths.at(index);
            art.setRead(true);
            QString formatted;
            art.formHeader(&formatted);
            list->changeItem (formatted.data(),index);
            if (server->isCached(art.ID.data())==PART_ALL)
                list->changeItemColor(QColor(0,0,255),index);
            break;
        }
    case MARK_UNREAD:
        {
            Article art(IDList.at(index));
            art.threadDepth=*depths.at(index);
            art.setRead(false);
            QString formatted;
            art.formHeader(&formatted);
            list->changeItem (formatted.data(),index);
            if (server->isCached(art.ID.data())==PART_ALL)
                list->changeItemColor(QColor(0,0,255),index);
            break;
        }
    case EDIT_RULES:
        {
            if (!RulesDlg)
                RulesDlg=new rulesDlg();
            qApp->setOverrideCursor (arrowCursor);
            if (RulesDlg->exec()==1)
            {
            }
            qApp->restoreOverrideCursor ();
            Rule::updateGlobals();
            break;
        }
    case CONFIG_SORTING:
        {
            if (!SortDlg)
                SortDlg=new sortDlg();
            qApp->setOverrideCursor (arrowCursor);
            if (SortDlg->exec()==1)
            {
                conf->setGroup("Sorting");
                threaded=conf->readNumEntry("Threaded",true);
                key1=conf->readNumEntry("key1",KEY_SUBJECT);
                key2=conf->readNumEntry("key2",KEY_SCORE);
                key3=conf->readNumEntry("key3",KEY_DATE);
                key4=conf->readNumEntry("key4",KEY_SENDER);
                fillTree();
            }
            qApp->restoreOverrideCursor ();
            break;
        }
    case CONFIG_FONTS:
        {
            qApp->setOverrideCursor (arrowCursor);
            fontsDlg dlg;
            if(dlg.exec()==1)
            {
                messwin->readConfig();
            }
            qApp->restoreOverrideCursor ();
            break;
        }
    case PRINT_ARTICLE:
        {
            conf->setGroup("Printing");
            if (conf->readNumEntry("HTMLPrinting",true))
            {
                qApp->setOverrideCursor (arrowCursor);
                messwin->printMsg();
                qApp->restoreOverrideCursor ();
            }
            else
            {
                QString cmd=qstrdup(conf->readEntry("CommandName","lpr"));
                cmd+=" \"";
                cmd+=cachepath;
                cmd+="/";
                cmd.replace(QRegExp("//"),"/");
                cmd+=IDList.at(index);
                cmd+="\"";
                debug ("print command: %s",cmd.data());
                QString s;
                s.sprintf("Printing %s",IDList.at(index));
                qApp->processEvents ();
                statusBar()->changeItem(s.data(),2);
                system(cmd.data());
            }
            break;
        }
    case ARTLIST:
        {
            emit needConnection();
            if (server->isConnected())
            {
                getSubjects();
                fillTree();
                success=true;
            }
            break;
        }
    case PREV:
        {
            if (index>0)
            {
                index--;
                list->setCurrentItem(index);
            }
            success=true;
            break;
        }
    case NEXT:
        {
            if (index<list->count())
            {
                index++;
                list->setCurrentItem(index);
            }
            success=true;
            break;
        }
    case DECODE_ARTICLE:
        {
            decArt(index,0);
            break;
        }
    case DECODE:
        {
            decode();
            break;
        }
    case TAG_ARTICLE:
        {
            markArt(index,0);
            break;
        }
    case SAVE_ARTICLE:
        {
            if (index<0)
                break;
            saveArt(IDList.at(index));
            break;
        }
    case MULTI_SAVE:
        {
            if (index<0)
                break;
            multiSaveArt(IDList.at(index));
            break;
        }

    case NO_READ:
        {
            unread = !unread;
            conf->setGroup("ArticleListOptions");
            conf->writeEntry("ShowOnlyUnread",unread);
            conf->sync();
            options->setItemChecked(NO_READ, unread);
            if (unread==false)
                group->getList(this);
            fillTree();
            success = true;
            break;
        }
    case NO_CACHED:
        {
            showcached = !showcached;
            conf->setGroup("ArticleListOptions");
            conf->writeEntry("ShowCachedArticles",showcached);
            conf->sync();
            options->setItemChecked(NO_CACHED, showcached);
            fillTree();
            success = true;
            break;
        }
    case NO_LOCKED:
        {
            showlocked = !showlocked;
            conf->setGroup("ArticleListOptions");
            conf->writeEntry("ShowLockedArticles",showlocked);
            conf->sync();
            options->setItemChecked(NO_LOCKED, showlocked);
            fillTree();
            success = true;
            break;
        }
    case PAGE_UP_ARTICLE:
        {
            messwin->slotScrollPrior();
            break;
        }
    case SCROLL_UP_ARTICLE:
        {
            messwin->slotScrollUp();
            break;
        }
    case PAGE_DOWN_ARTICLE:
        {
            messwin->slotScrollNext();
            break;
        }
    case SCROLL_DOWN_ARTICLE:
        
        {
            messwin->slotScrollDown();
            break;
        }
    case POST:
        {
            int mShowHeaders = 0xe0;
            conf->setGroup("Composer");
            conf->writeEntry("headers",mShowHeaders);
            
            KMMessage *m=new KMMessage();
            m->initHeader();
            m->setGroups(groupname);
            
            KMComposeWin *comp=new KMComposeWin(m);
            comp->show();
            break;
        }
    case FOLLOWUP:
        {
            if(index < 0)
                break;
            
            int mShowHeaders = 0xe0;
            conf->setGroup("Composer");
            conf->writeEntry("headers",mShowHeaders);
            
            Article *art=new Article(IDList.at(index));
            KMMessage *mm=new KMMessage();
            QString *ts=server->article(art->ID.data());
            mm->fromString(ts->data());
            delete ts;
            KMMessage *m=mm->createReply(true);
            //            m->initHeader();
            debug ("id1-->%s<--id2-->%s<--",
                   m->id().data(),
                   mm->id().data());
            QString refs=mm->references();
            refs+=" ";
            refs+=mm->id();
            m->setReferences(refs);
            m->setGroups(mm->groups());
            m->setTo("");
            delete mm;
            
            KMComposeWin *comp=new KMComposeWin(m);
            comp->show();
            break;
        }
    case REP_MAIL:
        {
            if(index < 0)
                break;
            
            int mShowHeaders  = 0x2c;
            conf->setGroup("Composer");
            conf->writeEntry("headers",mShowHeaders);
            
            Article *art=new Article(IDList.at(index));
            KMMessage *m=new KMMessage();
            QString *ts=server->article(art->ID.data());
            m->fromString(ts->data());
            delete ts;
            KMMessage *mm=m->createReply();
            m->initHeader();
            mm->setGroups("");
            
            KMComposeWin *comp=new KMComposeWin(mm);
            comp->show();
            break;
        }
    case FORWARD:
        {
            if(index < 0)
                break;
            
            int mShowHeaders = 0x2c;
            conf->setGroup("Composer");
            conf->writeEntry("headers",mShowHeaders);
            
            Article *art=new Article(IDList.at(index));
            KMMessage *m=new KMMessage();
            QString *ts=server->article(art->ID.data());
            m->fromString(ts->data());
            delete ts;
            
            KMMessage *mm=m->createForward();
            m->initHeader();
            mm->setGroups("");
            
            KMComposeWin *comp=new KMComposeWin(mm);
            comp->show();
            break;
        }
    case POSTANDMAIL:
        {
            if(index < 0)
                break;
            Article *art=new Article(IDList.at(index));
            
            int mShowHeaders=0x6c;
            conf->setGroup("Composer");
            conf->writeEntry("headers",mShowHeaders);
            
            KMMessage *mm=new KMMessage();
            QString *ts=server->article(art->ID.data());
            mm->fromString(ts->data());
            delete ts;
            KMMessage *m=mm->createReply(true);
            QString refs=mm->references();
            refs+=" ";
            refs+=mm->id();
            m->setReferences(refs);
            m->setGroups(mm->groups());
            delete mm;
            
            KMComposeWin *comp=new KMComposeWin(m);
            comp->show();
            break;
            
        }
        
    case CATCHUP:
        {
            QStrListIterator iter(IDList);
            for (;iter.current();++iter)
            {
                Article art(iter.current());
                if (!art.isread)
                {
                    art.setRead();
                }
            }
            fillTree();
            break;
        }     
    case EXPUNGE:
        {
            unread=false;
            actions(NO_READ);
            break;
        }     
        
        //
        // robert's cache stuff
        
    case TOGGLE_EXPIRE:
        {
            if(index < 0)
                break;
            
            Article art(IDList.at(index));
            art.threadDepth=*depths.at(index);
            
            if(art.canExpire()) {
                article->setItemChecked(TOGGLE_EXPIRE, true);
                art.toggleExpire();
            } else {
                article->setItemChecked(TOGGLE_EXPIRE, false);
                art.toggleExpire();
            }
            QString formatted;
            art.formHeader(&formatted);
            list->changeItem (formatted.data(),index);
            if (server->isCached(art.ID.data())==PART_ALL)
                list->changeItemColor(QColor(0,0,255),index);
            
            break;
        }
    case FIND_ARTICLE:
        {
            if (!FindDlg)
            {
                FindDlg=new findArtDlg(0);
                disconnect(FindDlg);
                connect (FindDlg,SIGNAL(FindThis(const char *,const char*,bool,bool)),
                         this,SLOT(FindThis(const char *,const char*,bool,bool)));
            }
            FindDlg->show();
            break;
        }
    case DOWNLOAD_ARTICLE:
        {
            if(index < 0)
                break;
            QString id=IDList.at(index);
            if (!server->isConnected())
            {
                if (!(server->isCached(id.data())==PART_ALL))
                {
                    emit needConnection();
                    if (!server->isConnected())
                    {
                        qApp->restoreOverrideCursor ();
                        return false;
                    }
                }
            }
            server->article(id.data());
            list->changeItemColor(QColor(0,0,255),index);
        }
    }
    qApp->restoreOverrideCursor ();
    setEnabled (true);
    acc->setEnabled(true);
    list->setEnabled(true);
    messwin->setEnabled(true);
    return success;
}

bool Artdlg::loadArt (QString id)
{
    disconnect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    setEnabled (false);
    acc->setEnabled(false);
    list->setEnabled(false);
    messwin->setEnabled(false);
    qApp->setOverrideCursor (waitCursor);
    
    int i=list->currentItem();
    
    if (id!=IDList.at(i))
    {
        int index=0;
        QStrListIterator iter(IDList);
        for (;iter.current();++iter,++index)
        {
            if (id==iter.current())
            {
                list->setCurrentItem(index);
                qApp->restoreOverrideCursor ();
                setEnabled (true);
                acc->setEnabled(true);
                list->setEnabled(true);
                messwin->setEnabled(true);
                QString s;
                s.sprintf ("%d/%d",list->currentItem()+1,IDList.count());
                statusBar()->changeItem(s.data(),1);
                connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
                return true;
                break;
            }
        }
    }
    
    if (id==IDList.at(i))
    {
        goTo(i);
    }
    
    debug ("ID=%s",id.data());
    
    if (!server->isConnected())
    {
        if (!(server->isCached(id.data())==PART_ALL))
        {
            emit needConnection();
            if (!server->isConnected())
            {
                qApp->restoreOverrideCursor ();
                connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
                return false;
            }
        }
    }
    QString *s;
    s=server->article(id.data());
    if (s->isEmpty())
    {
        debug ("entered get from web");
        QString buffer(2048);
        QString urldata("http://ww2.altavista.digital.com/cgi-bin/news.cgi?id@");
        debug ("id-->%s",id.data());
        id=id.mid(1,id.length()-2);
        //    KURL::encodeURL(id);
        urldata+=id;
        KURL url(urldata.data());
        buffer.sprintf("From: KRN\n"
                       "To: You\n"
                       "Date: now\n"
                       "Subject: Problem getting article\n"
                       "ID: <00@00>\n"
                       "\n"
                       "This article seems to have expired or be missing from both"
                       "your news server and Krn's local cache\n"
                       "Or for some reason I just can't get it now.\n"
                       "However, if you have a functional Internet connection, you may"
                       "be able to find it at Altavista following this link:\n"
                       "%s\n\n\n",url.url().data());
        
        //Now, lets create a phony article with this data.
        KMMessage *m=new KMMessage();
        m->fromString(qstrdup(buffer));
        messwin->setMsg(m);
        debug ("exited get from web");
    }
    else
    {
        KMMessage *m=new KMMessage();
        m->fromString(QString(s->data()));
        messwin->setMsg(m);
    }
    delete s;
    
    qApp->restoreOverrideCursor ();
    setEnabled (true);
    acc->setEnabled(true);
    list->setEnabled(true);
    messwin->setEnabled(true);
    QString sb;
    sb.sprintf ("%d/%d",list->currentItem()+1,IDList.count());
    statusBar()->changeItem(sb.data(),1);
    connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    return true;
}

void Artdlg::goTo (int i)
{
    if ((list->lastRowVisible()-(list->height()/list->cellHeight(i)))
        >(i-2))
    {
        list->setTopItem(i);
    }
    else
    {
        if (list->lastRowVisible()<(i+1))
        {
            list->setTopItem(i+2-(list->height()/list->cellHeight(i)));
        }
    }
}


void Artdlg::saveArt (QString id)
{
    if (!server->isConnected())
    {
        if (!(server->isCached(id.data())==PART_ALL))
        {
            emit needConnection();
            if (!server->isConnected())
            {
                qApp->restoreOverrideCursor ();
                return;
            }
        }
    }
    qApp->setOverrideCursor (waitCursor);
    QString *s;
    s=server->article(id.data());
    if (s)
    {
        if (!s->isEmpty())
        {
            qApp->setOverrideCursor (arrowCursor);
            QString f=KFileDialog::getSaveFileName(0,"*",this);
            if (!f.isEmpty())
            {
                int i=QMessageBox::Yes;
                if (QFile::exists(f))
                {
                    QMessageBox *box=new QMessageBox(klocale->translate("KRN - Existing File"),
                                                     klocale->translate("The File you selected already exists"),
                                                     QMessageBox::Information,
                                                     QMessageBox::Yes,
                                                     QMessageBox::No,
                                                     QMessageBox::Abort);
                    box->setButtonText(QMessageBox::Yes,
                                       klocale->translate("OverWrite"));
                    box->setButtonText(QMessageBox::No,
                                       klocale->translate("Append"));
                    box->setButtonText(QMessageBox::Abort,
                                       klocale->translate("Cancel"));
                    i=box->exec();
                }
                
                switch (i)
                {
                case QMessageBox::Yes:
                    {
                        kStringToFile(*s,f,false,true);
                        break;
                    }
                case QMessageBox::No:
                    {
                        QFile fi(f);
                        if (fi.open(IO_WriteOnly | IO_Append))
                        {
                            fi.writeBlock(s->data(),s->length());
                            fi.close();
                        }
                        else
                        {
                            warning ("Can't open file for writing");
                        }
                        break;
                    }
                }
            }
            qApp->restoreOverrideCursor ();
        }
        delete s;
    }
    else
    {
        s=new QString(klocale->translate("From: Krn\nTo: You\n\nError getting article.\nServer said:\n"));
        s->append(server->lastStatusResponse());
        KMMessage *m=new KMMessage();
        m->fromString(s->data());
        messwin->setMsg(m);
        delete s;
        qApp->restoreOverrideCursor ();
        return;
    }
    qApp->restoreOverrideCursor ();
    return;
}

void Artdlg::multiSaveArt (QString id)
{
    if (!server->isConnected())
    {
        if (!(server->isCached(id.data())==PART_ALL))
        {
            emit needConnection();
            if (!server->isConnected())
            {
                qApp->restoreOverrideCursor ();
                return;
            }
        }
    }
    qApp->setOverrideCursor (waitCursor);
    QString *s;
    s=server->article(id.data());
    if (s)
    {
        if (!s->isEmpty())
        {
            qApp->setOverrideCursor (arrowCursor);
            QString f=MultiSavePath;
            if (f.isEmpty())
                f=KFileDialog::getSaveFileName(0,"*",this);
            if (!f.isEmpty())
            {
                MultiSavePath=f;
                if (QFile::exists(f))
                {
                    QFile fi(f);
                    if (fi.open(IO_WriteOnly | IO_Append))
                    {
                        fi.writeBlock(s->data(),s->length());
                        fi.close();
                    }
                    else
                    {
                        warning ("Can't open file for writing");
                    }
                }
                else
                {
                    kStringToFile(*s,f,false,true);
                }
            }
            qApp->restoreOverrideCursor ();
        }
        delete s;
    }
    else
    {
        s=new QString(klocale->translate("From: Krn\nTo: You\n\nError getting article.\nServer said:\n"));
        s->append(server->lastStatusResponse());
        KMMessage *m=new KMMessage();
        m->fromString(s->data());
        messwin->setMsg(m);
        delete s;
        qApp->restoreOverrideCursor ();
        return;
    }
    qApp->restoreOverrideCursor ();
    return;
}

//column is useless right now.
void Artdlg::loadArt (int index,int)
{
    if (index<0) return;
    Article art(IDList.at(index));
    if (loadArt(art.ID))
    {
        art.setRead(true);
        art.threadDepth=*depths.at(index);
        QString formatted;
        art.formHeader(&formatted);
        list->changeItem (formatted.data(),index);
        if (server->isCached(art.ID.data())==PART_ALL)
            list->changeItemColor(QColor(0,0,255),index);
        article->setItemChecked(TOGGLE_EXPIRE, !art.canExpire());  // robert's cache stuff
    }
}

void Artdlg::markArt (int index,int)
{
    if (index<0) return;
    char *ID=IDList.at(index);
    Article art(ID);
    if (art.isMarked())
    {
        art.setMarked(false);
    }
    else
    {
        art.setMarked(true);
    }
    art.threadDepth=*depths.at(index);
    QString formatted;
    art.formHeader(&formatted);
    list->changeItem (formatted.data(),index);
    if (server->isCached(ID)==PART_ALL)
        list->changeItemColor(QColor(0,0,255),index);
}

void Artdlg::decArt (int index,int)
{
    debug ("decart->%d",index);
    if (index<0) return;
    QString *s=0;
    Article art(IDList.at(index));
    art.threadDepth=*depths.at(index);
    if (!(server->isCached(art.ID.data())==PART_ALL))
    {
        if (!server->isConnected())
        {
            emit needConnection();
            if (!server->isConnected())
            {
                qApp->restoreOverrideCursor ();
                return;
            }
         }
         s=server->article(art.ID.data());
         if (s)
         {
            if (!s->isEmpty())
            {
                QString temp( cachepath+"/"+art.ID+".head" );
                artsToDecode.append( temp.data());
                temp= cachepath+"/"+art.ID+".body" ;
                artsToDecode.append( temp.data());
                art.setAvailable(true);
            }
            delete s;
         }
         else
         {
             art.setAvailable(false);
         }
    }
    else //the article *is* cached
    {
         QString temp( cachepath+"/"+art.ID+".head" );
         artsToDecode.append( temp.data());
         temp= cachepath+"/"+art.ID+".body" ;
         artsToDecode.append( temp.data());
    }


    art.setRead(true);
    QString formatted;
    art.formHeader(&formatted);
    list->changeItem (formatted.data(),index);
    if (server->isCached(art.ID.data())==PART_ALL)
        list->changeItemColor(QColor(0,0,255),index);
}


void Artdlg::getSubjects()
{
    qApp->setOverrideCursor(waitCursor);
    statusBar ()->changeItem (klocale->translate("Getting Article List"), 2);
    qApp->processEvents ();
    
    group->getSubjects(server);
    
    statusBar ()->changeItem ("", 2);
    qApp->processEvents ();
    qApp->restoreOverrideCursor();
}
void Artdlg::updateCounter(const char *s)
{
    statusBar()->changeItem (s, 2);
    qApp->processEvents();
}

void Artdlg::popupMenu(int index,int)
{
    markArt(index,0);
}


void Artdlg::FindThis (const char *expr,const char *field,
                       bool casesen,bool wildmode)
{
    static int lastfound=-1;
    static QString lastexpr="";
    static QString lastfield="";
    static bool lastwmode=false;
    static bool lastcsen=false;

    Rule rule("temprule",expr,field,casesen,wildmode);
    
    bool sameQuery=false;
    
    QRegExp regex(expr,casesen,wildmode);
    QStrListIterator iter(IDList);
    
    int index=list->currentItem();
    if (index>0)
    {
        iter+=index;
        ++iter;
        ++index;
    }
    else
    {
        index=0;
    }
    
    sameQuery=(lastexpr==expr) && (lastfield==field)
        && (wildmode==lastwmode) &&(lastcsen==casesen);
    
    lastexpr=expr;
    lastfield=field;
    lastwmode=wildmode;
    lastcsen=casesen;

    if (sameQuery)
    {
        index=lastfound+1;
        iter.toFirst();
        iter+=index;
    }
    if (lastfound >-1)
    {
        if (server->isCached(IDList.at(lastfound))==PART_ALL)
            list->changeItemColor(QColor(0,0,255),lastfound);
        else
            list->changeItemColor(QColor(0,0,0),lastfound);
    }

    int p1=rule.missingParts();
    bool needsConn=false;
    
    disconnect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
    for (;iter.current();++iter,++index)
    {
        int p2=server->isCached(iter.current());
        if ((p1&PART_HEAD) > (p2&PART_HEAD)
            ||
            (p1&PART_BODY) > (p2&PART_BODY))
            needsConn=true;

        if (needsConn && (!server->isConnected()))
            emit needConnection ();

        if (needsConn && (!server->isConnected()))
            break;
        
        if (rule.match(Article (iter.current()),server))
        {
            list->changeItemColor(QColor(255,0,0),index);
            goTo(index);
            lastfound=index;
            break;
        }
    }
    connect (list,SIGNAL(highlighted(int,int)),this,SLOT(loadArt(int,int)));
}
void Artdlg::markReadArt (int index,int)
{
    if (index<0) return;
    Article art(IDList.at(index));
    art.threadDepth=*depths.at(index);
    if (art.isread)
    {
        art.setRead(false);
    }
    else
    {
        art.setRead(true);
    }
    QString formatted;
    art.formHeader(&formatted);
    list->changeItem (formatted.data(),index);
    if (server->isCached(art.ID.data())==PART_ALL)
        list->changeItemColor(QColor(0,0,255),index);
}

void Artdlg::openURL (const char *s)
{
    KURL url(s);
    if( url.isMalformed() )
    {
        warning("Invalid URL clicked!");
        return;
    };
    if(strcmp(url.protocol(),"news")==0)
    {
        if(strchr(url.path(),'@')!=NULL)
        {
            QString s=url.path();
            s="<"+s.right(s.length()-1)+">";
            loadArt(s);
        }
        else emit spawnGroup(url.path());
        return;
    }
    if(strcmp(url.protocol(),"http")==0)
    {
        KFM fm;
        fm.openURL(s);
        return;
    }
    if(strcmp(url.protocol(),"ftp")==0)
    {
        KFM fm;
        fm.openURL(s);
        return;
    }
    if(strcmp(url.protocol(),"mailto")==0)
    {
        QString address(url.path());
        KMMessage *m=new KMMessage();
        QString buffer(2048);
        buffer.sprintf ("To: %s\n\n\n",address.data());
        m->fromString (buffer);
        KMComposeWin *comp=new KMComposeWin(m);
        comp->show();
    }
}


void Artdlg::sortHeaders(int column)
{
    key1=KEY_NONE;
    key2=KEY_NONE;
    key3=KEY_NONE;
    key4=KEY_NONE;
    threaded=false;
    
    switch (column)
    {
    case 0:
        {
            key1=KEY_SENDER;
            break;
        }
    case 1:
        {
            key1=KEY_DATE;
            break;
        }
    case 2:
        {
            key1=KEY_LINES;
            break;
        }
    case 3:
        {
            key1=KEY_SCORE;
            break;
        }
    case 4:
        {
            key1=KEY_SUBJECT;
            break;
        }
    }
    fillTree();
}

void Artdlg::setTarget(int target)
{
    QObject::disconnect (toolBar(1),0,0,0);
    if (target==0)
        QObject::connect (toolBar(1), SIGNAL (clicked (int)), this, SLOT (defaultActions (int)));
    else if (target==1)
        QObject::connect (toolBar(1), SIGNAL (clicked (int)), this, SLOT (taggedActions (int)));
    else if (target==2)
        QObject::connect (toolBar(1), SIGNAL (clicked (int)), this, SLOT (allActions (int)));
    else if (target==3)
        QObject::connect (toolBar(1), SIGNAL (clicked (int)), this, SLOT (readActions (int)));
    else if (target==4)
        QObject::connect (toolBar(1), SIGNAL (clicked (int)), this, SLOT (unreadActions (int)));
}

void Artdlg::closeEvent (QCloseEvent *e)
{
    conf->setGroup("Geometry");
    conf->writeEntry("ArtX",x());
    conf->writeEntry("ArtY",y());
    conf->writeEntry("ArtW",width());
    conf->writeEntry("ArtH",height());
    conf->sync();
    group->isVisible=0;
    KTMainWindow::closeEvent(e);
}
