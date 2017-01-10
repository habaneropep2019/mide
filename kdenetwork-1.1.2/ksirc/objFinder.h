#ifndef OBJFINGER_H
#define OBJFINGER_H

#include <qapp.h>
#include <qobject.h>
#include <qdict.h>
#include <qstring.h>
#include <qstrlist.h>

class objFinder : public QObject {
  Q_OBJECT
public:

  objFinder();
  virtual ~objFinder();
    
  static void insert(QObject *obj, const char *key = 0);
  static QObject *find(const char *name, const char *inherits);
  static void dumpTree();
  static QStrList allObjects();

signals:
  void inserted(QObject *obj);
  
protected slots:
  void objDest();

private:
  /*
   * Don't allow the object to be created, all it's members are static
   */

  static QString randString();

  
  static QDict<QObject> *objList;
    
};

extern objFinder *objFind;

#endif

