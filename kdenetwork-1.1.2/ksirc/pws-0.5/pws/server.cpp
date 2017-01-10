#include "server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>

#include "../../irclistitem.h"

#include "server.moc"

PWSServer::PWSServer(QString script, QString logDir)
    : QObject()
{
    log = new("KSircListBox") KSircListBox();
    log->setCaption("Web Server Event Log");
    log->resize(400,100);

    web = 0x0;
    error = 0x0;
    
    fdWeb = open(logDir+"/WebLog", O_CREAT|O_RDONLY|O_NONBLOCK, S_IRUSR|S_IWUSR);
    if(fdWeb > 0){
        lseek(fdWeb, 0, SEEK_END);
        web = new("QTimer") QTimer(this, "web_timer");
        connect(web, SIGNAL(timeout()),
                this, SLOT(webLogData()));
        web->start(10000, FALSE); // 10 seconds it more than enough waste
    }
    else {
        warning("Could not open WebLog");
        perror("Failed");
    }
    
    fdError = open(logDir+"/ErrorLog", O_CREAT|O_RDONLY|O_NONBLOCK, S_IRUSR|S_IWUSR);
    if(fdError > 0){
        lseek(fdError, 0, SEEK_END);
        error = new("QTimer") QTimer(this, "error_timer");
        connect(error, SIGNAL(timeout()),
                this, SLOT(errorLogData()));
        error->start(10000, FALSE); // 10 seconds, like since who really cares
    }
    else{
        warning("Could not open ErrorLog");
        perror("Failed");
    }


    server = new("KProcess") KProcess();
    /* Setup and create the acutal mathopd process */
    connect(server, SIGNAL(processExited(KProcess *)),
            this, SLOT(serverDied(KProcess *)));
    connect(server, SIGNAL(receivedStdout(KProcess *, char *, int)),
            this, SLOT(stdoutData(KProcess *, char *, int)));
    connect(server, SIGNAL(receivedStderr(KProcess *, char *, int)),
            this, SLOT(stderrData(KProcess *, char *, int)));

    *server << "mathpod" << "-n";
    server->start(KProcess::NotifyOnExit, KProcess::All);

    int config = open(script, O_RDONLY);
    if(config <= 0){
        warning("Couldn't open: %s", script.data());
        perror("Failed");
        return;
    }
    struct stat finfo;
    if(fstat(config, &finfo) == 0){
        char *buf;
        buf = (char *) mmap(0x0, finfo.st_size, PROT_READ, MAP_PRIVATE, config, 0x0);
        if(buf < 0){
            perror("MMAP of config file failed");
            return;
        }
        server->writeStdin(buf, finfo.st_size);
        munmap(buf, finfo.st_size);
    }
    server->closeStdin();
}

PWSServer::~PWSServer()
{
    debug("Sending kill");
    server->kill();
    disconnect(server, 0, this, 0);
    // I don't delete the server controller since it only gets created once and ends once and It's segfaulting and I'm not sure why
//    delete server;

    log->hide();
    delete web;
    delete error;
    delete log;
}

void PWSServer::showLogWindow(bool show)
{
    if(show == TRUE)
        log->show();
    else
        log->hide();
}

void PWSServer::stdoutData(KProcess *proc, char *buf, int len)
{
    QString str(buf, len);
    logit(str);
}

void PWSServer::stderrData(KProcess *proc, char *buf, int len)
{
    QString str(buf, len);
    str.prepend("~4");
    logit(str);
    showLogWindow(TRUE);
}
void PWSServer::serverDied(KProcess *proc)
{
  if(web != 0)
    web->stop();
  if(error != 0)
    error->stop();
  
  webLogData();
  errorLogData();
  close(fdWeb);
  fdWeb = -1;
  close(fdError);
  fdError = -1;
  logit("~0,4*** Server Exited ***");
  logit("~0,4*** kSirc Server Shutdown ***");
}

void PWSServer::webLogData()
{
    char buf[1024];
    int bytes = read(fdWeb, buf, 1023);
    while(bytes > 0){
        if(bytes <= 1024)
            buf[bytes] = 0x0; // Bytes since 0 is first (not bytes+1)
        char *line = strtok(buf, "\n");
        while(line){
            logit(line);
            line = strtok(NULL, "\n");
        }
        bytes = read(fdWeb, buf, 1023);
    }
}

void PWSServer::errorLogData()
{
    char buf[1024];
    int bytes = read(fdError, buf, 1023);
    while(bytes > 0){
        if(bytes <= 1024)
            buf[bytes] = 0x0; // Bytes since 0 is first
        char *line = strtok(buf, "\n");
        while(line){
            QString sline = QString("~4") + line;
            logit(sline);
            line = strtok(NULL, "\n");
        }
        bytes = read(fdError, buf, 1023);
    }
}

void PWSServer::logit(QString txt)
{
    log->insertItem(new("ircListItem") ircListItem(txt, &black, log, 0));
    log->scrollToBottom(TRUE);
}
