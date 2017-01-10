#include "printdlg.h"

#include <qdialog.h>
#include <qlist.h>
#include <qlined.h>
#include <qcombo.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <kconfig.h>
#include <kapp.h>

#include "typelayout.h"

#include "printdlg.moc"

extern KConfig *conf;

KTypeLayout *l;

PrintDlg::PrintDlg() :
    QDialog (0,klocale->translate("KRN - Print Settings"),true)
{

    conf->setGroup("Printing");
    TLForm *f=new TLForm("printing",
                         klocale->translate("KRN - Printing Settings"),
                         this);
    
    l=f->layout;

    l->addBook("book");

    l->addPage("General",klocale->translate("General"));

    l->addGroup("g2",klocale->translate("Printing System"),true);
    l->newLine();
    l->addRadioButton("htmlprint","r1",klocale->translate("Use HTML Printing"),
                     conf->readNumEntry("HTMLPrinting",true));
    l->newLine();
    l->addRadioButton("directprint","r1",klocale->translate("Use Direct Printing"),
                     conf->readNumEntry("DirectPrinting",false));
    l->endGroup();//g2
    l->newLine();
    
    l->addGroup("g1",klocale->translate("Direct Printing"),true);

    l->newLine();
    l->addLabel("l1",klocale->translate("Print using:"));
    l->addLineEdit("commandname",
                   conf->readEntry("CommandName","lpr"));
    l->newLine();
    l->addLabel("lfeed",klocale->translate("Feed it with:"));
    QStrList *feeds=new QStrList;
    feeds->append (klocale->translate("Postscript"));
    feeds->append (klocale->translate("HTML"));
    feeds->append (klocale->translate("Text"));
    l->addComboBox("feeds",feeds);
    ((QComboBox *)(l->findWidget("feeds")))->setCurrentItem(conf->readNumEntry("Feed",2));
    delete feeds;
    l->endGroup();

    l->endGroup();//general

/*
    l->addPage("Formatter",klocale->translate("Formatter"));

    l->addLabel("fontl",klocale->translate("Font:"));
    QStrList *fonts=new QStrList;
    fonts->append("Times");
    fonts->append("Courier");
    fonts->append("Helvetica");
    fonts->append("Palatino");
    fonts->append("Symbol");
    fonts->append("Bookman");
    fonts->append("Helvetica Narrow");
    fonts->append("New Century");
    fonts->append("Avant Garde");
    l->addComboBox("font",fonts);
    delete fonts;
    l->endGroup();//formatter
*/

    l->endGroup(); //book

    l->newLine();
    
    l->addGroup("buttons","",false);
    QPushButton *b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    QPushButton *b2=(QPushButton *)(l->addButton("b2",klocale->translate("Cancel"))->widget);
    l->endGroup();
    
    l->activate();
    
    b1->setDefault(true);
    QObject::connect (b1,SIGNAL(clicked()),SLOT(save()));
    QObject::connect (b2,SIGNAL(clicked()),SLOT(reject()));
}

void PrintDlg::save()
{
    conf->setGroup("Printing");
    conf->writeEntry("HTMLPrinting",
                     ((QRadioButton *)l->findWidget("htmlprint"))->isChecked());
    conf->writeEntry("DirectPrinting",
                     ((QRadioButton *)l->findWidget("directprint"))->isChecked());
    conf->writeEntry("CommandName",
                     ((QLineEdit *)l->findWidget("commandname"))->text());
    conf->writeEntry("Feed",
                     ((QComboBox *)l->findWidget("feeds"))->currentItem());
    conf->sync();
    accept();
}

PrintDlg::~PrintDlg()
{
}
