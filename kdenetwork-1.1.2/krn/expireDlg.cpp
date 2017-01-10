#include "expireDlg.h"



#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>

#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>

#include "typelayout.h"
#include "tlform.h"

#include "expireDlg.moc"


extern KConfig *conf;

ExpireDlg::ExpireDlg() :
    QDialog (0,klocale->translate("KRN - Expire Settings"),true)
{

    conf->setGroup("Cache");
    TLForm *f=new TLForm("expiration",
                         klocale->translate("KRN - Expire Settings"),
                         this);

    KTypeLayout *l=f->layout;

    //The skips/newline are to give some extra room and the title don't look
    //too cramped.
    l->skip();
    l->newLine();
    l->addLabel("l1",klocale->translate("For how long should we keep things (in days)?"));
    ((QLabel *)l->findWidget("l1"))->setAlignment(AlignCenter);
    l->newLine();
    l->skip();

    l->newLine();

    l->addGroup("entries","",true);

    l->skip();
    l->addLabel("l2",klocale->translate("Read:"));
    l->addLabel("l2",klocale->translate("Unread:"));
    l->newLine();
    
    l->addLabel("l2",klocale->translate("Article Bodies:"));
    bodies=(KIntLineEdit *)(l->addIntLineEdit("bodies",conf->readEntry("ExpireReadBodies","5"),3)->widget);
    bodies2=(KIntLineEdit *)(l->addIntLineEdit("bodies2",conf->readEntry("ExpireUnreadBodies","5"),3)->widget);
    l->newLine();

    l->addLabel("l3",klocale->translate("Article Headers:"));
    headers=(KIntLineEdit *)(l->addIntLineEdit("headers",conf->readEntry("ExpireReadHeaders","5"),3)->widget);
    headers2=(KIntLineEdit *)(l->addIntLineEdit("headers2",conf->readEntry("ExpireUnreadHeaders","5"),3)->widget);
    l->newLine();

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

void ExpireDlg::save()
{
    conf->setGroup("Cache");
    conf->writeEntry("ExpireReadBodies",bodies->text());
    conf->writeEntry("ExpireReadHeaders",headers->text());
    conf->writeEntry("ExpireUnreadBodies",bodies2->text());
    conf->writeEntry("ExpireUnreadHeaders",headers2->text());
    accept();
}



ExpireDlg::~ExpireDlg()
{
}

