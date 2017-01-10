#include "NNTPConfigDlg.h"

#define Inherited QDialog

#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qchkbox.h>
#include <qcombo.h>


#include <kconfig.h>
#include <kapp.h>
#include <keditcl.h>

#include "tlform.h"
#include "typelayout.h"


#include "NNTPConfigDlg.moc"

extern KConfig *conf;



NNTPConfigDlg::NNTPConfigDlg(QWidget* parent, const char* name):Inherited( parent, name, TRUE )
{

    TLForm *f=new TLForm("expiration",
                         klocale->translate("KRN-NNTP configuration"),
                         this);
    
    KTypeLayout *l=f->layout;

    l->addGroup("servers",klocale->translate("Servers"),true);
    conf->setGroup("sending mail");
    l->addLabel("l1", klocale->translate("SMTP server"));
    l->newLine();
    smtpserver=(QLineEdit *)(l->addLineEdit("smtpserver",conf->readEntry("Smtp Host"))->widget);
    l->newLine();

    l->addLabel("l2", klocale->translate("NNTP Server"));
    l->newLine();
    conf->setGroup("NNTP");
    //    servername=(QLineEdit *)(l->addLineEdit("servername",conf->readEntry("NNTPServer"))->widget);;
    QStrList servlist;
    int i=servlist.find(conf->readEntry("NNTPServer"));
    if (i!=-1)
        servlist.remove(i);
    conf->readListEntry("NNTPServerList",servlist);
    servername=(QComboBox *)(l->addComboBox("servername",0,true)->widget);
    servername->insertItem(conf->readEntry("NNTPServer"),0);
    for (char *serv=servlist.first();serv!=0;serv=servlist.next())
    {
        servername->insertItem(serv);
    }
    servername->setCurrentItem(0);
    l->setAlign("servername",AlignLeft|AlignRight);
    l->endGroup();

    l->newLine();

    l->addGroup("lower");


    connectatstart=(QCheckBox *)(l->addCheckBox("connectatstart",
                                              klocale->translate("Connect on Startup"),
                                                conf->readNumEntry("ConnectAtStart"))->widget);

    silentconnect=(QCheckBox *)(l->addCheckBox("silentconnect",
                                              klocale->translate("Connect without asking"),
                                                conf->readNumEntry("SilentConnect"))->widget);
    l->newLine();
    authenticate=(QCheckBox *)(l->addCheckBox("authenticate",
                                              klocale->translate("Authenticate"),
                                              conf->readNumEntry("Authenticate"))->widget);
    l->skip();
    l->newLine();
    
    l->addLabel ("l3",klocale->translate("User Name"));
    username=(QLineEdit *)(l->addLineEdit("username",conf->readEntry("Username"))->widget);
    
    l->newLine();
    l->addLabel ("l4",klocale->translate("Password"));
    password=(QLineEdit *)(l->addLineEdit("password",conf->readEntry("Password"))->widget);

    l->endGroup();
    l->newLine();

    l->addGroup("g7");
    l->addLabel("l5",klocale->translate("Ask before downloading more than"));
    l->newLine();
    toomany=(KIntLineEdit *)(l->addIntLineEdit("toomany",conf->readEntry("TooMany","100"),-1)->widget);
    l->addLabel("l6",klocale->translate("articles"));
    l->endGroup();
    l->newLine();

    l->addGroup("buttons","",false);
    QPushButton *b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    QPushButton *b2=(QPushButton *)(l->addButton("b2",klocale->translate("Cancel"))->widget);
    
    l->endGroup();



    l->activate();
    
    b1->setDefault(true);
    connect (b1,SIGNAL(clicked()),SLOT(save()));
    connect (b2,SIGNAL(clicked()),SLOT(reject()));
}


NNTPConfigDlg::~NNTPConfigDlg()
{
}


void NNTPConfigDlg::save()
{
    conf->setGroup("NNTP");
    conf->writeEntry("NNTPServer",servername->currentText());
    QStrList l;
    conf->readListEntry("NNTPServerList",l);
    if (l.find(servername->currentText())==-1)
        l.append(servername->currentText());
    conf->writeEntry("NNTPServerList",l);
    conf->writeEntry("ConnectAtStart",connectatstart->isChecked());
    conf->writeEntry("SilentConnect",silentconnect->isChecked());
    conf->writeEntry("Authenticate",authenticate->isChecked());
    conf->writeEntry("Username",username->text());
    conf->writeEntry("Password",password->text());
    conf->writeEntry("TooMany",toomany->text());
    conf->setGroup("sending mail");
    conf->writeEntry("Smtp Host",smtpserver->text());
    conf->sync();
    accept();
}
