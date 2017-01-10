#ifndef _KU_MNT_H_
#define _KU_MNT_H_

#include <qstring.h>
#include <qlist.h>

class MntEnt {
public:
  MntEnt() {
  }
  
  MntEnt(const char *afsname, const char *adir,
         const char *atype, const char *aopts,
         const char *aquotafilename);
  ~MntEnt();

  QString getfsname();
  QString getdir();
  QString gettype();
  QString getopts();
  QString getquotafilename();

  void setfsname(const char *data);
  void setdir(const char *data);
  void settype(const char *data);
  void setopts(const char *data);
  void setquotafilename(const char *data);

public:
  QString fsname;
  QString dir;
  QString type;
  QString opts;
  QString quotafilename;
};

class Mounts {
public:
  Mounts();
  ~Mounts();

  MntEnt *getMount(uint num);
  uint getMountsNumber();
protected:
  QList<MntEnt> m;
};

#endif // _KU_MNT_H_
