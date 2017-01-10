#ifndef ADDWIZARD_H
#define ADDWIZARD_H

#include <kwizard.h>

class QWidget;
class QLineEdit;
class KIntLineEdit;

class AddServerWizard: public KWizard
{
    Q_OBJECT
public:
    AddServerWizard();
    virtual ~AddServerWizard();
    QStrList data;
public slots:
    void pageEntered(unsigned int page);
private:
    unsigned int lastPage;
};

class WizardPage: public QWidget
{
    Q_OBJECT
public:
    WizardPage(QWidget *parent);
    virtual bool validate() {return true; };
    virtual ~WizardPage();
    virtual QString data();
};

class ServerNamePage: public WizardPage
{
    Q_OBJECT
public:
    ServerNamePage(QWidget *parent);
    virtual ~ServerNamePage();
    bool validate();
    QLineEdit *name;
    virtual QString data();
};

class PublicDirPage: public WizardPage
{
    Q_OBJECT
public:
    PublicDirPage(QWidget *parent);
    virtual ~PublicDirPage();
    bool validate();
    QLineEdit *name;
    virtual QString data();
public slots:
    void setDirName();
};

class PortPage: public WizardPage
{
    Q_OBJECT
public:
    PortPage(QWidget *parent);
    virtual ~PortPage();
    bool validate();
    KIntLineEdit *port;
    virtual QString data();
};

class EndPage: public WizardPage
{
    Q_OBJECT
public:
    EndPage(QWidget *parent);
    virtual ~EndPage();
};

#endif
