#ifndef _KGROUP_H_
#define _KGROUP_H_

#include <qstring.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

class KGroup {
public:
  KGroup();
  KGroup(KGroup *copy);
  ~KGroup();

  const QString &getName() const;
  const QString &getPwd() const;
  unsigned int getGID() const;

  void setName(const QString &data);
  void setPwd(const QString &data);
  void setGID(unsigned int data);

  void addUser(const QString &name);
  bool removeUser(const QString &name);
  bool lookup_user(const QString &name);
  uint count() const;
  QString user(uint i);
  void clear();

protected:
  QString
    name,
    pwd;
  unsigned int
    gid;

  QList<QString> u;
};

class KGroups {
public:
  KGroups();
  ~KGroups();

  KGroup *lookup(const QString &name);
  KGroup *lookup(unsigned int gid);
  int first_free();

  bool save();
  bool load();

  KGroup *first();
  KGroup *next();
  KGroup *group(uint num);

  void add(KGroup *ku);
  void del(KGroup *au);

	uint count() const;

protected:
  int g_saved;
  QList<KGroup> g;

  int mode;
  int uid;
  int gid;
};

#endif // _KGROUP_H_

