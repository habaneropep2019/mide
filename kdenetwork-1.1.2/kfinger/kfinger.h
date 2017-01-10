
#ifndef __KFINGER_H_
#define __KFINGER_H_
#include <qbttngrp.h>
#include <qradiobt.h>
#include <qlistbox.h>
#include <qlined.h>
#include "finger.h"
#include <ktabctl.h>
#include <qkeycode.h> 
#include <kmenubar.h>
//#include <kpopmenu.h>
#include <qmlined.h>
#include <unistd.h>
//#include <qevent.h>
//#include <qpixmap.h>
#include <kapp.h>
#include <kslider.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qtooltip.h>
#include <kmsgbox.h>
#include <ktopwidget.h>
//#include <kstatusbar.h>
//#include <ktoolbar.h>
char *sstr1;

class NetutilView : public KTopLevelWidget
{
    Q_OBJECT
public:
    NetutilView( const char *name=0 );

private slots:
    void	slotAuto(int a);
    void        stopIt(); 
    void        writeIt();
    void        save(); 
    void        loadSettings(); 
    void	button1Clicked();
    void	talk();
//    void	sliderValueChanged( int );
    void        timerEvent(QTimerEvent *);
    void        about();
    void        HtmlHelp(); 
    void        setup();
private:
    KToolBar *toolBar;
    KStatusBar *statusBar;
    QString sstr;  
    KConfig *config;
    KApplication *app;
    QMultiLineEdit *le;
    KMenuBar *menubar;
    QLabel     *msg;
    bool  autofinger;
    FingerProtocol *protocol;
    //    QPushButton *pb;
    int Timer,tencount,val;
    //KCombo *ed2Combo,*ed3Combo;
    QComboBox *ed2Combo,*ed3Combo;
//    KSlider* sb;

};


class Format : public QDialog
{
    Q_OBJECT

public:
    Format (QWidget* parent = NULL,const char* name = NULL,const char *sufi=NULL);
 
protected slots:
    void save();
    void quit();

protected:
    KConfig *config;
    KApplication *app;

    QString suf;
    QLineEdit* var1Format;
    QLineEdit* var2Format;
    QLineEdit* var1Offset;
    QLineEdit* var2Offset;
    QRadioButton* cbvar[5];
    QLineEdit* outFormat;

};



class Talk : public QDialog
{
    Q_OBJECT

public:

    Talk (QWidget* parent = 0,const char* name = 0,QString command="ktalk");

    virtual ~Talk();


public slots:
    void adduser(const char *username);
protected slots:

    virtual void newselect(int a);
    virtual void runtalk();
    virtual void quit();

private:
QListBox* usrQListBox;
QLineEdit* TalkLineEdit;
QString theCommand;
protected:

};



class Setup : public QDialog
{
    Q_OBJECT
public:
    Setup( QWidget *parent=0, const char *name=0 );
private:
    QLineEdit* tmpQLineEdit;
    QListBox* tmpQListBox; 
    QLineEdit* tmpQLineEdit1;
    QListBox* tmpQListBox1; 
    QCheckBox* tmpQCheckBox;
    KSlider* tmpQSlider;
    QLabel* FingerTime;
    KConfig* config;
    QRadioButton* but[7];
    QLineEdit* SQLineEdit;
    QLineEdit* UQLineEdit;
    QLineEdit* TQLineEdit;
    QLineEdit* TCQLineEdit;

private slots:
    void        value(int);
    void        quit();
    void        save();  
    void        Add();
    void        Remove();
    void        Add1();
    void        Remove1();
    void        format();
    void        format1();
};





#endif
