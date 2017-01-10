#ifndef _KUSER_H_
#define _KUSER_H_

#include <qstring.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

#ifdef __FreeBSD__
#undef _KU_SHADOW
#endif

class KUsers;

class KUser {
public:
  KUser();
  KUser(const KUser *user);
  ~KUser();

  void copy(const KUser *user);

  const QString &getName() const;
  const QString &getPwd() const;
  const QString &getHomeDir() const;
  const QString &getShell() const;
  const QString &getFullName() const;

  unsigned int getUID() const;
  unsigned int getGID() const;

#ifdef __FreeBSD__
  const QString &getOffice() const;
  const QString &getWorkPhone() const;
  const QString &getHomePhone() const;
  const QString &getClass() const;
  time_t getLastChange() const;
  time_t getExpire() const;
#else
  const QString &getOffice1() const;
  const QString &getOffice2() const;
  const QString &getAddress() const;

#endif

#ifdef _KU_SHADOW
  const QString &getSPwd() const;
  long getLastChange() const;
  int getMin() const;
  int getMax() const;
  int getWarn() const;
  int getInactive() const;
  int getExpire() const;
  int getFlag() const;
#endif

  void setName(const QString &data);
  void setPwd(const QString &data);
  void setHomeDir(const QString &data);
  void setShell(const QString &data);
  void setFullName(const QString &data);

  void setUID(unsigned int data);
  void setGID(unsigned int data);

#ifdef __FreeBSD__
  void setOffice(const QString &data);
  void setWorkPhone(const QString &data);
  void setHomePhone(const QString &data);
  void setClass(const QString &data);
  void setExpire(time_t data);
  void setLastChange(time_t data);
#else
  void setOffice1(const QString &data);
  void setOffice2(const QString &data);
  void setAddress(const QString &data);
#endif

#ifdef _KU_SHADOW
  void setSPwd(const QString &data);
  void setLastChange(long data);
  void setMin(int data);
  void setMax(int data);
  void setWarn(int data);
  void setInactive(int data);
  void setExpire(int data);
  void setFlag(int data);
#endif

  int getCreateHome();
  int getCreateMailBox();
  int getCopySkel();

  void setCreateHome(int data);
  void setCreateMailBox(int data);
  void setCopySkel(int data);

protected:
  friend KUsers;

  void createHome();
  int createMailBox();
  int copySkel();

  int removeHome();
  int removeCrontabs();
  int removeMailBox();
  int removeProcesses();

  void copyDir(const QString &srcPath, const QString &dstPath);

  QString
    p_name,                        // parsed pw information
    p_pwd,
    p_dir,
    p_shell,
    p_fname,                        // parsed comment information
#ifdef __FreeBSD__
    p_office,
    p_ophone,
    p_hphone,
    p_class;
  time_t
    p_change,
    p_expire;
#else
    p_office1,
    p_office2,
    p_address;
#endif
  unsigned int
    p_uid,
    p_gid;

#ifdef _KU_SHADOW
  QString
    s_pwd;                         // parsed shadow information
  long
    s_lstchg;                      // last password change
  signed int
    s_min,                         // days until pwchange allowed.
    s_max,                         // days before change required 
    s_warn,                        // days warning for expiration 
    s_inact,                       // days before  account  inactive
    s_expire,                      // date when account expires 
    s_flag;                        // reserved for future use
#endif
  int
    isCreateHome,              // create homedir
    isCreateMailBox,           // create mailbox
    isCopySkel;                // copy skeleton
};

class KUsers {
public:
  KUsers();
  ~KUsers();
  KUser *lookup(const char *name);
  KUser *lookup(unsigned int uid);
  int first_free();

  bool load();

  bool save();

  KUser *first();
  KUser *next();
  uint count() const;
  KUser *user(uint num);

  void add(KUser *ku);
  void del(KUser *au);

protected:

  int p_backuped;
  int s_backuped;

  int pwd_mode;
  int sdw_mode;

  uid_t pwd_uid;
  gid_t pwd_gid;

  uid_t sdw_uid;
  gid_t sdw_gid;

  QList<KUser> u;
  QList<KUser> du;

  bool doCreate();
  bool doDelete();
  void fillGecos(KUser *user, const char *gecos);
  bool loadpwd();
  bool loadsdw();

  bool savepwd();
  bool savesdw();
};

#endif // _KUSER_H_

