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
#include "asker.h"

#include <kapp.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>

#include "tlform.h"
#include "typelayout.h"

#include "asker.moc"

#define Inherited QDialog

Asker::Asker
    (QWidget* parent,const char* name) : Inherited( parent, name, TRUE )
{

    TLForm *f=new TLForm("identity","KRN",this);
    
    KTypeLayout *l=f->layout;

    l->skip();
    l->newLine();
    label=(QLabel *)(l->addLabel("label","")->widget);
    l->newLine();
    l->skip();
    l->newLine();

    entry=(QLineEdit *)(l->addLineEdit("entry","")->widget);
    l->newLine();
    l->skip();
    l->newLine();
    b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);

    ((QLabel *)l->findWidget("label"))->setAlignment(AlignHCenter);
    
    l->activate();
    resize( 350, 10 );

    b1->setDefault(true);
    QObject::connect (b1,SIGNAL(clicked()),SLOT(accept()));
    QObject::connect (entry,SIGNAL(returnPressed()),SLOT(accept()));
    entry->setFocus();
}


Asker::~Asker()
{
}
