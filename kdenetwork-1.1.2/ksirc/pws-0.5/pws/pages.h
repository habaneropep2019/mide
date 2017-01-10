#ifndef PAGES_H
#define PAGES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qwidget.h>

class QLineEdit;
class QMultiLineEdit;
class QCheckBox;
class QString;
class KIntLineEdit;

class Page : public QWidget
{
    Q_OBJECT
public:
    Page(QWidget *parent = 0, const char *name = 0);
    virtual ~Page();
    virtual void dumpData();
};

class GeneralPage: public Page
{
    Q_OBJECT
public:
    GeneralPage(QWidget *parent = 0, const char *name = 0);
    virtual ~GeneralPage();
    virtual void dumpData();
    QMultiLineEdit *cgiext;
    QMultiLineEdit *indexnames;
    QMultiLineEdit *mimetypes;
    QLineEdit *logfile;
    QLineEdit *errorlog;
    QLineEdit *childlog;
    QLineEdit *pidfile;
public slots:
    void exam1();
};

class ServerPage: public Page
{
    Q_OBJECT
public:
    ServerPage(QWidget *parent = 0, const char *name = 0);
    virtual ~ServerPage();
    virtual void dumpData();
    QString Name;
    QCheckBox *enabled;
    KIntLineEdit *port;
    QLineEdit *htmldir;
    QLineEdit *alias1;
    QLineEdit *aliasdir1;
    QLineEdit *alias2;
    QLineEdit *aliasdir2;
    QLineEdit *alias3;
    QLineEdit *aliasdir3;
public slots:
    void examfp();
    void exama1();
    void exama2();
    void exama3();
};

#endif