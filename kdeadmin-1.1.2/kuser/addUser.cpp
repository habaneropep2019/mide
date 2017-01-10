#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <qdir.h>

#include <kmsgbox.h>
#include <kstring.h>
#include <kerror.h>

#include "globals.h"
#include "maindlg.h"
#include "misc.h"
#include "addUser.moc"

#ifdef _KU_QUOTA
addUser::addUser(KUser *auser, Quota *aquota, QWidget *parent, const char *name, int isprep) :
  propdlg(auser, aquota, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);
  createhome->setChecked(true);
  connect(createhome, SIGNAL(toggled(bool)), this, SLOT(createHomeChecked(bool)));

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
  copyskel->setEnabled(FALSE);

  userPrivateGroup = new QCheckBox(w1, "userPrivateGroup");
  userPrivateGroup->setText(i18n("User Private Group"));
  userPrivateGroup->setGeometry(200, 150, 200, 30);
  connect(userPrivateGroup, SIGNAL(toggled(bool)), this, SLOT(userPrivateGroupChecked(bool)));
}
#else
addUser::addUser(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false) :
  propdlg(auser, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);
  createhome->setChecked(true);
  connect(createhome, SIGNAL(toggled(bool)), this, SLOT(createHomeChecked(bool)));

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
  copyskel->setEnabled(FALSE);

  userPrivateGroup = new QCheckBox(w1, "userPrivateGroup");
  userPrivateGroup->setText(i18n("User Private Group"));
  userPrivateGroup->setGeometry(200, 150, 200, 30);
  connect(userPrivateGroup, SIGNAL(toggled(bool)), this, SLOT(userPrivateGroupChecked(bool)));
}
#endif

void addUser::setUserPrivateGroup(bool data) {
  userPrivateGroup->setChecked(data);
}

bool addUser::getUserPrivateGroup() {
  return userPrivateGroup->isChecked();
}

void addUser::setCreateHomeDir(bool data) {
  createhome->setChecked(data);
  copyskel->setEnabled(data);
}

void addUser::setCopySkel(bool data) {
  copyskel->setChecked(data);
}

void addUser::ok() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();
  
  if (users->lookup(newuid) != NULL) {
    ksprintf(&tmp, i18n("User with UID %u already exists"), newuid);
    err->addMsg(tmp, STOP);
    err->display();
    return;
  }

  check();
  
  if (createhome->isChecked()) {
    if (checkHome())
      user->setCreateHome(1);

    if (checkMailBox())
      user->setCreateMailBox(1);
  }

  if (copyskel->isChecked())
    user->setCopySkel(1);

  accept();
}

void addUser::userPrivateGroupChecked(bool data) {
  cbpgrp->setEnabled(!data);
}

void addUser::createHomeChecked(bool data) {
  copyskel->setEnabled(data);
}

bool addUser::checkHome() {
  struct stat s;
  int r;
  QString tmp;

  r = stat(user->getHomeDir(), &s);

  if ((r == -1) && (errno = ENOENT))
    return true;

  if (r == 0)
    if (S_ISDIR(s.st_mode))
      ksprintf(&tmp, i18n("Directory %s already exists (uid = %d, gid = %d)"), 
           (const char *)user->getHomeDir(), s.st_uid, s.st_gid);
    else
      ksprintf(&tmp, i18n("%s is not a directory") ,(const char *)user->getHomeDir());
  else
    ksprintf(&tmp, "checkHome: stat: %s ", strerror(errno));
  
  err->addMsg(tmp, STOP);
  err->display();

  return false;
}

bool addUser::checkMailBox() {
  QString mailboxpath;
  QString tmp;

  struct stat s;
  int r;

  ksprintf(&mailboxpath, "%s/%s", MAIL_SPOOL_DIR,
           (const char *)user->getName());
  r = stat(mailboxpath, &s);
  
  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0)
    if (S_ISREG(s.st_mode))
      ksprintf(&tmp, i18n("Mailbox %s already exist (uid=%d)"),
               (const char *)mailboxpath, s.st_uid);
    else
      ksprintf(&tmp, i18n("%s exists but is not a regular file"),
               (const char *)mailboxpath);
  else
    ksprintf(&tmp, "checkMail: stat: %s ", strerror(errno));
  
  err->addMsg(tmp, STOP);
  err->display();

  return false;
}

