#include <qpushbt.h>
#include <qstrlist.h>
#include <qcombo.h>
#include <qchkbox.h>


#include <kapp.h>


#include "typelayout.h"
#include "tlform.h"
#include "NNTP.h"


#include "sortDlg.h"
#include "sortDlg.moc"

extern KConfig *conf;

sortDlg::sortDlg():QDialog(0,0,true)
{
    TLForm *f=new TLForm("sorting",
                         klocale->translate("KRN - Header Sorting"),
                         this);
    
    l=f->layout;

    l->addGroup("up","",true);
    conf->setGroup("Sorting");

    QStrList *allkeys=new QStrList;
    allkeys->append(klocale->translate("Subject"));
    allkeys->append(klocale->translate("Score"));
    allkeys->append(klocale->translate("Sender"));
    allkeys->append(klocale->translate("Date"));
    allkeys->append(klocale->translate("Lines"));
    allkeys->append(klocale->translate("None"));

    l->addLabel("l1",klocale->translate("Sort by:"));
    l->newLine();
    l->addComboBox("key1",allkeys);
    l->newLine();
    l->addComboBox("key2",allkeys);
    l->newLine();
    l->addComboBox("key3",allkeys);
    l->newLine();
    l->addComboBox("key4",allkeys);

    l->setAlign("key1",AlignLeft|AlignRight);
    l->setAlign("key2",AlignLeft|AlignRight);
    l->setAlign("key3",AlignLeft|AlignRight);
    l->setAlign("key4",AlignLeft|AlignRight);
    
    delete allkeys;
    l->newLine();
    l->addCheckBox("threaded",klocale->translate("Use Threading"),
                   conf->readNumEntry("Threaded",true));

    l->endGroup();
    l->newLine();

    l->addGroup("buttons","",false);
    QPushButton *b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    QPushButton *b2=(QPushButton *)(l->addButton("b2",klocale->translate("Cancel"))->widget);
    l->endGroup();

    b1->setDefault(true);
    connect (b1,SIGNAL(clicked()),SLOT(save()));
    connect (b2,SIGNAL(clicked()),SLOT(reject()));

    //Set the comboboxes to the right spots

    ((QComboBox *)(l->findWidget("key1")))->setCurrentItem(conf->readNumEntry("key1",KEY_SUBJECT));
    ((QComboBox *)(l->findWidget("key2")))->setCurrentItem(conf->readNumEntry("key2",KEY_SCORE));
    ((QComboBox *)(l->findWidget("key3")))->setCurrentItem(conf->readNumEntry("key3",KEY_DATE));
    ((QComboBox *)(l->findWidget("key4")))->setCurrentItem(conf->readNumEntry("key4",KEY_SENDER));
    
    l->activate();
    
}

void sortDlg::save()
{
    conf->setGroup("Sorting");
    conf->writeEntry("key1",((QComboBox *)(l->findWidget("key1")))->currentItem());
    conf->writeEntry("key2",((QComboBox *)(l->findWidget("key2")))->currentItem());
    conf->writeEntry("key3",((QComboBox *)(l->findWidget("key3")))->currentItem());
    conf->writeEntry("key4",((QComboBox *)(l->findWidget("key4")))->currentItem());
    conf->writeEntry("Threaded",((QCheckBox *)l->findWidget("threaded"))->isChecked());
    conf->sync();
    accept();
}


sortDlg::~sortDlg()
{
}
