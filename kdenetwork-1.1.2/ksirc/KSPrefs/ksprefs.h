#include <qwidget.h>
#include <qobject.h>
#include <qdialog.h>
#include <qpushbt.h>
#include <qtabdlg.h>

#include "general.h"
#include "startup.h"
#include "serverchannel.h"
#include "UserMenuRef.h"
#include "defaultfilters.h"

struct KSPrefsSettings
{
  QString nick;
  QString real_name;
  QString startup_commands;
};

class KSPrefs : public QDialog
{
  Q_OBJECT
public:
  KSPrefs(QWidget * parent=0, const 
	  char * name=0);
  
  virtual ~KSPrefs();

  void hide();

signals:
  void update();
  void settings(KSPrefsSettings *);

protected:
    void resizeEvent ( QResizeEvent * );

protected slots:
  void slot_apply();
  void slot_cancel();

private:
  QTabDialog *pTab;
  general *pGeneral;
  StartUp *pStart;
  UserMenuRef *pMenu;
  ServerChannel *pServerChannel;
  DefaultFilters *pFilters;
  
};
