#include <keditcl.h>

#include <qpushbt.h>
#include <qlist.h>
#include <qlined.h>
#include <qlabel.h>
#include <qcombo.h>
#include <qlistbox.h>
#include <qchkbox.h>


#include <kapp.h>
#include <ksimpleconfig.h>

#include "typelayout.h"
#include "tlform.h"
#include "asker.h"
#include "rules.h"


#include "rulesDlg.h"
#include "rulesDlg.moc"

extern KSimpleConfig *ruleFile;
extern QList <Rule> ruleList;

rulesDlg::rulesDlg():QDialog(0,0,true)
{

    ruleFile->setGroup("Index");
    QStrList names;
    ruleFile->readListEntry("RuleNames",names);
    QStrList gnames;
    ruleFile->readListEntry("GlobalRules",gnames);


    TLForm *f=new TLForm("rules",
                         klocale->translate("KRN - Scoring Rules Editor"),
                         this);
    
    l=f->layout;

    l->addBook ("book");

    l->addPage ("globalpage",klocale->translate("Global Rules"));

    l->addGroup ("l1",klocale->translate("All Rules"),true);
    list2=(QListBox *)(l->addListBox("allrules",&names)->widget);
    l->endGroup();

    l->addGroup ("buttons");
    QPushButton *addglb=(QPushButton *)
        (l->addButton("addToActive","-->")->widget);
    l->newLine();
    QPushButton *rmglb=(QPushButton *)
        (l->addButton("removeFromActive","<--")->widget);

    connect (addglb,SIGNAL(clicked()),this,SLOT(addGlobal()));
    connect (rmglb,SIGNAL(clicked()),this,SLOT(deleteGlobal()));
    
    l->endGroup();
    
    l->addGroup ("l1",klocale->translate("Active Rules"),true);
    globalList=(QListBox *)(l->addListBox("globals",&gnames)->widget);
    l->newLine();
    l->endGroup();

    l->endGroup(); //globalpage
    
    l->addPage ("editpage",klocale->translate("Rule Editor"));


    list=(QListBox *)(l->addListBox("rulenames",&names)->widget);
    list->setMinimumWidth(100);
    connect (list,SIGNAL(highlighted(const char*)),
             this,SLOT(editRule(const char*)));
                       

    l->addGroup ("rulbut","");
    QPushButton *save=(QPushButton *)
        (l->addButton ("save",klocale->translate("Save"))->widget);
    connect (save,SIGNAL(clicked()),SLOT(saveRule()));
    l->newLine();


    QPushButton *saveas=(QPushButton *)
        (l->addButton ("saveas",klocale->translate("Save as"))->widget);
    connect (saveas,SIGNAL(clicked()),SLOT(saveRuleAs()));
    l->newLine();


    QPushButton *delrule=(QPushButton *)
        (l->addButton ("delete",klocale->translate("Delete"))->widget);
    connect (delrule,SIGNAL(clicked()),SLOT(deleteRule()));

    l->endGroup(); //rulbut

    l->addGroup ("ruleedit","",true);


    l->addLabel("l1",klocale->translate("Match Articles With:"));
    l->newLine();
    l->addLineEdit("expr","");
    l->newLine();

    l->addLabel("l2",klocale->translate("In Field:"));
    l->newLine();

    QStrList *fields=new QStrList;
    fields->append( "Sender" );
    fields->append( "Subject" );
    fields->append( "Cached Header" );
    fields->append( "Cached Body" );
    fields->append( "Cached Message" );
    fields->append( "Header" );
    fields->append( "Body" );
    fields->append( "Message" );
    l->addComboBox("field",fields);
    l->setAlign("field",AlignLeft|AlignRight);
    delete fields;

    l->newLine();
    l->addGroup("gg","",false);
    l->addLabel ("l",klocale->translate("Weight:"));
    l->addIntLineEdit("value","",4);
    l->addCheckBox("neg",klocale->translate("Negative"),false);
    l->endGroup(); //gg
    l->newLine();
    
    l->addGroup("gg","",false);
    l->addCheckBox("casesen",klocale->translate("Case Sensitive"),false);
    l->addCheckBox("wildmode",klocale->translate("Wildcard Mode"),false);
    l->endGroup(); //gg

    l->endGroup(); //ruleedit
    
    l->endGroup(); //rule editor page

    l->endGroup(); //book
    
    l->newLine();
    
    l->addGroup("buttons","",false);
    QPushButton *b1=(QPushButton *)(l->addButton("b1",klocale->translate("Done"))->widget);
    l->endGroup();

    b1->setDefault(true);
    connect (b1,SIGNAL(clicked()),SLOT(accept()));

    l->setAlign("value",AlignRight|AlignLeft);
    l->activate();
    
    list->setCurrentItem(0);
    
}

rulesDlg::~rulesDlg()
{
}


void rulesDlg::editRule(const char *name)
{
    rule=new Rule(0,0,Rule::Sender,false,false);
    rule->load(name);
    
    ((QLineEdit *)(l->findWidget("expr")))->setText(rule->regex.pattern());
    ((QComboBox *)(l->findWidget("field")))->setCurrentItem((int)rule->field);
    ((QCheckBox *)(l->findWidget("casesen")))->setChecked(rule->regex.caseSensitive());
    ((QCheckBox *)(l->findWidget("wildmode")))->setChecked(rule->regex.wildcard());
    bool b=false;
    QString s;
    if (rule->value <0)
    {
        s.setNum(-1*rule->value);
        b=true;
    }
    else
        s.setNum(rule->value);

    
    ((KIntLineEdit *)(l->findWidget("value")))->setText(s);
    ((QCheckBox *)(l->findWidget("neg")))->setChecked(b);
    
    delete rule;
}
void rulesDlg::saveRuleAs()
{
    Asker ask;
    ask.label->setText(klocale->translate("Rule name:"));
    ask.exec();
    int i=((KIntLineEdit *)(l->findWidget("value")))->getValue();
    if (((QCheckBox *)(l->findWidget("neg")))->isChecked())
        i*=-1;
    rule=new Rule(ask.entry->text(),
                  ((QLineEdit *)(l->findWidget("expr")))->text(),
                  (Rule::Field)((QComboBox *)(l->findWidget("field")))->currentItem(),
                  ((QCheckBox *)(l->findWidget("casesen")))->isChecked(),
                  ((QCheckBox *)(l->findWidget("wildmode")))->isChecked(),
                  i);
    rule->save(ask.entry->text());
    list2->setCurrentItem(list->currentItem());
    list->insertItem(ask.entry->text());
    list2->insertItem(ask.entry->text());
    delete rule;
}

void rulesDlg::saveRule()
{
    QString name=list->text(list->currentItem());
    int i=((KIntLineEdit *)(l->findWidget("value")))->getValue();
    if (((QCheckBox *)(l->findWidget("neg")))->isChecked())
        i*=-1;
    rule=new Rule(name,
                  ((QLineEdit *)(l->findWidget("expr")))->text(),
                  (Rule::Field)((QComboBox *)(l->findWidget("field")))->currentItem(),
                  ((QCheckBox *)(l->findWidget("casesen")))->isChecked(),
                  ((QCheckBox *)(l->findWidget("wildmode")))->isChecked(),
                  i);
    rule->save(name);
    delete rule;
}

void rulesDlg::deleteRule()
{
    QString name=list->text(list->currentItem());
    ruleFile->setGroup("Index");
    QStrList names;
    ruleFile->readListEntry("RuleNames",names);
    if (names.find(name.data())!=-1)
    {
        names.remove(name);
        ruleFile->writeEntry("RuleNames",names);
    }
    ruleFile->sync();
    list2->setCurrentItem(list->currentItem());
    list2->removeItem(list->currentItem());
    list->removeItem(list->currentItem());
}

void rulesDlg::addGlobal()
{
    debug ("adding global rule");
    QString name=list2->text(list2->currentItem());
    globalList->insertItem(list2->text(list2->currentItem()));
    globalList->setCurrentItem(globalList->count());
    ruleFile->setGroup("Index");
    QStrList names;
    ruleFile->readListEntry("GlobalRules",names);
    if (names.find(name.data())==-1)
    {
        debug ("saving rule %s",name.data());
        names.append(name.data());
        ruleFile->writeEntry("GlobalRules",names);
    }
    ruleFile->sync();
}
void rulesDlg::deleteGlobal()
{
    QString name=globalList->text(globalList->currentItem());
    globalList->removeItem(globalList->currentItem());
    ruleFile->setGroup("Index");
    QStrList names;
    ruleFile->readListEntry("GlobalRules",names);
    if (names.find(name.data())!=-1)
    {
        names.remove(name);
        ruleFile->writeEntry("GlobalRules",names);
    }
    ruleFile->sync();
}
