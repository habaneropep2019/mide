#include "globals.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <kmsgbox.h>
#include <kbuttonbox.h>

#include "pwddlg.h"
#include "pwddlg.moc"
#include "misc.h"
#include "globals.h"

pwddlg::pwddlg(KUser *auser, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE)
{
  user = auser;

  setCaption(i18n("Enter password"));

  layout = new QVBoxLayout(this, 10);
  QGridLayout *grid = new QGridLayout(2, 2);
  layout->addLayout(grid);

  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setText(i18n("Password"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb1, 0, 0, AlignRight);

  leusername1 = new QLineEdit( this, "LineEdit_1" );

  // ensure it fits at least 12 characters
  leusername1->setText( "XXXXXXXXXXXX" );
  leusername1->setMinimumSize( leusername1->sizeHint() );

  // clear text
  leusername1->setText( "" );
  leusername1->setFocus();
  leusername1->setEchoMode( QLineEdit::Password );
  grid->addWidget(leusername1, 0, 1);

  QLabel* lb2 = new QLabel(this, "lb2");
  lb2->setText(i18n("Retype password"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb2, 1, 0, AlignRight);

  leusername2 = new QLineEdit( this, "LineEdit_2" );

  // ensure it fits at least 12 characters
  leusername2->setText( "XXXXXXXXXXXX" );
  leusername2->setMinimumSize( leusername2->sizeHint() );

  // clear text
  leusername2->setText( "" );
  leusername2->setEchoMode( QLineEdit::Password );
  grid->addWidget(leusername2, 1, 1);

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

pwddlg::~pwddlg() {
  delete leusername1;
  delete leusername2;
  delete pbOk;
  delete pbCancel;
  delete layout;
}

void pwddlg::ok()
{
  char salt[3];
  char tmp[128];

  const char * set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

  if (strcmp(leusername1->text(), leusername2->text())) {
    err->addMsg(i18n("Passwords are not identical.\nTry again"), STOP);
    err->display();
    leusername1->setText("");
    leusername2->setText("");
    leusername1->setFocus();
  }
  else {
    srand(time(NULL));
    salt[0] = set[getpid() % strlen(set)];
    salt[1] = set[rand() % strlen(set)];
    salt[2] = 0;

    strcpy(tmp, crypt(leusername1->text(), salt));

#ifdef _KU_SHADOW
    if (is_shadow != 0) {
      user->setSPwd(tmp);
      user->setPwd("x");
    }
    else
#endif
      user->setPwd(tmp);
    accept();
  }
}

void pwddlg::cancel()
{
  reject();
}
