#include "TooManydlg.h"
#include "TooManydlg.moc"

#include <qdialog.h>
#include <qkeycode.h>
#include <qpushbt.h>
#include <qradiobutton.h>
#include <qchkbox.h>

#include <kapp.h>
#include <keditcl.h>

#include "tlform.h"
#include "typelayout.h"
extern KConfig *conf;

TooManyDlg::TooManyDlg (QWidget* parent,const char* name)
:QDialog( parent, name, TRUE )
{
    conf->setGroup("NNTP");

    newthres=false;
    frombottom=true;
    TLForm *f=new TLForm("too many",
                         klocale->translate("KRN-Confirmation"),
                         this);
    KTypeLayout *l=f->layout;

    l->newLine();
    l->addLabel("l",klocale->translate("Read how many articles?"));
    l->newLine();
    number=(KIntLineEdit *)(l->addIntLineEdit("howmany",conf->readEntry("TooMany","100"),-1)->widget);
    l->newLine();
    l->newLine();

    l->addGroup("from",klocale->translate("Get them from"),true);
    fbrb=(QRadioButton *)(l->addRadioButton("oldest","fromgroup",klocale->translate("Oldest articles"),true)->widget);
    l->newLine();
    l->addRadioButton("newest","fromgroup",klocale->translate("Newest articles"));
    l->endGroup();// from
    l->newLine();

    ntcb=(QCheckBox *)(l->addCheckBox("newsetting",klocale->translate("Make this the new threshold"),false)->widget);
    l->newLine();

    l->newLine();
    l->addGroup("buttons","",false);
    QPushButton *b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    l->endGroup(); //buttons

    l->activate();

    b1->setDefault(true);
    connect (b1,SIGNAL(clicked()),SLOT(accept()));
    disconnect (this,0,this,SLOT(reject()));
}

int TooManyDlg::exec()
{
    qApp->setOverrideCursor (arrowCursor);
    int res=QDialog::exec();
    qApp->restoreOverrideCursor ();
    howmany=number->getValue();
    frombottom=fbrb->isChecked();
    newthres=ntcb->isChecked();
    if (newthres)
    {
        conf->setGroup("NNTP");
        conf->writeEntry("TooMany",number->text());
    }
    return res;
}


void TooManyDlg::keyPressEvent( QKeyEvent *e )
{

    if ( e->key() == Key_Escape )
    {
        e->ignore();
    }
    else
    {
        QDialog::  keyPressEvent( e );
    }
}

TooManyDlg::~TooManyDlg()
{
}
