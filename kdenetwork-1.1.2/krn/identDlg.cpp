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
#include "identDlg.h"
#include <kconfig.h>
#include <kapp.h>
#include <qlabel.h>
#include <qpushbt.h>

#include "tlform.h"
#include "typelayout.h"
#include "kmidentity.h"

//extern KConfig *conf;
extern KMIdentity *identity;

#include "identDlg.moc"

#define Inherited QDialog

IdentDlg::IdentDlg
    ( QWidget* parent,const char* name): Inherited( parent, name, TRUE )
{
    
    
    TLForm *f=new TLForm("identity",
                         klocale->translate("KRN-Identity Configuration"),
                         this);
    
    KTypeLayout *l=f->layout;

    //The skips/newline are to give some extra room and the title don't look
    //too cramped.
    l->skip();
    l->newLine();
    l->addLabel("l1",klocale->translate("Here you should enter your personal information"));
    l->newLine();
    l->addLabel("l11",klocale->translate("You should know what these are!"));
    ((QLabel *)l->findWidget("l1"))->setAlignment(AlignCenter);
    ((QLabel *)l->findWidget("l11"))->setAlignment(AlignCenter);
    l->newLine();
    l->skip();
    l->newLine();

    l->addGroup("entries","",true);
    
    l->addLabel("l2",klocale->translate("Real Name"));
    l->newLine();
    realname =(QLineEdit *)(l->addLineEdit("realname",
                                           identity->fullName())->widget);
    l->newLine();
    
    l->addLabel("l3",klocale->translate("E-mail Address"));
    l->newLine();
    address =(QLineEdit *)(l->addLineEdit("address",
                                          identity->emailAddr())->widget);
    l->newLine();
    
    l->addLabel("l4",klocale->translate("Organization"));
    l->newLine();
    organization =(QLineEdit *)(l->addLineEdit("organization",
                                               identity->organization())->widget);
    
    l->endGroup();
    
    l->newLine();
    
    l->addGroup("buttons","",false);
    b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    b1->setDefault(true);
    b2=(QPushButton *)(l->addButton("b2",klocale->translate("Cancel"))->widget);
    l->endGroup();
    
    l->activate();
    
    QObject::connect (b1,SIGNAL(clicked()),SLOT(accept()));
    QObject::connect (b2,SIGNAL(clicked()),SLOT(reject()));
}


IdentDlg::~IdentDlg()
{
}
