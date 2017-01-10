#include <kmsgbox.h>
#include <kiconloader.h>
#include <kapp.h>
#include "kerror.h"
#include <kconfig.h>
#include <ktopwidget.h>
#include <qfont.h>
#include <unistd.h>
#include "globals.h"
#include "misc.h"
#include "maindlg.h"
#include "mainWidget.h"

char tmp[1024];
KConfig *config;
KError *err;
bool changed = false;

#ifdef _KU_QUOTA
int is_quota = 1;
#else
int is_quota = 0;
#endif

#ifdef _KU_SHADOW
int is_shadow = 1;
#else
int is_shadow = 0;
#endif

void initmain() {
  config = kapp->getConfig();
  err = new KError();
}

void donemain() {
  delete err;
}

mainDlg *md = NULL;

int main(int argc, char **argv) {
  mainWidget *mw = NULL;

  KApplication a(argc, argv, "kuser");

  initmain();

  if (getuid()) {
    err->addMsg(i18n("Only root is allowed to manage users."), STOP);
    err->display();
    exit(1);
  }

  mw = new mainWidget("kuser");
  
  a.setMainWidget(mw);
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  donemain();
}
