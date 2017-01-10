#include <pages.h>
#include "pages.moc"

#include <qlined.h>
#include <qmlined.h>
#include <qpushbt.h>
#include <qdir.h>
#include <qcheckbox.h>

#include <kconfig.h>
#include <kfiledialog.h>
#include <keditcl.h>

#include <typelayout.h>

static KConfig *conf;

Page::Page(QWidget *parent = 0, const char *name = 0)
	: QWidget(parent, name)
{
    conf = kapp->getConfig();
    resize(1024,1024);
}


Page::~Page()
{
}

void Page::dumpData()
{
}


GeneralPage::GeneralPage(QWidget *parent = 0, const char *name = 0)
	: Page(parent, name)
{
    conf->setGroup("General");
    TLForm *f=new("TLForm") TLForm("general","General",this);
    KTypeLayout *l=f->layout;
    
    l->addBook("book");
    
    l->addPage("p2","MIME types");

    l->addGroup ("gg1","Index Files:",true);
    QStrList indexes;
    indexnames=(QMultiLineEdit *)(l->addMultiLineEdit("indexes","")->widget);
    conf->readListEntry("IndexNames",indexes);
    if (indexes.count()==0)
    {
        indexes.append("index.html");
        indexes.append("index.htm");
    }
    int i=0;
    for (char *ex=indexes.first();ex!=0;ex=indexes.next(),i++)
    {
        indexnames->insertLine(ex,i);
    }
    l->endGroup();//gg1

    l->addGroup ("gg2","CGI extensions:",true);
    QStrList extensions;
    cgiext=(QMultiLineEdit *)(l->addMultiLineEdit("cgiext","")->widget);
    conf->readListEntry("CGIext",extensions);
    if (extensions.count()==0)
    {
        extensions.append("cgi");
        extensions.append("pl");
    }
    i=0;
    for (char *ex=extensions.first();ex!=0;ex=extensions.next(),i++)
    {
        cgiext->insertLine(ex,i);
    }
    l->endGroup();//gg2

    l->newLine();

    l->addGroup ("gg3","Extra MIME Types:",true);
    mimetypes=(QMultiLineEdit *)(l->addMultiLineEdit("mimetypes","")->widget);
    QString mtyp=conf->readEntry("MIMETypes",
                                 "text/html { html htm shtm HTML HTM SHTML }\n"
                                 "image/jpeg { jpg jpeg JPG JPEG }\n"
                                 "image/gif { gif GIF }\n"
                                 "text/plain { * }\n");
    mimetypes->insertLine(mtyp.data(),0);
    l->endGroup();//gg3

    l->setSpawn("gg3",2,1);

    l->endGroup();//p2
    
    l->addPage("p1","Administration");

    QString home=QDir::homeDirPath();

    l->addGroup("g1","",true);
    l->addLabel ("l1","This is the place where logs and similar\n");
    l->newLine();
    l->addLabel ("l1","administrative stuff will be stored.\n");
    l->newLine();
    l->addLabel ("l1","You usually don't need to care much about this.");
    l->endGroup();//g1
    l->newLine();

    l->addGroup("g2","Directory:",true);
    logfile=(QLineEdit *)((l->addLineEdit
                           ("admindir",
                            conf->readEntry("Logs",KApplication::localkdedir()+"/share/apps/pws/")))->widget);
    QPushButton *b=(QPushButton *)(l->addButton("examinelog","...")->widget);
    QObject::connect (b,SIGNAL(clicked()),this,SLOT(exam1()));
    l->endGroup();//g2

    l->endGroup();//p1
    
    l->endGroup();//book
    
    l->activate();
}


GeneralPage::~GeneralPage()
{
}

void GeneralPage::dumpData()
{
    conf->setGroup("General");
    QStrList indexes;
    for (int i=0; i<indexnames->numLines();i++)
    {
        if (indexnames->textLine(i) && (!QString(indexnames->textLine(i)).isEmpty()))
            indexes.append(indexnames->textLine(i));
    }
    conf->writeEntry("IndexNames",indexes);

    QStrList extensions;
    for (int i=0; i<cgiext->numLines();i++)
    {
        if (cgiext->textLine(i) && (!QString(cgiext->textLine(i)).isEmpty()))
            extensions.append(cgiext->textLine(i));
    }
    conf->writeEntry("CGIext",extensions);
    conf->writeEntry("Logs",logfile->text());

    conf->writeEntry("MIMETypes",mimetypes->text().stripWhiteSpace());
    
    conf->sync();
}

void GeneralPage::exam1()
{
    QString d=KDirDialog::getDirectory(KApplication::localkdedir()+"/share/apps/pws/");
    if (!d.isEmpty())
        logfile->setText(d.data());
}


ServerPage::ServerPage(QWidget *parent = 0, const char *name = 0)
	: Page(parent, name)
{

    QPushButton *b;
    conf->setGroup(name);
    Name=name;
    TLForm *f=new("TLForm") TLForm(name,name,this);
    KTypeLayout *l=f->layout;
    QString home=QDir::homeDirPath();
    
    l->addBook("book");
    
    l->addPage("p2","Aliases");
    
    l->addGroup ("a1", "Alias 1",true);
    l->addLabel ("l1","Directory:");
    aliasdir1=(QLineEdit *)(l->addLineEdit("dir1","                                 ")->widget);
    aliasdir1->setText(conf->readEntry("AliasDir1",""));
    l->setAlign("dir1",AlignLeft|AlignRight);
    b=(QPushButton *)(l->addButton("exdir1","...")->widget);
    QObject::connect (b,SIGNAL(clicked()),this,SLOT(exama1()));
    l->newLine();
    l->addLabel ("l1","Alias:");
    alias1=(QLineEdit *)(l->addLineEdit("ali1","")->widget);
    alias1->setText(conf->readEntry("Alias1",""));
    l->endGroup ();//a1
    l->newLine();
    
    l->addGroup ("a2", "Alias 2",true);
    l->addLabel ("l2","Directory:");
    aliasdir2=(QLineEdit *)(l->addLineEdit("dir2","                                 ")->widget);
    aliasdir2->setText(conf->readEntry("AliasDir2",""));
    l->setAlign("dir2",AlignLeft|AlignRight);
    b=(QPushButton *)(l->addButton("exdir2","...")->widget);
    QObject::connect (b,SIGNAL(clicked()),this,SLOT(exama2()));
    l->newLine();
    l->addLabel ("l2","Alias:");
    alias2=(QLineEdit *)(l->addLineEdit("ali2","")->widget);
    alias2->setText(conf->readEntry("Alias2",""));
    l->endGroup ();//a2
    l->newLine();
    
    l->addGroup ("a3", "Alias 3",true);
    l->addLabel ("l3","Directory:");
    aliasdir3=(QLineEdit *)(l->addLineEdit("dir3","                                 ")->widget);
    aliasdir3->setText(conf->readEntry("AliasDir3",""));
    l->setAlign("dir3",AlignLeft|AlignRight);
    b=(QPushButton *)(l->addButton("exdir3","...")->widget);
    QObject::connect (b,SIGNAL(clicked()),this,SLOT(exama3()));
    l->newLine();
    l->addLabel ("l3","Alias:");
    alias3=(QLineEdit *)(l->addLineEdit("ali3","")->widget);
    alias3->setText(conf->readEntry("Alias3",""));
    l->endGroup ();//a3
    l->endGroup ();//p2


    l->addPage("p1","Basic");


    l->addGroup("g1","Port:",true);
    port=(KIntLineEdit *)(l->addIntLineEdit("port",conf->
                                            readEntry("Port","8000"),5)->widget);
    l->endGroup();//g1
    l->addGroup("g1.1","Server Name",true);
    ((QLineEdit *)(l->addLineEdit("name",Name.data())->widget))->setEnabled(false);
    l->endGroup();//g1.1
    l->newLine();
    
    l->addGroup("g2","Main HTML directory:",true);
    htmldir=(QLineEdit *)(l->addLineEdit
                          ("filepath",
                           conf->readEntry("HTMLDir",
                                           home+QString("/html_")+name+"/"))->widget);
    b=(QPushButton *)(l->addButton("examinefp","...")->widget);
    QObject::connect (b,SIGNAL(clicked()),this,SLOT(examfp()));
    l->endGroup();//g2

    l->setSpawn ("g2",2,1);
    
    l->newLine();
    l->addGroup("g3");
    enabled=(QCheckBox *)(l->addCheckBox ("enabled", "Server is enabled",
                    conf->readBoolEntry("Enabled",true))->widget);
    l->endGroup();//g3
    
    l->endGroup(); //p1


    l->endGroup(); //book
    l->activate();
}

QString sanitize(QString dirname)
{
    if (dirname.isEmpty())
        return dirname.copy();
    if (dirname.left(1)!="/")
        dirname=QString("/")+dirname;
    if (dirname.right(1)!="/")
        dirname+="/";
    return dirname.copy();
}

void ServerPage::dumpData()
{
    conf->setGroup(Name.data());
    conf->writeEntry("Enabled",enabled->isChecked());
    conf->writeEntry("Port",port->text());
    conf->writeEntry("HTMLDir",sanitize(htmldir->text()));
    conf->writeEntry("Alias1",sanitize(alias1->text()));
    conf->writeEntry("AliasDir1",sanitize(aliasdir1->text()));
    conf->writeEntry("Alias2",sanitize(alias2->text()));
    conf->writeEntry("AliasDir2",sanitize(aliasdir2->text()));
    conf->writeEntry("Alias3",sanitize(alias3->text()));
    conf->writeEntry("AliasDir3",sanitize(aliasdir3->text()));
    conf->sync();
}

void ServerPage::examfp()
{
    QString d=KFileDialog::getDirectory(QDir::homeDirPath()).data();
    if (!d.isEmpty())
        htmldir->setText(d.data());
}

void ServerPage::exama1()
{
    QString d=KFileDialog::getDirectory(QDir::homeDirPath()).data();
    if (!d.isEmpty())
        aliasdir1->setText(d.data());
}
void ServerPage::exama2()
{
    QString d=KFileDialog::getDirectory(QDir::homeDirPath()).data();
    if (!d.isEmpty())
        aliasdir2->setText(d.data());
}
void ServerPage::exama3()
{
    QString d=KFileDialog::getDirectory(QDir::homeDirPath()).data();
    if (!d.isEmpty())
        aliasdir3->setText(d.data());
}


ServerPage::~ServerPage()
{
}
