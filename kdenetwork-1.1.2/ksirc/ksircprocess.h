#ifndef KSIRCPROCESS
#define KSIRCPROCESS

class KSircProcess;
class KSircTopLevel;

#include <qobject.h>

#include <kprocess.h>

#include "iocontroller.h"
#include "messageReceiver.h"

class KSircProcess : public QObject
{
  Q_OBJECT;
  friend class KSircIOController;
public:
  KSircProcess(char *_server=0L, QObject * parent=0, const char * name=0);
  virtual ~KSircProcess();

  QDict<KSircMessageReceiver> &getWindowList()
    {
      return TopList;
    }

  virtual bool eventFilter ( QObject *, QEvent * );
  
signals:
  void ProcMessage(QString, int, QString);

public slots:
 virtual void ServMessage(QString server, int command, QString args);
 virtual void new_toplevel(QString);
 //  virtual void new_ksircprocess(QString);
 virtual void close_toplevel(KSircTopLevel *, char *);
 virtual void default_window(KSircTopLevel *);
 virtual void recvChangeChannel(QString, QString);
 
 virtual void filters_update();

  virtual void notify_forw_online(QString);
  virtual void notify_forw_offline(QString);

protected slots:
  virtual void clean_toplevel(KSircTopLevel *clean);

  virtual void turn_on_autocreate();
      
  
private:
  void base_filters();

  KSircIOController *iocontrol;
  KProcess *proc;

  QDict<KSircMessageReceiver> TopList;

  bool running_window;
  bool default_follow_focus;
  bool auto_create_really;

  char *server;

};

#endif
