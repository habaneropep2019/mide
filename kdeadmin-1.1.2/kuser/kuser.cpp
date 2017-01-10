#include "globals.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#ifdef _SCO_DS
#include <sys/fcntl.h>
#endif
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <qstring.h>
#include <qdir.h>

#include <kmsgbox.h>
#include <kstring.h>

#include "maindlg.h"
#include "kuser.h"
#include "misc.h"

#ifdef _KU_SHADOW
#include <shadow.h>
#endif

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

// This is to simplify compilation for Red Hat Linux systems, where
// uid's for regular users start at 500 <duncan@kde.org>
#ifdef KU_FIRST_USER
#define _KU_FIRST_UID KU_FIRST_USER
#else 
#define _KU_FIRST_UID 1001 
#endif

// class KUser

KUser::KUser() {
  p_name.setStr("");
  p_pwd.setStr("*");
  p_dir.setStr("");
  p_shell.setStr("");
  p_fname.setStr("");
#ifdef __FreeBSD__
  p_office.setStr("");
  p_ophone.setStr("");
  p_hphone.setStr("");
  p_class.setStr("");
  p_change = 0;
  p_expire = 0;
#else
  p_office1.setStr("");
  p_office2.setStr("");
  p_address.setStr("");
#endif
  p_uid     = 0;
  p_gid     = 100;

#ifdef _KU_SHADOW   
  s_pwd.setStr("");
  s_lstchg  = 0;
  s_min     = 0;
  s_max     = -1;
  s_warn    = 7;
  s_inact   = -1;
  s_expire  = 99999;
  s_flag    = 0;
#endif

  isCreateHome = 0;
  isCreateMailBox = 0;
  isCopySkel = 0;
}
  
KUser::KUser(const KUser *user) {
  copy(user);
}

void KUser::copy(const KUser *user) {
  p_name = user->p_name;
  p_pwd = user->p_pwd;
  p_dir = user->p_dir;
  p_shell = user->p_shell;
  p_fname = user->p_fname;
#ifdef __FreeBSD__
  p_office = user->p_office;
  p_ophone = user->p_ophone;
  p_hphone = user->p_hphone;
  p_class = user->p_class;
  p_change = user->p_change;
  p_expire = user->p_expire;
#else
  p_office1 = user->p_office1;
  p_office2 = user->p_office2;
  p_address = user->p_address;
#endif
  p_uid     = user->p_uid;
  p_gid     = user->p_gid;

#ifdef _KU_SHADOW   
  s_pwd = user->s_pwd;
  s_lstchg  = user->s_lstchg;
  s_min     = user->s_min;
  s_max     = user->s_max;
  s_warn    = user->s_warn;
  s_inact   = user->s_inact;
  s_expire  = user->s_expire;
  s_flag    = user->s_flag;
#endif

  isCreateHome = user->isCreateHome;
  isCreateMailBox = user->isCreateMailBox;
  isCopySkel = user->isCopySkel;
}
  
KUser::~KUser() {
}

int KUser::getCreateHome() {
  return isCreateHome;
}

int KUser::getCreateMailBox() {
  return isCreateMailBox;
}

int KUser::getCopySkel() {
  return isCopySkel;
}

const QString &KUser::getName() const {
  return (p_name);
}

const QString &KUser::getPwd() const {
  return (p_pwd);
}

const QString &KUser::getHomeDir() const {
  return (p_dir);
}

const QString &KUser::getShell() const {
  return (p_shell);
}

const QString &KUser::getFullName() const {
  return (p_fname);
}

#ifdef __FreeBSD__
// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
const QString &KUser::getOffice() const {
  return (p_office);
}

const QString &KUser::getWorkPhone() const {
  return (p_ophone);
}

const QString &KUser::getHomePhone() const {
  return (p_hphone);
}

// New fields needed for the FreeBSD /etc/master.passwd file
const QString &KUser::getClass() const {
  return (p_class);
}

time_t KUser::getLastChange() const {
  return (p_change);
}

time_t KUser::getExpire() const {
  return (p_expire);
}

#else

const QString &KUser::getOffice1() const {
  return (p_office1);
}

const QString &KUser::getOffice2() const {
  return (p_office2);
}

const QString &KUser::getAddress() const {
  return (p_address);
}

#endif

unsigned int KUser::getUID() const {
  return (p_uid);
}

unsigned int KUser::getGID() const {
  return (p_gid);
}

#ifdef _KU_SHADOW
const QString &KUser::getSPwd() const {
  return (s_pwd);
}

long KUser::getLastChange() const {
  return(s_lstchg);
}

int KUser::getMin() const {
  return (s_min);
}

int KUser::getMax() const {
  return (s_max);
}

int KUser::getWarn() const {
  return (s_warn);
}

int KUser::getInactive() const {
  return (s_inact);
}

int KUser::getExpire() const {
  return (s_expire);
}

int KUser::getFlag() const {
  return (s_flag);
}
#endif

void KUser::setName(const QString &data) {
  p_name.setStr(data);
}

void KUser::setPwd(const QString &data) {
  p_pwd.setStr(data);
}

void KUser::setHomeDir(const QString &data) {
  p_dir.setStr(data);
}

void KUser::setShell(const QString &data) {
  p_shell.setStr(data);
}

void KUser::setFullName(const QString &data) {
  p_fname.setStr(data);
}

#ifdef __FreeBSD__
// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
void KUser::setOffice(const QString &data) {
  p_office.setStr(data);
}

void KUser::setWorkPhone(const QString &data) {
  p_ophone.setStr(data);
}

void KUser::setHomePhone(const QString &data) {
  p_hphone.setStr(data);
}

// New fields needed for the FreeBSD /etc/master.passwd file
void KUser::setClass(const QString &data) {
  p_class.setStr(data);
}

void KUser::setLastChange(time_t data) {
  p_change = data;
}

void KUser::setExpire(time_t data) {
  p_expire = data;
}

#else

void KUser::setOffice1(const QString &data) {
  p_office1.setStr(data);
}

void KUser::setOffice2(const QString &data) {
  p_office2.setStr(data);
}

void KUser::setAddress(const QString &data) {
  p_address.setStr(data);
}

#endif

void KUser::setUID(unsigned int data) {
  p_uid = data;
}

void KUser::setGID(unsigned int data) {
  p_gid = data;
}

#ifdef _KU_SHADOW
void KUser::setSPwd(const QString &data) {
  s_pwd.setStr(data);
}

void KUser::setLastChange(long data) {
  s_lstchg = data;
}

void KUser::setMin(int data) {
  s_min = data;
}

void KUser::setMax(int data) {
  s_max = data;
}

void KUser::setWarn(int data) {
  s_warn = data;
}

void KUser::setInactive(int data) {
  s_inact = data;
}

void KUser::setExpire(int data) {
  s_expire = data;
}

void KUser::setFlag(int data) {
  s_flag = data;
}

#endif

void KUser::setCreateHome(int data) {
  isCreateHome = data;
}

void KUser::setCreateMailBox(int data) {
  isCreateMailBox = data;
}

void KUser::setCopySkel(int data) {
  isCopySkel = data;
}

KUsers::KUsers() {
  p_backuped = 0;
  s_backuped = 0;

  pwd_mode = 0644;
  pwd_uid = 0;
  pwd_gid = 0;

  sdw_mode = 0600;
  sdw_uid = 0;
  sdw_gid = 0;

  u.setAutoDelete(TRUE);
  du.setAutoDelete(TRUE);

  if (!load())
    err->display();
}

void KUsers::fillGecos(KUser *user, const char *gecos) {
  int no = 0;
  const char *s = gecos;
  const char *pos = NULL;
  // At least one part of the string exists
  for(;;) {
    pos = strchr(s, ',');
    char val[200];
    if(pos == NULL)
      strcpy(val, s);
    else {
      strncpy(val, s, (int)(pos-s));
      val[(int)(pos-s)] = 0;
    }

    switch(no) {
      case 0: user->setFullName(val); break;
#ifdef __FreeBSD__
      case 1: user->setOffice(val); break;
      case 2: user->setWorkPhone(val); break;
      case 3: user->setHomePhone(val); break;
#else
      case 1: user->setOffice1(val); break;
      case 2: user->setOffice2(val); break;
      case 3: user->setAddress(val); break;
#endif
    }
    if(pos == NULL) break;
    s = pos+1;
    no++;
  }
}

bool KUsers::load() {
  if (!loadpwd())
    return FALSE;

  if (!loadsdw())
    return FALSE;

  return TRUE;
}

// Load passwd file

bool KUsers::loadpwd() {
  passwd *p;
  KUser *tmpKU = 0;

  // Start reading passwd file

  struct stat st;

  stat(PASSWORD_FILE, &st);
  pwd_mode = st.st_mode & 0666;
  pwd_uid = st.st_uid;
  pwd_gid = st.st_gid;

  // We are reading our PASSWORD_FILE
  QString tmp;
#ifdef HAVE_FGETPWENT
  FILE *fpwd = fopen(PASSWORD_FILE, "r");
  if(fpwd == NULL) {
     ksprintf(&tmp, i18n("Error opening %s for reading"), PASSWORD_FILE);
     err->addMsg(tmp, STOP);
     return FALSE;
  }

  while ((p = fgetpwent(fpwd)) != NULL) {
#else
  while ((p = getpwent()) != NULL) {
#endif

#ifdef _KU_QUOTA
    quotas->addQuota(p->pw_uid);
#endif
    tmpKU = new KUser();
    tmpKU->setUID(p->pw_uid);
    tmpKU->setGID(p->pw_gid);
    tmpKU->setName(p->pw_name);
    tmpKU->setPwd(p->pw_passwd);
    tmpKU->setHomeDir(p->pw_dir);
    tmpKU->setShell(p->pw_shell);
#ifdef __FreeBSD__
    tmpKU->setClass(p->pw_class);
    tmpKU->setLastChange(p->pw_change);
    tmpKU->setExpire(p->pw_expire);
#endif

    if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
      fillGecos(tmpKU, p->pw_gecos);

    u.append(tmpKU);
  }

  // End reading passwd file

#ifdef HAVE_FGETPWENT
  fclose(fpwd);
#endif

  return (TRUE);
}

// Load shadow passwords

bool KUsers::loadsdw() {
#ifdef _KU_SHADOW
  QString tmp;
  struct spwd *spw;
  KUser *up = NULL;
  FILE *f;

  struct stat st;

  if (!is_shadow)
    return TRUE;

  stat(SHADOW_FILE, &st);
  sdw_mode = st.st_mode & 0666;
  sdw_uid = st.st_uid;
  sdw_gid = st.st_gid;

  if ((f = fopen(SHADOW_FILE, "r")) == NULL) {
    is_shadow = 0;
    printf("Shadow file missing detected\n");
    return TRUE;
  }

  fclose(f);

  setspent();

  while ((spw = getspent())) {     // read a shadow password structure
    if ((up = lookup(spw->sp_namp)) == NULL) {
      ksprintf(&tmp, i18n("No /etc/passwd entry for %s.\nEntry will be removed at the next `Save'-operation."),
                  spw->sp_namp);
      err->addMsg(tmp, STOP);
      err->display();
      continue;
    }

    up->setSPwd(spw->sp_pwdp);        // cp the encrypted pwd
    up->setLastChange(spw->sp_lstchg);
    up->setMin(spw->sp_min);
    up->setMax(spw->sp_max);
#ifndef _SCO_DS
    up->setWarn(spw->sp_warn);
    up->setInactive(spw->sp_inact);
    up->setExpire(spw->sp_expire);
    up->setFlag(spw->sp_flag);
#endif
  }

  endspent();
#endif // _KU_SHADOW
  return TRUE;
}

bool KUsers::save() {
  if (!savepwd())
    return FALSE;

  if (!savesdw())
    return FALSE;

  if (!doDelete())
    return FALSE;

  if (!doCreate())
    return FALSE;

  return TRUE;
}

bool KUsers::doCreate() {
  for (unsigned int i=0; i<u.count(); i++) {
    KUser *user = u.at(i);

    if(user->getCreateMailBox()) {
      user->createMailBox();
      user->setCreateMailBox(0);
    }

    if(user->getCreateHome()) {
       user->createHome();
       user->setCreateHome(0);
    }

    if(user->getCopySkel()) {
       user->copySkel();
       user->setCopySkel(0);
    }
  }

  return TRUE;
}

bool KUsers::doDelete() {
  for (unsigned int i=0; i<du.count(); i++) {
    KUser *user = du.at(i);

    user->removeHome();
    user->removeCrontabs();
    user->removeMailBox();
    user->removeProcesses();

    du.remove(i);
  }

  return TRUE;
}

// Save password file

bool KUsers::savepwd() {
  FILE *passwd;
  QString s;
  QString s1;

  QString tmp;

  if (!p_backuped) {
    backup(PASSWORD_FILE);
    p_backuped = TRUE;
  }

  umask(0077);

  if ((passwd = fopen(PASSWORD_FILE,"w")) == NULL) {
    ksprintf(&tmp, i18n("Error opening %s for writing"), PASSWORD_FILE);
    err->addMsg(tmp, STOP);
    return FALSE;
  }

  for (unsigned int i=0; i<u.count(); i++) {
    KUser *user = u.at(i);

#ifdef __FreeBSD__
    ksprintf(&s, "%s:%s:%i:%i:%s:%i:%i:", (const char *)user->getName(),
             (const char *)user->getPwd(), user->getUID(),
             user->getGID(), (const char *)user->getClass(),
             user->getLastChange(), user->getExpire());

    ksprintf(&s1, "%s,%s,%s,%s", (const char *)user->getFullName(),
              (const char *)user->getOffice(),
              (const char *)user->getWorkPhone(),
              (const char *)user->getHomePhone());
#else

    ksprintf(&s, "%s:%s:%i:%i:",  (const char *)user->getName(),
             (const char *)user->getPwd(), (const char *)user->getUID(),
             (const char *)user->getGID());

    ksprintf(&s1, "%s,%s,%s,%s", (const char *)user->getFullName(), (const char *)user->getOffice1(),
	           (const char *)user->getOffice2(), (const char *)user->getAddress());

#endif

    for (int j=(s1.length()-1); j>=0; j--) {
      if (s1[j] != ',')
        break;

      s1.truncate(j);
    }

    s+=s1+":"+user->getHomeDir()+":"+user->getShell()+"\n";
    fputs((const char *)s, passwd);
  }
  fclose(passwd);

  chmod(PASSWORD_FILE, pwd_mode);
  chown(PASSWORD_FILE, pwd_uid, pwd_gid);

#ifdef PWMKDB
  // need to run a utility program to build /etc/passwd, /etc/pwd.db
  // and /etc/spwd.db from /etc/master.passwd
  if (system(PWMKDB) != 0) {
     ksprintf(&tmp, i18n("Unable to build password database"));
     err->addMsg(tmp, STOP);
     return FALSE;
  }
#endif

  return TRUE;
}

// Save shadow passwords file

bool KUsers::savesdw() {
#ifdef _KU_SHADOW
  QString tmp;
  FILE *f;
  struct spwd *spwp;
  struct spwd s;
  KUser *up;

  if (!is_shadow)
    return TRUE;

  if (!s_backuped) {
    backup(SHADOW_FILE);
    s_backuped = TRUE;
  }

  umask(0077);

  if ((f = fopen(SHADOW_FILE, "w")) == NULL) {
    ksprintf(&tmp, i18n("Error opening %s for writing"), SHADOW_FILE);
    err->addMsg(tmp, STOP);
    return FALSE;
  }

  s.sp_namp = (char *)malloc(200);
  s.sp_pwdp = (char *)malloc(200);
    
  for (uint index = 0; index < u.count(); index++) {
    up = u.at(index);
    if (!(const char *)up->getSPwd()) {
      ksprintf(&tmp, i18n("No shadow entry for %s."), (const char *)up->getName());
      err->addMsg(tmp, STOP);
      continue;
    }

    strncpy(s.sp_namp, (const char *)up->getName(), 200);
    strncpy(s.sp_pwdp, (const char *)up->getSPwd(), 200);
    s.sp_lstchg = up->getLastChange();
    s.sp_min    = up->getMin();
    s.sp_max    = up->getMax();
#ifndef _SCO_DS
    s.sp_warn   = up->getWarn();
    s.sp_inact  = up->getInactive();
    s.sp_expire = up->getExpire();
    s.sp_flag   = up->getFlag();
#endif
    spwp = &s;
    putspent(spwp, f);
  }
  fclose(f);

  chmod(SHADOW_FILE, sdw_mode);
  chown(SHADOW_FILE, sdw_uid, sdw_gid);

  free(s.sp_namp);
  free(s.sp_pwdp);
#endif // _KU_SHADOW
  return TRUE;
}

KUser *KUsers::lookup(const char *name) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getName() == name)
      return (u.at(i));
  return NULL;
}

KUser *KUsers::lookup(unsigned int uid) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getUID() == uid)
      return (u.at(i));
  return NULL;
}

int KUsers::first_free() {
  uint i = 0;
  uint t = _KU_FIRST_UID ;

  for (t = _KU_FIRST_UID ; t<65534; t++) {
    while ((i<u.count()) && (u.at(i)->getUID() != t))
      i++;

    if (i == u.count())
      return (t);
    i = 0;
  }

  err->addMsg(i18n("You have more than 65534 users!?!? You have ran out of uid space!"), STOP);
  return (-1);
}

KUsers::~KUsers() {
  u.clear();
  du.clear();
}

uint KUsers::count() const {
  return (u.count());
}

KUser *KUsers::user(uint num) {
  return (u.at(num));
}

KUser *KUsers::first() {
  return (u.first());
}

KUser *KUsers::next() {
  return (u.next());
}

void KUsers::add(KUser *ku) {
  u.append(ku);
}

void KUsers::del(KUser *au) {
  KUser *nu = new KUser(au);
  du.append(nu);
  u.remove(au);
}

void KUser::createHome() {
  QDir d = QDir::root();

  if (d.cd(p_dir)) {
    QString tmp;
    ksprintf(&tmp, i18n("Directory %s already exists"), (const char *)p_dir);
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (mkdir((const char *)p_dir, 0700) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot create home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (chown((const char *)p_dir, p_uid, p_gid) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot change owner of home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (chmod((const char *)p_dir, KU_HOMEDIR_PERM) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot change permissions on home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }
}

int KUser::createMailBox() {
  QString mailboxpath;
  int fd;
  ksprintf(&mailboxpath, "%s/%s", MAIL_SPOOL_DIR, (const char *)p_name);
  if((fd = open((const char *)mailboxpath, O_CREAT|O_EXCL|O_WRONLY,
                S_IRUSR|S_IWUSR)) < 0) {
    QString tmp;
    ksprintf(&tmp, "Cannot create %s: %s", (const char *)mailboxpath,
             strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
    return -1;
  }

  close(fd);

  if (chown((const char *)mailboxpath, p_uid, KU_MAILBOX_GID) != 0) {
    QString tmp;
    ksprintf(&tmp, "Cannot change owner on mailbox: %s\nError: %s",
             (const char *)mailboxpath, strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
    return -1;
  }

  if (chmod((const char *)mailboxpath, KU_MAILBOX_PERM) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot change permissions on mailbox: %s\nError: %s"),
             (const char *)mailboxpath, strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  return 0;
}

void KUser::copyDir(const QString &srcPath, const QString &dstPath) {
  struct stat st;
  QDir s(srcPath);
  QDir d(dstPath);
  QString prefix(SKEL_FILE_PREFIX);
  int len = prefix.length();

  s.setFilter(QDir::Dirs);

  for (uint i=0; i<s.count(); i++) {
    QString name(s[i]);

    if (name == ".")
      continue;
    if (name == "..")
      continue;

    QString filename(s.filePath(name));
    QDir dir(filename);

    if (stat(filename, &st) != 0)
      printf("errno = %d, '%s'\n", errno, strerror(errno));

    if (name.left(len) == prefix)
      name = name.remove(0, len);

    d.mkdir(name, FALSE);
    if (chown(d.filePath(name), p_uid, p_gid) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot change owner of directory %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }

    if (chmod(d.filePath(name), st.st_mode & 07777) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot change permissions on directory %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }

    copyDir(s.filePath(name), d.filePath(name));
  }

  s.setFilter(QDir::Files | QDir::Hidden);

  for (uint i=0; i<s.count(); i++) {
    QString name(s[i]);

    QString filename(s.filePath(name));

    stat(filename, &st);

    if (name.left(len) == prefix)
      name = name.remove(0, len);

    if (copyFile(filename, d.filePath(name)) == -1) {
      err->display();
      continue;
    }

    if (chown(d.filePath(name), p_uid, p_gid) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot change owner of file %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }

    if (chmod(d.filePath(name), st.st_mode & 07777) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot change permissions on file %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }
  }
}

int KUser::copySkel() {
  QDir s(SKELDIR);
  QDir d(p_dir);

  umask(0777);

  if (!s.exists()) {
    QString tmp;
    ksprintf(&tmp, i18n("Directory %s does not exist"), (const char *)s.dirName());
    err->addMsg(tmp, STOP);
    err->display();
    return (-1);
  }

  if (!d.exists()) {
    QString tmp;
    ksprintf(&tmp, i18n("Directory %s does not exist"), (const char *)d.dirName());    err->addMsg(tmp, STOP);
    err->display();
    return (-1);
  }

  copyDir(s.absPath(), d.absPath());

  return 0;
}

// Temporarily use rm
// TODO: replace by our own procedure cause calling other programs
//       for things we are know how to do is not a good idea

int KUser::removeHome() {
  struct stat sb;
  QString command;

  if (!stat((const char *)p_dir, &sb))
    if (S_ISDIR(sb.st_mode) && sb.st_uid == p_uid) {
#ifdef MINIX
      ksprintf(&command, "/usr/bin/rm -rf -- %s", (const char *)p_dir);
#else
      ksprintf(&command, "/bin/rm -rf -- %s", (const char *)p_dir);
#endif
    if (system((const char *)command) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot remove home directory %s\nError: %s"),
               (const char *)command, strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
     }
   }

  return 0;
}

// TODO: remove at jobs too.

int KUser::removeCrontabs() {
  QString file;
  QString command;

  ksprintf(&file, "/var/cron/tabs/%s",
           (const char*)p_name);
  if (access((const char *)file, F_OK) == 0) {
  	ksprintf(&command, "crontab -u %s -r",
             (const char*)p_name);
    if (system((const char *)command) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot remove crontab %s\nError: %s"),
               (const char *)command, strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
     }
  }

  return 0;
}

int KUser::removeMailBox() {
  QString file;

  ksprintf(&file, "%s/%s", MAIL_SPOOL_DIR,
           (const char*)p_name);
  if (remove((const char *)file) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot remove mailbox %s\nError: %s"),
             (const char *)file, strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  return 0;
}

int KUser::removeProcesses() {
  // be paranoid -- kill all processes owned by that user, if not root.

  if (p_uid != 0)
    switch (fork()) {
      case 0:
        setuid(p_uid);
        kill(-1, 9);
        _exit(0);
        break;
      case -1:
        err->addMsg(i18n("Cannot fork()"), STOP);
        err->display();
        perror("fork");
        break;
    }

  return 0;
}

