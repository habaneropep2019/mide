#include "globals.h"
#include <kmsgbox.h>
#include <kstring.h>
#include <kbuttonbox.h>
#include <qlayout.h>
#include <qvalidator.h>
#include "grpnamedlg.h"
#include "grpnamedlg.moc"
#include "misc.h"
#include "maindlg.h"

grpnamedlg::grpnamedlg(KGroup *agroup, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE) {
  group = agroup;
  group->setGID(groups->first_free());

  setCaption(i18n("Add group"));

  QVBoxLayout *layout = new QVBoxLayout(this, 10);
  QGridLayout *grid = new QGridLayout(2, 2);
  layout->addLayout(grid);

  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setText(i18n("Group name:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb1, 0, 0, AlignRight);

  QLabel* lb2 = new QLabel(this, "lb2");
  lb2->setText(i18n("Group number:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb2, 1, 0, AlignRight);

  legrpname = new QLineEdit( this, "LineEdit_1" );

  // ensure it fits at least 20 characters
  legrpname->setText( "XXXXXXXXXXXXXXXXXXX" );
  legrpname->setMinimumSize( legrpname->sizeHint() );

  // clear text
  legrpname->setText( "" );
  legrpname->setFocus();
  grid->addWidget(legrpname, 0, 1);

  legid = new QLineEdit( this, "LineEdit_2" );

  // ensure it fits at least 20 characters
  legid->setText( "XXXXXXXXXXXXXXXXXXX" );
  legid->setMinimumSize( legid->sizeHint() );
  
  QString s;
  s.setNum(group->getGID());
  
  // clear text
  legid->setText(s);
  legid->setValidator(new QIntValidator(this, "val1"));

  grid->addWidget(legid, 1, 1);
  
  // add a button box
  KButtonBox *bbox = new KButtonBox(this);

  // make buttons right aligned
  bbox->addStretch(1);

  // the default buttons  
  pbOk = bbox->addButton(i18n("OK"));
  pbCancel = bbox->addButton(i18n("Cancel"));
  pbOk->setDefault(TRUE);
  
  // establish callbacks
  QObject::connect(pbOk, SIGNAL(clicked()), 
		   this, SLOT(ok()));
  QObject::connect(pbCancel, SIGNAL(clicked()), 
		   this, SLOT(cancel()));

  bbox->layout();
  bbox->setMinimumSize(bbox->sizeHint());
  
  layout->addWidget(bbox);
  layout->freeze();
}

grpnamedlg::~grpnamedlg() {
  delete legrpname;
  delete pbOk;
  delete pbCancel;
}

void grpnamedlg::ok()
{
  QString tmp;
  QString s;
  s.setStr(legid->text());

  if (groups->lookup(legrpname->text()) != NULL) {
    ksprintf(&tmp, i18n("Group with name %s already exists."), legrpname->text());
    err->addMsg(tmp, STOP);
    err->display();
    return;
  }
  
  if (groups->lookup(s.toInt()) != NULL) {
    ksprintf(&tmp, i18n("Group with gid %d already exists."), s.toInt());
    err->addMsg(tmp, STOP);
    err->display();
    return;
  }
  
  group->setName(legrpname->text());
  group->setGID(s.toInt());
  accept();
}

void grpnamedlg::cancel() {
  reject();
}
