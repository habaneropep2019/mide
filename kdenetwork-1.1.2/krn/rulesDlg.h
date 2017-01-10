#ifndef RULES_DLG_H
#define RULES_DLG_H

#include <qdialog.h>
#include "rules.h"

class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QListBox;
class KTypeLayout;
class KIntLineEdit;
class rulesDlg : public QDialog
{
    Q_OBJECT
        
public:
    
    rulesDlg();
    
    virtual ~rulesDlg();
    Rule *rule;
    QListBox *list;
    QListBox *list2;
    QListBox *globalList;
    KIntLineEdit *gvalue;

public slots:
    void editRule(const char *name);
    void saveRuleAs();
    void saveRule();
    void deleteRule();
    void addGlobal();
    void deleteGlobal();
private:
    KTypeLayout *l;
};

#endif