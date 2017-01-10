/**********************************************************************

	--- Qt Architect generated file ---

	File: FilterRuleEditor.cpp
	Last generated: Mon Dec 15 18:14:27 1997

 *********************************************************************/

#include "FilterRuleEditor.h"

#include <kconfig.h>
#include <qmsgbox.h> 
#include <qregexp.h>
#include <iostream.h>

#define Inherited FilterRuleEditorData

extern KConfig *kConfig;

FilterRuleEditor::FilterRuleEditor
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  setCaption( i18n("Edit Filter Rules") );
  ApplyButton->setAutoDefault(TRUE);
  ApplyButton->setDefault(TRUE);
  updateListBox();
}


FilterRuleEditor::~FilterRuleEditor()
{
}

void FilterRuleEditor::newRule()
{
  LineTitle->setText("");
  LineSearch->setText("");
  LineFrom->setText("");
  LineTo->setText("");
  LineTitle->setFocus();
  ApplyButton->setText(i18n("&Insert"));
}

void FilterRuleEditor::OkPressed()
{
  int number, after;
  if(strcmp(ApplyButton->text(), i18n("&Insert")) == 0){
    number = kConfig->readNumEntry("Rules", 0) + 1;
    after = number - 1;
    kConfig->writeEntry("Rules", number);
  }
  else if(strcmp(ApplyButton->text(), i18n("&Modify")) == 0){
    number = RuleList->currentItem() + 1;
    after = number - 1;
  }
  else{
    return;
  }

  const char *name = LineTitle->text();
  const char *search = LineSearch->text();
  const char *from = LineFrom->text();
  const char *to = LineTo->text();
  if((strlen(name) == 0) || 
     (strlen(search) == 0) || 
     (strlen(from) == 0) ||
     (strlen(to) == 0)){
    QMessageBox::warning(this, i18n("Missing Arguments"), 
			 i18n("Cannot create a Rule since not\nall the fields are filled in."));
    
  }
  else{
    ApplyButton->setText(i18n("&Modify"));
    kConfig->setGroup("FilterRules");
    QString key;
    key.sprintf("name-%d", number);
    kConfig->writeEntry(key, name);
    key.sprintf("search-%d", number);
    kConfig->writeEntry(key, search);
    key.sprintf("from-%d", number);
    kConfig->writeEntry(key, from);
    key.sprintf("to-%d", number);
    kConfig->writeEntry(key, to);
    //    kConfig->sync();
    updateListBox(after);
  }
}

void FilterRuleEditor::updateListBox(int citem )
{
  kConfig->setGroup("FilterRules");
  int number = kConfig->readNumEntry("Rules", 0);
  RuleList->setAutoUpdate(FALSE);
  RuleList->clear();
  for(; number > 0; number--){
    QString key;
    key.sprintf("name-%d", number);
    RuleList->insertItem(kConfig->readEntry(key), 0);
  }
  if(RuleList->count() > 0)
    RuleList->setCurrentItem(citem);
  RuleList->setAutoUpdate(FALSE);
  RuleList->repaint(); 
}

void FilterRuleEditor::moveRule(int from, int to)
{
  kConfig->setGroup("FilterRules");
  QString src;
  QString dest;
  src.sprintf("name-%d", from);
  dest.sprintf("name-%d", to);
  kConfig->writeEntry(dest, convertSpecial(kConfig->readEntry(src)));
  kConfig->writeEntry(src, "", FALSE);
  src.sprintf("search-%d", from);
  dest.sprintf("search-%d", to);
  kConfig->writeEntry(dest, convertSpecial(kConfig->readEntry(src)));
  kConfig->writeEntry(src, "", FALSE);
  src.sprintf("from-%d", from);
  dest.sprintf("from-%d", to);
  kConfig->writeEntry(dest, convertSpecial(kConfig->readEntry(src)));
  kConfig->writeEntry(src, "", FALSE);
  src.sprintf("to-%d", from);
  dest.sprintf("to-%d", to);
  kConfig->writeEntry(dest, convertSpecial(kConfig->readEntry(src)));
  kConfig->writeEntry(src, "", FALSE);
}

void FilterRuleEditor::deleteRule()
{
  if(RuleList->currentItem() >= 0){
    kConfig->setGroup("FilterRules");
    int max = kConfig->readNumEntry("Rules");
    for(int i = RuleList->currentItem()+2; i <= max; i++){
      moveRule(i, i-1);
    }
    max--;
    kConfig->writeEntry("Rules", max);
    updateListBox();
  }
  else{
    //    deleteButton->setEnabled(FALSE);
  }
}

void FilterRuleEditor::closePressed()
{
  this->hide();
  delete this;
}

void FilterRuleEditor::newHighlight(int i)
{
  i++;
  kConfig->setGroup("FilterRules");
  QString key;
  key.sprintf("name-%d", i);
  LineTitle->setText(convertSpecial(kConfig->readEntry(key)));
  key.sprintf("search-%d", i);
  LineSearch->setText(convertSpecial(kConfig->readEntry(key)));
  key.sprintf("from-%d", i);
  LineFrom->setText(convertSpecial(kConfig->readEntry(key)));
  key.sprintf("to-%d", i);
  LineTo->setText(convertSpecial(kConfig->readEntry(key)));

}

void FilterRuleEditor::raiseRule()
{
  int item = RuleList->currentItem();
  kConfig->setGroup("FilterRules");
  int max = kConfig->readNumEntry("Rules");
  if(item > 0){
    moveRule(item, max+1);
    moveRule(item+1, item);
    moveRule(max+1, item+1);
    updateListBox(item - 1);
  }
}

void FilterRuleEditor::lowerRule()
{
  int item = RuleList->currentItem();
  kConfig->setGroup("FilterRules");
  int max = kConfig->readNumEntry("Rules");
  if(item < max-1){
    moveRule(item+2, max+1);
    moveRule(item+1, item+2);
    moveRule(max+1, item+1);
    updateListBox(item+1);
  }
}

QString FilterRuleEditor::convertSpecial(QString str)
{
  str.detach();
  str.replace(QRegExp("\\$"), "$$");
  return str;
}

#include "FilterRuleEditor.moc"
