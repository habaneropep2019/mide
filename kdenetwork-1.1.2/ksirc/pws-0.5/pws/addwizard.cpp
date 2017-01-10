#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <qdir.h>
#include <qfile.h>
#include <qpainter.h>

#include "addwizard.h"
#include "addwizard.moc"

#include <qlayout.h>
#include <qlabel.h>
#include <qlined.h>

#include <kapp.h>
#include <kmsgbox.h>
#include <kfiledialog.h>
#include <keditcl.h>

static KConfig *conf;

AddServerWizard::AddServerWizard()
    :KWizard(0,0,true)
{
    lastPage=0;
    setCancelButton();
//    setOkButton();
    setHelpButton();
//    setEnablePopupMenu(true);
    setEnableArrowButtons(true);
    //    setDirectionsReflectsPage(true);

    conf = kapp->getConfig();

    QObject::connect( this, SIGNAL(okclicked()), this, SLOT(accept()) );
    QObject::connect( this, SIGNAL(cancelclicked()), this, SLOT(reject()) );
    QObject::connect( this, SIGNAL(selected(int)), this, SLOT(pageEntered(unsigned int)));

    WizardPage *wp;
    PublicDirPage *pdp;
    PortPage *pp;
    EndPage *ep;
    QString title;
    KWizardPage *p;

    wp=new("ServerNamePage") ServerNamePage(this);
    title="Server Name";
    p = new("KWizardPage") KWizardPage;
    p->w = wp;
    p->title = title.copy();
    p->enabled = true;
    addPage(p);

    pdp=new("PublicDirPage") PublicDirPage(this);
    title="Server's Public Directory";
    p = new("KWizardPage") KWizardPage;
    p->w = pdp;
    p->title = title.copy();
    p->enabled = true;
    addPage(p);

    pp=new("PortPage") PortPage(this);
    title="Server's Port Number";
    p = new("KWizardPage") KWizardPage;
    p->w = pp;
    p->title = title.copy();
    p->enabled = true;
    addPage(p);

    ep=new("EndPage") EndPage(this);
    title="Finish";
    p = new("KWizardPage") KWizardPage;
    p->w = ep;
    p->title = title.copy();
    p->enabled = true;
    addPage(p);

    adjustSize();
}

AddServerWizard::~AddServerWizard()
{
}

void AddServerWizard::pageEntered(unsigned int pageNum)
{
    debug ("Entered Page %d",pageNum);
    debug ("From Page %d",lastPage);
    if (lastPage<pageNum) //going forward
        //if last page not well filled
        if (!(((WizardPage *)(pages->at(lastPage)->w))->validate()))
        {
            gotoPage(lastPage); //stay put
            return;
        }
    
    //everything is fine
    debug (((WizardPage *)(pages->at(lastPage)->w))->data());
    data.append (((WizardPage *)(pages->at(lastPage)->w))->data());
    lastPage=pageNum; //advance
    if (pageNum==(pages->count()-1))//last page
    {
        setOkButton();
        adjustSize();
    }
}


WizardPage::WizardPage(QWidget *parent)
:QWidget (parent)
{
}

WizardPage::~WizardPage()
{
}

QString WizardPage::data()
{
    return "some data";
}

ServerNamePage::ServerNamePage(QWidget *parent)
:WizardPage (parent)
{
    QVBoxLayout *vl=new("QVBoxLayout") QVBoxLayout (this,10);
    QLabel *l=new("QLabel") QLabel(this);
    l->setText(i18n("Enter a name for this server.\n"
               "This name is used to identify\n"
               "this configuration."));
    l->setMinimumSize(l->sizeHint());
    vl->addWidget(l,10);

    l=new("QLabel") QLabel(this);
    l->setText (i18n("Name:"));
    vl->addWidget(l,10);

    name=new("QLineEdit") QLineEdit(this);
    name->setText(i18n("MyServer"));
    name->setFixedHeight(name->sizeHint().height());
    vl->addWidget(name,0);

    l=new("QLabel") QLabel(this);
    l->setText ("");
    vl->addWidget(l,10);
    
    vl->activate();
    setMinimumSize(300,200);
}
ServerNamePage::~ServerNamePage()
{
}

bool ServerNamePage::validate()
{
    debug ("validate");
    QString nam=name->text();
    
    conf->setGroup("Servers");
    QStrList names;
    conf->readListEntry("ServerNames",names);
    if (nam.isEmpty())
    {
        debug ("empty name");
        KMsgBox::message(0,"PWS: Error","Nothing is not a name.\n"
                               "Please choose one");
        qApp->processEvents();
        return false;
    }
    else if ( (names.find(nam.data())==-1) && (nam!="General") )
    {
        debug ("true");
        return true;
    }
    else
    {
        debug ("repeated name");
        KMsgBox::message(0,"PWS: Error","That server name already exists.\n"
                        "Please choose a new one");
        return false;
    }
}

QString ServerNamePage::data()
{
    return name->text();
}

PublicDirPage::PublicDirPage(QWidget *parent)
:WizardPage (parent)
{
    QVBoxLayout *vl=new("QVBoxLayout") QVBoxLayout (this,10);
    QHBoxLayout *hl=new("QHBoxLayout") QHBoxLayout ();
    QLabel *l=new("QLabel") QLabel(this);
    l->setText(i18n("Enter the name of the directory where\n"
               "the web pages are located.\n"));
    l->setMinimumSize(l->sizeHint());
    vl->addWidget(l,10);

    l=new("QLabel") QLabel(this);
    l->setText (i18n("Directory:"));
    vl->addWidget(l,10);

    name=new("QLineEdit") QLineEdit(this);
    name->setFixedHeight(name->sizeHint().height());
    name->setText(QDir::homeDirPath()+"/html");
    QPushButton *examine=new("QPushButton") QPushButton("...",this);
    examine->setFixedSize(examine->sizeHint());
    
    vl->addLayout (hl,0);
    hl->addWidget(name,10);
    hl->addWidget(examine,0);
    
    l=new("QLabel") QLabel(this);
    l->setText ("");
    vl->addWidget(l,10);
    
    vl->activate();
    setMinimumSize(300,200);

    QObject::connect (examine,SIGNAL(clicked()),
                      this,SLOT(setDirName()));
}

PublicDirPage::~PublicDirPage()
{
}

bool PublicDirPage::validate()
{
    debug ("validate");
    QString nam=name->text();

    if (!QFile::exists(name->text()))
    {
        int i=KMsgBox::yesNo(0,"PWS: Error","That directory doesn't exist.\n"
                             "Do you want me to create it?");
        if (i==1) //he said yes
        {
            mkdir(name->text(), 0700);
        }
        else
            return true;
    }
    return true;
}


void PublicDirPage::setDirName()
{
    QString d=KFileDialog::getDirectory(name->text());
    if (!d.isEmpty())
        name->setText(d.data());
}

QString PublicDirPage::data()
{
    return name->text();
}

PortPage::PortPage(QWidget *parent)
:WizardPage (parent)
{
    QVBoxLayout *vl=new("QVBoxLayout") QVBoxLayout (this,10);
    QLabel *l=new("QLabel") QLabel(this);
    l->setText(i18n("Enter a port for this server.\n"
               "The port is part of the URL for the server\n"
               "for example, in http://my.server.org:800\n"
               "800 is the port number.\n"
               "The standard port for web servers is 80\n"
               "To use a port number under 1024 you will \n"
               "need the root password.")
              );
    l->setMinimumSize(l->sizeHint());
    vl->addWidget(l,10);

    l=new("QLabel") QLabel(this);
    l->setText (i18n("Port Number:"));
    vl->addWidget(l,10);

    port=new("KIntLineEdit") KIntLineEdit(this);
    if (geteuid()==0)
        port->setText("80");
    else
        port->setText("8080");
    port->setFixedHeight(port->sizeHint().height());
    vl->addWidget(port,0);

    l=new("QLabel") QLabel(this);
    l->setText ("");
    vl->addWidget(l,10);
    
    vl->activate();
    setMinimumSize(300,200);
}
PortPage::~PortPage()
{
}

bool PortPage::validate()
{
    return true;
}

QString PortPage::data()
{
    return port->text();
}

EndPage::EndPage(QWidget *parent)
:WizardPage (parent)
{
    QVBoxLayout *vl=new("QVBoxLayout") QVBoxLayout (this,10);
    QLabel *l=new("QLabel") QLabel(this);
    l->setText(i18n("You have finished configuring\n"
               "the web server.\n"
               "Press OK to save it, Cancel to\n"
               "forget about it.\n"));

    l->setMinimumSize(280,170);
    vl->addWidget(l,10);

    vl->activate();
    setMinimumSize(300,200);
}

EndPage::~EndPage()
{
}

