#include <pwswidget.h>
#include <pwswidget.moc>

#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>


#include <qlayout.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qmessagebox.h>  

#include <kapp.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kmsgbox.h>

#include <typelayout.h>
#include <pages.h>
#include <kfileio.h>
#include <addwizard.h>


static KConfig *conf;

PWSServer *PWSWidget::server = 0x0;

PWSWidget::PWSWidget(QWidget *parent, const char *name)
	: QWidget(parent, name)
{

    debug("starting pwswidget");
    increaser=0;

    conf = kapp->getConfig();
    
    //Put widgets all around the place
    
    QVBoxLayout *VLay=new("QVBoxLayout") QVBoxLayout(this,5, -1, "VLay");
    QHBoxLayout *HLay1=new("QHBoxLayout") QHBoxLayout(5, "HLay1");
    QHBoxLayout *HLay2=new("QHBoxLayout") QHBoxLayout(5, "HLay2");
    QHBoxLayout *HLay3=new("QHBoxLayout") QHBoxLayout(5, "HLay3");
    VLay->addLayout(HLay1,1);
    VLay->addLayout(HLay2,0);
    VLay->addLayout(HLay3,0);

    list=new("PWSListView") PWSListView(this, "pws_listview"); // Do this before the resize
//    list->setMinimumSize(QSize(100,200));

    // Now some pretty icons in the listview
    list->addColumn("", 18);
    list->addColumn("Things", -5);
    list->setSorting(0,false);
    list->setAllColumnsShowFocus(true);
    QObject::connect (list,SIGNAL(selectionChanged(QListViewItem*)),
                      this,SLOT(flipPage(QListViewItem *)));

    stack=new("QWidgetStack") QWidgetStack(this);
//    stack->setMinimumSize(QSize(300,200));
    stack->raiseWidget(1);
    connect(stack, SIGNAL(aboutToShow(QWidget *)),
            this, SLOT(adjustStack(QWidget *)));

    createGeneralPage();
    loadServers();

    list->setSorting(0,false);
    list->setAllColumnsShowFocus(true);
    list->setSelected(list->firstChild(),true);

    
    HLay1->addWidget(list,5);
    HLay1->addWidget(stack,10);
    
    QPushButton *b1=new("QPushButton") QPushButton(i18n("Save Configuration"),this, "b1");
    QObject::connect (b1,SIGNAL(clicked()),SLOT(accept()));
    QPushButton *b2=new("QPushButton") QPushButton(i18n("(Re)Start Server"),this, "b2");
    QObject::connect (b2,SIGNAL(clicked()),SLOT(slotRestart()));
    QPushButton *b5=new("QPushButton") QPushButton(i18n("Log Window"),this, "b5");
    QObject::connect (b5,SIGNAL(clicked()),SLOT(logWindow()));
    QPushButton *b6=new("QPushButton") QPushButton(i18n("Delete Server"),this, "b6");
    QObject::connect (b6,SIGNAL(clicked()),SLOT(deleteServer()));
    QPushButton *b3=new("QPushButton") QPushButton(i18n("Add Server"),this, "b3");
    QObject::connect (b3,SIGNAL(clicked()),SLOT(addServer()));
    QPushButton *b4=new("QPushButton") QPushButton(i18n("Quit"),this, "b4");
    QObject::connect (b4,SIGNAL(clicked()),SLOT(quit()));

    b1->setFixedSize(b1->sizeHint());
    b2->setFixedSize(b1->sizeHint());
    b3->setFixedSize(b1->sizeHint());
    b4->setFixedSize(b1->sizeHint());
    b5->setFixedSize(b1->sizeHint());
    b6->setFixedSize(b1->sizeHint());
    
    HLay2->addStretch(10);
    HLay2->addWidget(b1,0);
    HLay2->addWidget(b2,0);
    HLay2->addWidget(b5,0);
    
    HLay3->addStretch(10);
    HLay3->addWidget(b6,0);
    HLay3->addWidget(b3,0);
    HLay3->addWidget(b4,0);

    save = FALSE;

    debug("done pwswidget");
}

PWSWidget::~PWSWidget()
{
    if(server != 0){
        delete server;
        debug("Deleted old mathpod");
    }
}

void PWSWidget::quit()
{
  debug ("forgetting everything");
  if(save == TRUE){
    switch(QMessageBox::information(this, "kSirc - PWS",
                                    "You did not save your changes\n"
                                    "For the changes to take effect you must\n"
                                    "save your work and restart the web server\n\n"
                                    "Do you want to save your changes and\n"
                                    "restrart the server before exiting?\n",
                                    "&Save+Restart Server", "&Don't Save", "&Cancel",
                                    0, // Enter == button 0
                                    2)) { // Escape == button 2
                                    case 0:
                                      accept();
                                      restart();
                                      break;
                                    case 1:
                                      emit quitPressed(parent());
                                      break;
                                    case 2:
                                      return;
                                      break;
    }
  }
  emit quitPressed(parent());
}

void PWSWidget::accept()
{
    debug ("accepted, saving configuration for pws");
    //dump all data to the KConfig
    ((GeneralPage *)pages.find("General"))->dumpData();
    bool needRoot=false;

    conf->setGroup("Servers");
    QStrList names;
    conf->readListEntry("ServerNames",names);
    for (char *name=names.first();name!=0;name=names.next())
    {
        ((ServerPage *)pages.find(name))->dumpData();
        if (conf->readBoolEntry("Enabled")!=false)
            if (conf->readNumEntry("Port")<1024)
                needRoot=true;
    }
    debug ("now saving configuration for mathopd");

    QString config;
    config+="DefaultName ";
    struct utsname u;
    uname (&u);
    config+=u.nodename;
    config+="\n";

    // General configuration
    
    conf->setGroup ("General");
    config+="Control {\n";
    config+="\tTypes {\n";
    config+=conf->readEntry("MIMETypes");
    config+="\t}\n";

    config+="\tIndexNames { ";
    QStrList indexes;
    conf->readListEntry("IndexNames",indexes);
    for (char *ex=indexes.first();ex!=0;ex=indexes.next())
    {
        config+=ex;
        config+=" ";
    }
    config+=+" }\n";
    
    config+="\tSpecials {\n";

    config+="\t\tCGI { ";
    QStrList extensions;
    conf->readListEntry("CGIext",extensions);
    for (char *ex=extensions.first();ex!=0;ex=extensions.next())
    {
        config+=ex;
        config+=" ";
    }
    config+=+" }\n";

    config+="\t}\n";
    config+="}\n";

    
    //Per server data
    
    for (char *name=names.first();name!=0;name=names.next())
    {
        conf->setGroup(name);
        if (conf->readBoolEntry("Enabled")==false)
            continue;
        config+="Server {\n";
        config+="\tPort ";
        config+=conf->readEntry("Port")+"\n";
        //Main html dir
        config+="\tControl {\n";
        config+="\t\tAlias /\n";
        config+="\t\tLocation ";
        config+=conf->readEntry("HTMLDir")+"\n";
        config+="\t}\n";
        
        QString alias,dir;
        
        //alias1
        alias=conf->readEntry("Alias1");
        dir=conf->readEntry("AliasDir1");
        if ((!alias.isEmpty()) && (!dir.isEmpty()))
        {
            config+="\tControl {\n";
            config+="\t\tAlias "+alias+"\n";
            config+="\t\tLocation ";
            config+=dir+"\n";
            config+="\t}\n";
        }
        //alias2
        alias=conf->readEntry("Alias2");
        dir=conf->readEntry("AliasDir2");
        if ((!alias.isEmpty()) && (!dir.isEmpty()))
        {
            config+="\tControl {\n";
            config+="\t\tAlias "+alias+"\n";
            config+="\t\tLocation ";
            config+=dir+"\n";
            config+="\t}\n";
        }
        //alias3
        alias=conf->readEntry("Alias3");
        dir=conf->readEntry("AliasDir3");
        if ((!alias.isEmpty()) && (!dir.isEmpty()))
        {
            config+="\tControl {\n";
            config+="\t\tAlias "+alias+"\n";
            config+="\t\tLocation ";
            config+=dir+"\n";
            config+="\t}\n";
        }
        config+="}\n";
    }
    
    
    //Logs and stuff
    
    conf->setGroup ("General");
    QString logdir=conf->readEntry("Logs");
    
    config+="Log  ";
    config+=logdir+"WebLog\n";
    config+="ErrorLog ";
    config+=logdir+"ErrorLog\n";
    config+="ChildLog ";
    config+=logdir+"ChildLog\n";
    config+="PIDFile ";
    config+=logdir+"PIDFile\n";
    config+="Umask 022\n";

    /*
     * There's not way I'm going to allow this to ever become root
     */
    
#undef NEEDROOT
#ifdef NEEDROOT
#warning I am not really sure about this uid/gid thing.
    if (needRoot)
    {
        QString usergrp;
        passwd *p=getpwuid(getuid());
        group *g=getgrgid(getgid());
        usergrp.sprintf ("user %s\ngroup %s\n",p->pw_name,g->gr_name);
        config+=usergrp;
    }
#endif
    
    kStringToFile(config,KApplication::localkdedir()+"/share/apps/pws/server-config");

    save = FALSE;

}

void PWSWidget::restart()
{
    conf = kapp->getConfig();
    conf->setGroup("General");
    QString logdir=conf->readEntry("Logs");
    
    if(server != 0)
        delete server;
    server = new("PWSServer") PWSServer(KApplication::localkdedir()+"/share/apps/pws/server-config", logdir);
    
//    system(command.data());
}

void PWSWidget::stop()
{
    if(server != 0)
        delete server;
    server = 0x0;

}

void PWSWidget::slotRestart(){
  if(save == TRUE){
    switch(QMessageBox::information(this, "kSirc - PWS",
                                    "You did not save your changes\n"
                                    "For the changes to take effect you must\n"
                                    "save your work.\n\n"
                                    "Do you want to save your changes and\n\n",
                                    "&Save+Restart Server", "&Cancel", "",
                                    0, // Enter == button 0
                                    1)) { // Escape == button 2
                                    case 0:
                                      accept();
                                      restart();
                                      break;
                                    case 1:
                                      return;
                                      break;
    }
  }
  PWSWidget::restart();
}

void PWSWidget::addServer()
{

    AddServerWizard *wiz=new("AddServerWizard") AddServerWizard();
    int i=wiz->exec();
    qApp->processEvents();
    debug ("done wizard-->%d",i);
    if (i==1) //pressed OK
    {
        conf->setGroup("Servers");
        QStrList names;
        conf->readListEntry("ServerNames",names);
        debug ("name=%s",wiz->data.at(0));
        names.append (wiz->data.at(0));
        conf->writeEntry("ServerNames",names);
        conf->setGroup(wiz->data.at(0));
        debug ("htmldir=%s",wiz->data.at(1));
        conf->writeEntry("HTMLDir",wiz->data.at(1));
        debug ("port=%d",atoi(wiz->data.at(2)));
        conf->writeEntry("Port",atoi(wiz->data.at(2)));
        conf->sync();
    }
    loadServers();
    save = TRUE;
}


void PWSWidget::deleteServer()
{
    if(current == "General")
        return;
    
    conf->setGroup("Servers");
    QStrList names;
    conf->readListEntry("ServerNames",names);
    names.remove(names.find(current));
    conf->writeEntry("ServerNames",names);

    list->setEnabled(FALSE);
    stack->setEnabled(FALSE);
    
    list->clear();
    QDictIterator<QWidget> it(pages);
    while(it.current()){
        stack->removeWidget(it.current());
        pages.remove(it.currentKey());
        delete it.current();
        ++it;
    }
    createGeneralPage();
    loadServers();

    list->setEnabled(TRUE);
    stack->setEnabled(TRUE);
    repaint(TRUE);
    save = TRUE;
    list->setSelected(list->firstChild(), TRUE);
    

}

void PWSWidget::loadServers()
{
    conf->setGroup("Servers");
    QStrList names;
    conf->readListEntry("ServerNames",names);

    if (names.count()==0)
    {
      //addServer();
    }
    conf->setGroup("Servers");
    conf->readListEntry("ServerNames",names);
    for (char *name=names.first();name!=0;name=names.next())
        createServerPage(name);
}

void PWSWidget::createGeneralPage()
{
    //list stuff
    QListViewItem *item=new("QListViewItem") QListViewItem(list);
    item->setPixmap(0,Icon("pws_general.xpm"));
    item->setText(1,"General");

    //stack stuff
    GeneralPage *w=new("GeneralPage") GeneralPage(stack);
    //w->show();
    stack->addWidget(w,increaser);
    increaser++;
    pages.insert("General",w);
}


void PWSWidget::createServerPage(const char *name)
{
    // Don't add duplicates
    for(QListViewItem *item = list->firstChild(); item != 0; item = item->itemBelow()){
        if(strcmp(name, item->text(1)) == 0)
            return;
    }
    
    //list stuff
    QListViewItem *item=new("QListViewItem") QListViewItem(list);
    item->setPixmap(0,Icon("pws_www.xpm"));
    item->setText(1,name);
    list->setSorting(0,false);

    //stack stuff
    ServerPage *w=new("ServerPage") ServerPage(stack,name);
    w->resize(stack->size());
    stack->addWidget(w,increaser);
    stack->raiseWidget(w);
    list->setSelected(item,true);
    increaser++;
    pages.insert(name,w);
}

void PWSWidget::flipPage(QListViewItem *item)
{
    stack->raiseWidget(pages.find(item->text(1)));
    current = item->text(1);
}

void PWSWidget::logWindow()
{
    if(server != 0){
        server->showLogWindow(TRUE);
    }
}


void PWSWidget::resizeEvent(QResizeEvent *e){
    debug("Got resize");
    QWidget *w = stack->visibleWidget();
    if(w != 0x0)
        w->resize(stack->size());
    QWidget::resizeEvent(e);
}

void PWSWidget::adjustStack(QWidget *w){
    debug("Calling resize");
    w->resize(stack->size());
}

void PWSListView::resizeEvent(QResizeEvent *e){
        this->setColumnWidth(0, 18);
        this->setColumnWidth(1, this->width()-19);
}

