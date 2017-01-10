#ifndef _PWS_SERVER_
#define _PWS_SERVER_

#include <kprocess.h>
#include <qtimer.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "../../irclistbox.h"
#include "../../irclistitem.h"

class PWSServer : public QObject{
    Q_OBJECT
public:
    PWSServer(QString script, QString LogDir);
    ~PWSServer();

    void showLogWindow(bool show=TRUE);

    void logit(QString txt);
    
private slots:
    void stdoutData(KProcess *proc, char *buf, int len);
    void stderrData(KProcess *proc, char *buf, int len);
    void serverDied(KProcess *proc);

    void webLogData();
    void errorLogData();
    

private:
    KProcess *server;
    
    int fdWeb, fdError;
    QTimer *web, *error;

    KSircListBox *log;
};

#endif