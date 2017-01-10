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

#include <qstring.h>

#include <kstring.h>

#include <kmsgbox.h>

#include "maindlg.h"
#include "globals.h"
#include "kgroup.h"
#include "misc.h"

#ifdef _KU_SHADOW
#include <shadow.h>
#endif

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

// This is to simplify compilation for Red Hat Linux systems, where
// gid's for regular users' private groups start at 500 <duncan@kde.org>
#ifdef KU_FIRST_USER
#define _KU_FIRST_GID KU_FIRST_USER
#else 
#define _KU_FIRST_GID 1001 
#endif

KGroup::KGroup() {
  u.setAutoDelete(TRUE);
  name.setStr("");
  pwd.setStr("*");
  gid     = 0;
}
  
KGroup::KGroup(KGroup *copy) {
  name    = copy->name;
  pwd     = copy->pwd;
  gid     = copy->gid;

  u.setAutoDelete(TRUE);
}

KGroup::~KGroup() {
  u.clear();
}

const QString &KGroup::getName() const {
  return (name);
}

const QString &KGroup::getPwd() const {
  return (pwd);
}

unsigned int KGroup::getGID() const {
  return (gid);
}

void KGroup::setName(const QString &data) {
  name.setStr(data);
}

void KGroup::setPwd(const QString &data) {
  pwd.setStr(data);
}

void KGroup::setGID(unsigned int data) {
  gid = data;
}

bool KGroup::lookup_user(const QString &name) {
  for (uint i = 0; i<u.count(); i++)
    if (name == (*u.at(i)))
      return true;
  return false;
}

void KGroup::addUser(const QString &name) {
  u.append(new QString(name));
}

bool KGroup::removeUser(const QString &name) {
  QString *q;

  for (uint i=0;i<u.count();i++)
    if ((*(q = u.at(i))) == name) {
      u.remove(q);
      return TRUE;
    }
  return FALSE;
}

uint KGroup::count() const {
  return (u.count());
}

QString KGroup::user(uint i) {
  return (*u.at(i));
}

void KGroup::clear() {
  u.clear();
}

KGroups::KGroups() {
  g_saved = 0;

  mode = 0644;
  uid = 0;
  gid = 0;

  g.setAutoDelete(TRUE);

  if (!load())
    err->display();
}

bool KGroups::load() {
  struct group *p;
  KGroup *tmpKG = 0;
  struct stat st;

  stat(GROUP_FILE, &st);
  mode = st.st_mode;
  uid = st.st_uid;
  gid = st.st_gid;

#ifdef HAVE_FGETGRENT
  FILE *fgrp = fopen(GROUP_FILE, "r");
  QString tmp;
  if (fgrp == 0) {
    ksprintf(&tmp, i18n("Error opening %s for reading"), GROUP_FILE);
    err->addMsg(tmp, STOP);
    return FALSE;
  }

  while ((p = fgetgrent(fgrp)) != NULL) {
#else
  while ((p = getgrent()) != NULL) {
#endif
    tmpKG = new KGroup();
    tmpKG->setGID(p->gr_gid);
    tmpKG->setName(p->gr_name);
    tmpKG->setPwd(p->gr_passwd);

    char *u_name;
    int i = 0;
    while ((u_name = p->gr_mem[i])!=0) {
      tmpKG->addUser(u_name);
      i++;
    }

    g.append(tmpKG);
  }

#ifdef HAVE_FGETGRENT
  fclose(fgrp);
#endif

  return TRUE;
}

bool KGroups::save() {
  FILE *grp;
  QString tmpS;
  QString tmpN;
  QString tmp;

  if (!g_saved) {
    backup(GROUP_FILE);
    g_saved = TRUE;
  }

  umask(0077);

  if ((grp = fopen(GROUP_FILE, "w")) == NULL) {
    ksprintf(&tmp, i18n("Error opening %s for writing"), GROUP_FILE);
    err->addMsg(tmp, STOP);
    return FALSE;
  }

  for (unsigned int i=0; i<g.count(); i++) {
    tmpN.setNum(g.at(i)->getGID());
    tmpS = g.at(i)->getName()+':'+g.at(i)->getPwd()+':'+tmpN+':';
    for (uint j=0; j<g.at(i)->count(); j++) {
       if (j != 0)
	 tmpS += ',';

       tmpS += g.at(i)->user(j);
    }
    tmpS += '\n';
    fputs(tmpS, grp);
  }
  fclose(grp);

  chmod(GROUP_FILE, mode);
  chown(GROUP_FILE, uid, gid);

#ifdef GRMKDB
  if (system(GRMKDB) != 0) {
    err->addMsg("Unable to build group database", STOP);
    return FALSE;
  }
#endif
  return (TRUE);
}

KGroup *KGroups::lookup(const QString &name) {
  for (uint i = 0; i<g.count(); i++)
    if (g.at(i)->getName() == name)
      return (g.at(i));
  return NULL;
}

KGroup *KGroups::lookup(unsigned int gid) {
  for (uint i = 0; i<g.count(); i++)
    if (g.at(i)->getGID() == gid)
      return (g.at(i));
  return NULL;
}

int KGroups::first_free() {
  uint i = 0;
  uint t = _KU_FIRST_GID ;

  for (t= _KU_FIRST_GID ; t<65534; t++) {
    while ((i<count()) && (group(i)->getGID() != t))
      i++;

    if (i == count())
      return (t);
  }

  err->addMsg(i18n("You have more than 65534 groups!?!? You have ran out of gid space!"), STOP);
  return (-1);
}

KGroups::~KGroups() {
  g.clear();
}

KGroup *KGroups::group(uint num) {
  return (g.at(num));
}

KGroup *KGroups::first() {
  return (g.first());
}

KGroup *KGroups::next() {
  return (g.next());
}

void KGroups::add(KGroup *ku) {
  g.append(ku);
}

void KGroups::del(KGroup *au) {
  g.remove(au);
}

uint KGroups::count() const {
	return g.count();
}
