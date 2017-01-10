#ifndef editDefaults_included
#define editDefaults_included

#include <qdialog.h>

#include "userDefaultsPage.h"

class editDefaults : public QDialog {
  Q_OBJECT
public:
  editDefaults(QWidget* parent = NULL, const char* name = NULL);
  virtual ~editDefaults();

  const char *getShell() const;
  const char *getHomeBase() const;
  bool getCreateHomeDir() const;
  bool getCopySkel() const;
  bool getUserPrivateGroup() const;

  void setShell(const char *data);
  void setHomeBase(const char *data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);
  void setUserPrivateGroup(bool data);

protected:
  userDefaultsPage *page1;

protected slots:
  virtual void ok();
  virtual void cancel();
};

#endif // editDefaults_included
