/*
    KFinger - ver 0.8.2
    KDE project - kdenetwork
    
    kfinger.C : Main Widget
    
    (c) Andrea Rizzi <rizzi@kde.org>
	14 Apr 1999
    
    License: GPL

*/

#include "kfinger.h"
#include "kiconloader.h"
#include <kstdaccel.h>
#include <qdatetime.h>

NetutilView::NetutilView(const char *name )
    : KTopLevelWidget( name )
{    

protocol = 0L;
//  setCaption( klocale->translate("KDE finger utility") );
  app=KApplication::getKApplication();
  config=app->getConfig();
  KStdAccel keys(config);
  menubar = new KMenuBar( this, "menubar" );
   QPopupMenu *file = new QPopupMenu;
    file->insertItem(klocale->translate("F&inger Now"), this, SLOT(button1Clicked()));
    file->insertItem(klocale->translate("&Talk..."),this,SLOT(talk()));
    file->insertSeparator();
    file->insertItem(klocale->translate("&Quit"), qApp, SLOT(quit()),keys.quit());
   QPopupMenu *options = new QPopupMenu;
    options->insertItem(klocale->translate("Setup"), this, SLOT(setup()) );
    options->insertItem(klocale->translate("&Save Options"), this, SLOT(save()));
   QPopupMenu *help = kapp->getHelpMenu( TRUE,
    i18n("kfinger 0.8.2\n\n(c) Andrea Rizzi (rizzi@kde.org)"));
  menubar->insertItem( klocale->translate("&File"), file );
  menubar->insertItem( klocale->translate("&Options"), options  );
  menubar->insertSeparator();
  menubar->insertItem( klocale->translate("&Help"), help );


    config->setGroup("Options");
    statusBar = new KStatusBar(this);
    statusBar->insertItem(klocale->translate("KDE finger utility"),1);
    toolBar = new KToolBar(this);
 
toolBar->insertButton(Icon("mini-connect.xpm"),0,SIGNAL(clicked()),this ,SLOT(button1Clicked()),1,klocale->translate("Finger"),-1);	
toolBar->insertButton(Icon("stop.xpm"),1,SIGNAL(clicked()),this ,SLOT(stopIt()),1,klocale->translate("Stop current finger"),-1);	
toolBar->setItemEnabled(1,FALSE);
toolBar->insertButton(Icon("mini-ktalk.xpm"),4,SIGNAL(clicked()),this ,SLOT(talk()),1,klocale->translate("Talk..."),-1);	
toolBar->insertButton(Icon("mini-clock.xpm"),5,SIGNAL(toggled(int)),this ,SLOT(slotAuto(int)),1,klocale->translate("AutoFinger On/Off"),-1);	
toolBar->setToggle(5,TRUE);


   le = new QMultiLineEdit( this, "lineEdit" );
    le->setGeometry( 10, 70, 420, 160 );
    le->setReadOnly(1);
    	QFont font( "fixed", 10 );
	font.setStyleHint( (QFont::StyleHint)0 );
	font.setCharSet( (QFont::CharSet)0 );
	le->setFont( font );
	 
  le->setText( klocale->translate("Welcome to KDE Finger\n"));
    QToolTip::add( le, klocale->translate("Finger result" ));




   QComboBox *combo = new   QComboBox(TRUE,toolBar);
   connect(combo,SIGNAL(activated(const char *)), this,SLOT(button1Clicked()));  
   toolBar->insertWidget(3,120,combo);

   QComboBox *combo1 = new   QComboBox(TRUE,toolBar);
   connect(combo1,SIGNAL(activated(const char *)), this,SLOT(button1Clicked()));  
   toolBar->insertWidget(2,120,combo1);

ed2Combo = combo1;
ed3Combo = combo;

loadSettings();
setStatusBar(statusBar);
setMenu(menubar);
addToolBar(toolBar);
setView(le); 
toolBar->show();
menubar->show();
le->show();
enableStatusBar(KStatusBar::Show);


show();

}

void NetutilView::slotAuto(int a)
 {
 autofinger=toolBar->isButtonOn(a);
 config->setGroup("Options");
 config->writeEntry("Time",autofinger);
 }

 void NetutilView::save()
 {
 config->sync();
 }


void NetutilView::loadSettings()
{
int va;
 config->setGroup("Options");
  autofinger = config->readNumEntry("Time");
  toolBar->setButton(5,autofinger);
  va=(config->readNumEntry("Wait")-6) / 6;
  Timer = startTimer(config->readNumEntry("Wait") * 100);
  tencount = 0;    

 while(ed2Combo->count()>0){ ed2Combo->removeItem(0); } 

 config->setGroup("Fserver");
    for(va=1;va<=config->readNumEntry("NumFserver");va++)
      {

	sstr.sprintf("Server%d",va);
	ed2Combo->insertItem(config->readEntry(sstr));
      }
      if(config->readNumEntry("NumFserver")<1)
      ed2Combo->insertItem("localhost");
      
 while(ed3Combo->count()>0) { ed3Combo->removeItem(0); } 
 config->setGroup("Fusers");
    for(va=1;va<=config->readNumEntry("NumFusers");va++)
      {
	sstr.sprintf("User%d",va);
	ed3Combo->insertItem(config->readEntry(sstr));
      }
      if(config->readNumEntry("NumFuser")<1)
      ed3Combo->insertItem(" ");
      
}

void NetutilView::talk()
{
int i,intvar;
QString sstr3;
QString sstr2;
QString user,server;
sstr3="";
sstr2="";
user="";
server="";
config->setGroup("Talk");
Talk *ta= new Talk(this,"Talk...",config->readEntry("TalkCom"));
for(i=config->readNumEntry("StartLine");i<le->numLines()-2;i++){
if(config->readNumEntry("SelectUser")==1) {
config->setGroup("FoUser");
	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O1"));
     	sscanf(sstr3,config->readEntry("F1"),(const char*)sstr2);
	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O2"));
     	sscanf(sstr3,config->readEntry("F2"),&intvar);

	if(config->readNumEntry("Param")==1)sstr3.sprintf(config->readEntry("Out"),(const char*)sstr2);
	if(config->readNumEntry("Param")==2)sstr3.sprintf(config->readEntry("Out"),(const char*)sstr2,intvar);
	if(config->readNumEntry("Param")==3)sstr3.sprintf(config->readEntry("Out"),intvar);
	if(config->readNumEntry("Param")==4)sstr3.sprintf(config->readEntry("Out"),intvar,(const char*)sstr2);
	user=sstr3.copy();

config->setGroup("Talk");
     }  
if(config->readNumEntry("SelectUser")==2)
	{
	  user.sprintf("%s",ed3Combo->currentText());

	} 
if(config->readNumEntry("SelectUser")==3) {
     user=config->readEntry("UserName").copy();
       }


if(config->readNumEntry("SelectServer")==1) {
config->setGroup("FoServer");
	sstr2.sprintf("%s",le->textLine(i));
	sstr2.remove(0,config->readNumEntry("O1"));
     	sscanf(sstr2,config->readEntry("F1"),(const char*)sstr3);
	sstr2.sprintf("%s",le->textLine(i));
	sstr2.remove(0,config->readNumEntry("O2"));
     	sscanf(sstr2,config->readEntry("F2"),&intvar);

	if(config->readNumEntry("Param")==1)sstr2.sprintf(config->readEntry("Out"),(const char*)sstr3);
	if(config->readNumEntry("Param")==2)sstr2.sprintf(config->readEntry("Out"),(const char*)sstr3,intvar);
	if(config->readNumEntry("Param")==3)sstr2.sprintf(config->readEntry("Out"),intvar);
	if(config->readNumEntry("Param")==4)sstr2.sprintf(config->readEntry("Out"),intvar,(const char*)sstr3);
	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O1"));
     	sscanf(sstr3,config->readEntry("F1"),(const char*)sstr2);
	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O2"));
     	sscanf(sstr3,config->readEntry("F2"),&intvar);

	if(config->readNumEntry("Param")==1)sstr3.sprintf(config->readEntry("Out"),(const char *)sstr2);
	if(config->readNumEntry("Param")==2)sstr3.sprintf(config->readEntry("Out"),(const char *)sstr2,intvar);
	if(config->readNumEntry("Param")==3)sstr3.sprintf(config->readEntry("Out"),intvar);
	if(config->readNumEntry("Param")==4)sstr3.sprintf(config->readEntry("Out"),intvar,(const char *)sstr2);	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O1"));
     	sscanf(sstr3,config->readEntry("F1"),(const char*)sstr2);
	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O2"));
     	sscanf(sstr3,config->readEntry("F2"),&intvar);

	if(config->readNumEntry("Param")==1)sstr3.sprintf(config->readEntry("Out"),(const char *)sstr2);
	if(config->readNumEntry("Param")==2)sstr3.sprintf(config->readEntry("Out"),(const char *)sstr2,intvar);
	if(config->readNumEntry("Param")==3)sstr3.sprintf(config->readEntry("Out"),intvar);
	if(config->readNumEntry("Param")==4)sstr3.sprintf(config->readEntry("Out"),intvar,(const char *)sstr2);	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O1"));
     	sscanf(sstr3,config->readEntry("F1"),(const char*)sstr2);
	sstr3.sprintf("%s",le->textLine(i));
	sstr3.remove(0,config->readNumEntry("O2"));
     	sscanf(sstr3,config->readEntry("F2"),&intvar);

	if(config->readNumEntry("Param")==1)sstr3.sprintf(config->readEntry("Out"),(const char *)sstr2);
	if(config->readNumEntry("Param")==2)sstr3.sprintf(config->readEntry("Out"),(const char *)sstr2,intvar);
	if(config->readNumEntry("Param")==3)sstr3.sprintf(config->readEntry("Out"),intvar);
	if(config->readNumEntry("Param")==4)sstr3.sprintf(config->readEntry("Out"),intvar,(const char *)sstr2);	

	server=sstr3.copy();
config->setGroup("Talk");

     }
if(config->readNumEntry("SelectServer")==2)
	{
	  server.sprintf("%s",ed2Combo->currentText() );
	}
if(config->readNumEntry("SelectServer")==3)    {
      server=config->readEntry("ServerName").copy(); }

user+='@';
user+=server;
ta->adduser(user);
}
ta->show();

}

void NetutilView::button1Clicked()
{

 if(protocol!=NULL)
   delete protocol;
  protocol = 0L;
  le->clear();
  toolBar->setItemEnabled(0,FALSE);
  toolBar->setItemEnabled(1,TRUE);
      qApp->processEvents();
      qApp->flushX();

  QString sstr,sstr2;
  sstr2.sprintf("%s",ed2Combo->currentText());  //server name
  sstr.sprintf("%s\n\r",ed3Combo->currentText()); //user name
  
  if (sstr[0]==0) sstr[0]=' ';
  protocol=new FingerProtocol( sstr1);  
  
  connect(protocol,SIGNAL(finish()),this,SLOT(stopIt()));
  connect(protocol,SIGNAL(update()),this,SLOT(writeIt()));
 if (!protocol->connection((const char *)sstr2,79)) // 79 : finger port
  {
    sstr2.sprintf(klocale->translate("Error on server : %s"),ed2Combo->currentText());
    statusBar->changeItem((const char*)sstr2,1);
      if(protocol!=NULL)
	    delete protocol;
    protocol = 0L;
    toolBar->setItemEnabled(1,FALSE);
    toolBar->setItemEnabled(0,TRUE);
  } else {
    sstr2.sprintf("%s\n\r",ed3Combo->currentText());
    if( protocol->writeString(sstr2)==111) 
    {
    
    sstr2.sprintf(klocale->translate("Connection refused by  %s"),ed2Combo->currentText());
    statusBar->changeItem((const char*)sstr2,1);
    
    }
 }

}

void NetutilView::writeIt()
{
 QString sstr;
 le->setText(sstr1);
 sstr.sprintf(klocale->translate("Finger  %s@%s   --   %s"),ed3Combo->currentText(),ed2Combo->currentText(),(const char *)(QDateTime::currentDateTime()).toString());
 statusBar->changeItem(sstr,1);
}

void NetutilView::stopIt()
{
if(protocol!=NULL)
{ 
protocol->stopFlag=FALSE;
//delete protocol;
}
protocol = 0L;

toolBar->setItemEnabled(0,TRUE);
toolBar->setItemEnabled(1,FALSE);
}



void NetutilView::about()
 {
  KMsgBox::message(this,klocale->translate("About"),klocale->translate("Kfinger 0.8.1\n(c) by Andrea Rizzi\nrizzi@kde.org"));
 }

void NetutilView::HtmlHelp()
{ 
 KApplication::getKApplication()->invokeHTMLHelp("kfinger/kfinger.html", "");
}



void NetutilView::timerEvent(QTimerEvent  * )
{
tencount++;
if( (tencount==10)&&(autofinger )) 
 {    
  button1Clicked();
  tencount=0; 
 }
}

void NetutilView::setup()
{
 if(new Setup() == 0L)
   warning("Could not start setup");
 loadSettings();
}











// SETUP DIALOG


Setup::Setup( QWidget *parent, const char *name )
    : QDialog( parent, name,TRUE )
{
setMinimumSize(350,340);
setMaximumSize(350,340);
    KTabCtl *tab = new KTabCtl( this, "ktab" );
setCaption( klocale->translate("Kfinger Setup" ));

    QWidget *w = new QWidget( tab, "page one" );

	tmpQListBox = new QListBox( w, "ListBox_1" );
	tmpQListBox->setGeometry( 10, 100, 200, 150 );
    KApplication *app=KApplication::getKApplication();
//    KConfig *
config=app->getConfig();
int va;
QString sstr;
    config->setGroup("Fserver");
    for(va=1;va<=config->readNumEntry("NumFserver");va++)
      {

	sstr.sprintf("Server%d",va);
	tmpQListBox->insertItem(config->readEntry(sstr));
      }


	tmpQLineEdit = new QLineEdit( w, "LineEdit_1" );
	tmpQLineEdit->setGeometry( 10, 40, 200, 30 );
    connect( tmpQLineEdit, SIGNAL(returnPressed()), SLOT(Add()) );
	QPushButton* tmpQPushButton;
	tmpQPushButton = new QPushButton( w, "PushButton_1" );
	tmpQPushButton->setGeometry( 260, 40, 80, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(Add()) );

	tmpQPushButton->setText( klocale->translate("&Add" ));

	tmpQPushButton = new QPushButton( w, "PushButton_2" );
	tmpQPushButton->setGeometry( 260, 100, 80, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(Remove()) );
	tmpQPushButton->setText( klocale->translate("&Remove") );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( w, "Label_1" );
	tmpQLabel->setGeometry( 10, 10, 200, 30 );
	tmpQLabel->setText( klocale->translate("Server Name" ));

	tmpQLabel = new QLabel( w, "Label_2" );
	tmpQLabel->setGeometry( 10, 70, 200, 30 );
	tmpQLabel->setText( klocale->translate("Server list" ));

	w->resize( 350, 300 );
    tab->addTab( w, klocale->translate("Server list" ));

    w = new QWidget( tab, "page two" );


	tmpQListBox1 = new QListBox( w, "ListBox_1" );
	tmpQListBox1->setGeometry( 10, 100, 200, 150 );

    config->setGroup("Fusers");
    for(va=1;va<=config->readNumEntry("NumFusers");va++)
      {

	sstr.sprintf("User%d",va);
	tmpQListBox1->insertItem(config->readEntry(sstr));
      }

	tmpQLineEdit1 = new QLineEdit( w, "LineEdit_1" );
	tmpQLineEdit1->setGeometry( 10, 40, 200, 30 );
        connect( tmpQLineEdit1, SIGNAL(returnPressed()), SLOT(Add1() ) );

	tmpQPushButton = new QPushButton( w, "PushButton_1" );
	tmpQPushButton->setGeometry( 260, 40, 80, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(Add1()) );
	tmpQPushButton->setText( klocale->translate("&Add") );

	tmpQPushButton = new QPushButton( w, "PushButton_2" );
	tmpQPushButton->setGeometry( 260, 100, 80, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(Remove1()) );
	tmpQPushButton->setText( klocale->translate("&Remove") );

	//QLabel* tmpQLabel;
	tmpQLabel = new QLabel( w, "Label_1" );
	tmpQLabel->setGeometry( 10, 10, 200, 30 );
	tmpQLabel->setText( klocale->translate("User Name" ));

	tmpQLabel = new QLabel( w, "Label_2" );
	tmpQLabel->setGeometry( 10, 70, 200, 30 );
	tmpQLabel->setText( klocale->translate("Users list" ));

	w->resize( 350, 300 );
    tab->addTab( w, klocale->translate("Users list" ));

    w = new QWidget( tab, "page three" );

    config->setGroup("Options");

	tmpQCheckBox = new QCheckBox( w, "CheckBox_1" );
	tmpQCheckBox->setGeometry( 10, 30, 160, 30 );
	
		QFont font( "helvetica", 14, 50, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		tmpQCheckBox->setFont( font );
	
	tmpQCheckBox->setText( klocale->translate("Timed finger") );

	tmpQCheckBox->setChecked(config->readNumEntry("Time"));

        tmpQSlider = new KSlider(KSlider::Horizontal,w, "Slider_1" );
	tmpQSlider->setGeometry( 10, 70, 330, 50 );
	tmpQSlider->setSteps( 10, 10 );
        va=(config->readNumEntry("Wait")-6) / 6;
	tmpQSlider->setValue( va );
	connect( tmpQSlider, SIGNAL(valueChanged(int)), SLOT(value(int)) );
	FingerTime = new QLabel( w, "Label_3" );
	FingerTime->setGeometry( 10, 100, 180, 30 );
	
		FingerTime->setFont( font );
	
	sstr.sprintf(klocale->translate("Finger wait: %d\""),(va+1)*6);
        FingerTime->setText(sstr );

	QLabel *tmpLabel = new QLabel( w, "Label_4" );
	tmpLabel->setGeometry( 10, 180, 150, 30 );
        tmpLabel->setText(klocale->translate("Talk Command "));
        	tmpLabel->setFont( font );
	tmpLabel->setAlignment( AlignVCenter | AlignRight );
        
	config->setGroup("Talk");
	TCQLineEdit = new QLineEdit( w, "LineEdit_11" );
	TCQLineEdit->setGeometry( 170, 180, 150, 30 );
        TCQLineEdit->setText(config->readEntry("TalkCom"));    
	
	resize( 350, 300 );
        tab->addTab( w, klocale->translate("Misc") );
    
    w = new QWidget( tab, "page four" );
	but[4] = new QRadioButton( w, "RadioButton_1" );
	but[4]->setGeometry( 20, 165, 150, 20 );
	but[4]->setText( klocale->translate("Finger Output" ));

	but[5] = new QRadioButton( w, "RadioButton_2" );
	but[5]->setGeometry( 20, 225, 150, 20 );
	but[5]->setText( klocale->translate("Finger server" ));

	but[6] = new QRadioButton( w, "RadioButton_3" );
	but[6]->setGeometry( 20, 245, 150, 20 );
	but[6]->setText( klocale->translate("Fixed" ));

	but[1] = new QRadioButton( w, "RadioButton_4" );
	but[1]->setGeometry( 20, 45, 150, 20 );
	but[1]->setText( klocale->translate("Finger Output") );

	but[2] = new QRadioButton( w, "RadioButton_5" );
	but[2]->setGeometry( 20, 105, 150, 20 );
	but[2]->setText( klocale->translate("Finger User") );

	but[3] = new QRadioButton( w, "RadioButton_6" );
	but[3]->setGeometry( 20, 125, 150, 20 );
	but[3]->setText( klocale->translate("Fixed" ));

	UQLineEdit = new QLineEdit( w, "LineEdit_2" );
	UQLineEdit->setGeometry( 80, 245, 200, 20 );
	UQLineEdit->setText(config->readEntry("ServerName"));

	SQLineEdit = new QLineEdit( w, "LineEdit_3" );
	SQLineEdit->setGeometry( 80, 125, 200, 20 );
	SQLineEdit->setText(config->readEntry("UserName") );

	TQLineEdit = new QLineEdit( w, "LineEdit_4" );
	TQLineEdit->setGeometry( 250, 10, 50, 20 );
	TQLineEdit->setText( config->readEntry("StartLine") );

	tmpQLabel = new QLabel( w, "Label_1" );
	tmpQLabel->setGeometry( 60, 10, 180, 20 );
	tmpQLabel->setText( klocale->translate("Lines to skip :" ));
	tmpQLabel->setAlignment( AlignRight );
	tmpQLabel->setMargin( -1 );

	
	tmpQPushButton = new QPushButton( w, "PushButton" );
	tmpQPushButton->setGeometry( 190, 70, 120, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(format()) );
	tmpQPushButton->setText( klocale->translate("Format..." ));

	tmpQPushButton = new QPushButton( w, "PushButton" );
	tmpQPushButton->setGeometry( 190, 190, 120, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(format1() ) );
	tmpQPushButton->setText( klocale->translate("Format...") );
        

	QButtonGroup* tmpQButtonGroup;
	tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_2" );
	tmpQButtonGroup->setGeometry( 10, 30, 330, 120 );
	tmpQButtonGroup->setFrameStyle( 49 );
	tmpQButtonGroup->setTitle( klocale->translate("User" ));
	tmpQButtonGroup->setAlignment( 1 );
	tmpQButtonGroup->insert( but[1] );
	tmpQButtonGroup->insert( but[2] );
	tmpQButtonGroup->insert( but[3] );
	tmpQButtonGroup->lower();
	tmpQButtonGroup->setExclusive( TRUE );

	tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_1" );
	tmpQButtonGroup->setGeometry( 10, 150, 330, 120 );
	tmpQButtonGroup->setFrameStyle( 49 );
	tmpQButtonGroup->setTitle( klocale->translate("Server" ));
	tmpQButtonGroup->setAlignment( 1 );
	tmpQButtonGroup->insert( but[4] );
	tmpQButtonGroup->insert( but[5] );
	tmpQButtonGroup->insert( but[6] );
	tmpQButtonGroup->lower();
	tmpQButtonGroup->setExclusive( TRUE );

        if(config->readNumEntry("SelectUser")>0) but[config->readNumEntry("SelectUser")]->setChecked(1);
        if(config->readNumEntry("SelectServer")>0) but[config->readNumEntry("SelectServer")+3]->setChecked(1);

	w->resize( 350, 300 );
	tab->addTab( w, klocale->translate("Talk" ));



    QPushButton *quitButton = new QPushButton( this, "cancelButton" );
    quitButton->setText( klocale->translate("Cancel") );
    quitButton->setGeometry( 275,305, 65,30 );
    connect( quitButton, SIGNAL(clicked()), SLOT(quit()) );
    
quitButton = new QPushButton( this, "OKButton" );
    quitButton->setText( klocale->translate("OK" ));
    quitButton->setGeometry( 10,305, 65,30 );
    connect( quitButton, SIGNAL(clicked()), SLOT(save()) );
    tab->resize( 350, 300 );




show();
}



  

void Setup::format()
{
 if(new Format(0,0,"FoUser")==0)
   warning("Could not open Format Dialog");
}

void Setup::format1()
{
 if(new Format(0,0,"FoServer")==0)
   warning("Could not open Format Dialog");
}

void Setup::quit()
{
close();
}

void Setup::save()
{
unsigned int va;
config->setGroup("Options");
config->writeEntry("Time",tmpQCheckBox->isChecked());
config->writeEntry("Wait",(tmpQSlider->value()+1)*6);
QString sstr;
config->setGroup("Fusers");
config->writeEntry("NumFusers",tmpQListBox1->count());
   
 for(va=1;va<=tmpQListBox1->count();va++)
      {
	sstr.sprintf("User%d",va);
	config->writeEntry(sstr,tmpQListBox1->text(va-1));
      }
config->setGroup("Fserver");
config->writeEntry("NumFserver",tmpQListBox->count());
   
 for(va=1;va<=tmpQListBox->count();va++)
      {
	sstr.sprintf("Server%d",va);
	config->writeEntry(sstr,tmpQListBox->text(va-1));
      }

config->setGroup("Talk");
config->writeEntry("TalkCom",TCQLineEdit->text());
config->writeEntry("StartLine",TQLineEdit->text());

for(va=1;va<=3;va++){
   if(but[va]->isChecked())config->writeEntry("SelectUser",va); 
   if(but[va+3]->isChecked())config->writeEntry("SelectServer",va);
 }

config->writeEntry("UserName",SQLineEdit->text());
config->writeEntry("ServerName",UQLineEdit->text());


close();
}




void Setup::value(int valu)
{
QString sstr;
sstr.sprintf(klocale->translate("Finger wait: %d\""),(valu+1)*6);
FingerTime->setText(sstr);
}
void Setup::Add()
{
tmpQListBox->insertItem(tmpQLineEdit->text(),tmpQListBox->currentItem());
}
void Setup::Remove()
{
tmpQListBox->removeItem(tmpQListBox->currentItem());
}
void Setup::Add1()
{
tmpQListBox1->insertItem(tmpQLineEdit1->text(),tmpQListBox1->currentItem());
}
void Setup::Remove1()
{
tmpQListBox1->removeItem(tmpQListBox1->currentItem());
}

//TALK DIALOG



Talk::Talk( QWidget *parent, const char *name,QString command)
    : QDialog( parent, name,TRUE )
{
       theCommand=command;
       setMinimumSize(400,310);
       setMaximumSize(400,310);

	usrQListBox = new QListBox( this, "ListBox_1" );
	usrQListBox->setGeometry( 10, 80, 380, 180 );
	connect( usrQListBox, SIGNAL(selected(int)), SLOT(newselect(int)) );
	usrQListBox->setFrameStyle( 51 );
	usrQListBox->setLineWidth( 2 );

	//QLineEdit* tmpQLineEdit;
	TalkLineEdit = new QLineEdit( this, "LineEdit_1" );
	TalkLineEdit->setGeometry( 140, 30, 250, 30 );
	connect( TalkLineEdit, SIGNAL(returnPressed()), SLOT(runtalk()) );
	TalkLineEdit->setText( "" );

	QPushButton* tmpQPushButton;
	tmpQPushButton = new QPushButton( this, "PushButton_1" );
	tmpQPushButton->setGeometry( 20, 30, 100, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(runtalk()) );
	tmpQPushButton->setText( klocale->translate("Talk with..." ));
	tmpQPushButton->setAutoRepeat( FALSE );
	tmpQPushButton->setAutoResize( FALSE );
	tmpQPushButton->setAutoDefault( TRUE );

	tmpQPushButton = new QPushButton( this, "PushButton_2" );
	tmpQPushButton->setGeometry( 150, 270, 100, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(quit()) );
	tmpQPushButton->setText( klocale->translate("Cancel" ));
	tmpQPushButton->setAutoRepeat( FALSE );
	tmpQPushButton->setAutoResize( FALSE );

	resize( 400, 310 );
//show();
}


Talk::~Talk()
{
}
void Talk::adduser(const char *username)
{
usrQListBox->insertItem(username);
}

void Talk::newselect(int a)
{
TalkLineEdit->setText(usrQListBox->text(a) );
}


void Talk::runtalk()
{
sprintf(sstr1,theCommand+" %s",TalkLineEdit->text());
 if ((fork())==0) {
execl("/bin/sh", "sh", "-c",sstr1, NULL);

      _exit(127);
    } 

}
void Talk::quit()
{
close();
}


// Format Dialog

Format::Format( QWidget *parent, const char *name,const char *sufi )
    : QDialog( parent, name,TRUE )
{
KApplication *app=KApplication::getKApplication();
config=app->getConfig();

setMinimumSize(410,330);
setMaximumSize(410,330);
	var1Format = new QLineEdit( this, "LineEdit_1" );
	var1Format->setGeometry( 220, 20, 80, 30 );
	var1Format->setText( "" );
	var1Format->setMaxLength( 32767 );
	var1Format->setEchoMode( QLineEdit::Normal );
	var1Format->setFrame( TRUE );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this, "Label_1" );
	tmpQLabel->setGeometry( 10, 20, 210, 30 );
	tmpQLabel->setText( klocale->translate("Format string for var1(string)") );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	tmpQLabel = new QLabel( this, "Label_2" );
	tmpQLabel->setGeometry( 10, 70, 210, 30 );
	tmpQLabel->setText( klocale->translate("Fromat string for var2(int)" ));
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	var2Format = new QLineEdit( this, "LineEdit_2" );
	var2Format->setGeometry( 220, 70, 80, 30 );
	var2Format->setText( "" );
	var2Format->setMaxLength( 32767 );
	var2Format->setEchoMode( QLineEdit::Normal );
	var2Format->setFrame( TRUE );

	var1Offset = new QLineEdit( this, "LineEdit_3" );
	var1Offset->setGeometry( 375, 20, 30, 30 );
	var1Offset->setText( "" );
	var1Offset->setMaxLength( 32767 );
	var1Offset->setEchoMode( QLineEdit::Normal );
	var1Offset->setFrame( TRUE );

	tmpQLabel = new QLabel( this, "Label_3" );
	tmpQLabel->setGeometry( 300, 20, 75, 30 );
	tmpQLabel->setText( klocale->translate("Offset" ));
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	tmpQLabel = new QLabel( this, "Label_4" );
	tmpQLabel->setGeometry( 300, 70, 75, 30 );
	tmpQLabel->setText( klocale->translate("Offset" ));
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	var2Offset = new QLineEdit( this, "LineEdit_4" );
	var2Offset->setGeometry( 375, 70,30, 30 );
	var2Offset->setText( "" );
	var2Offset->setMaxLength( 32767 );
	var2Offset->setEchoMode( QLineEdit::Normal );
	var2Offset->setFrame( TRUE );

	cbvar[1] = new QRadioButton( this, "RadioButton_1" );
	cbvar[1]->setGeometry( 80, 140, 100, 30 );
	cbvar[1]->setText( klocale->translate("Only var1") );
	cbvar[1]->setAutoRepeat( FALSE );
	cbvar[1]->setAutoResize( FALSE );

	cbvar[3] = new QRadioButton( this, "RadioButton_2" );
	cbvar[3]->setGeometry( 80, 180, 100, 30 );
	cbvar[3]->setText( klocale->translate("Only var2" ));
	cbvar[3]->setAutoRepeat( FALSE );
	cbvar[3]->setAutoResize( FALSE );

	cbvar[2] = new QRadioButton( this, "RadioButton_3" );
	cbvar[2]->setGeometry( 210, 140, 100, 30 );
	cbvar[2]->setText( klocale->translate("var1, var2" ));
	cbvar[2]->setAutoRepeat( FALSE );
	cbvar[2]->setAutoResize( FALSE );

	cbvar[4] = new QRadioButton( this, "RadioButton_4" );
	cbvar[4]->setGeometry( 210, 180, 100, 30 );
	cbvar[4]->setText( klocale->translate("var2, var1" ));
	cbvar[4]->setAutoRepeat( FALSE );
	cbvar[4]->setAutoResize( FALSE );

	outFormat = new QLineEdit( this, "LineEdit_5" );
	outFormat->setGeometry( 230, 230, 150, 30 );
	outFormat->setText( "" );
	outFormat->setMaxLength( 32767 );
	outFormat->setEchoMode( QLineEdit::Normal );
	outFormat->setFrame( TRUE );

	tmpQLabel = new QLabel( this, "Label_5" );
	tmpQLabel->setGeometry( 10, 230, 190, 30 );
	tmpQLabel->setText( klocale->translate("Output format string" ));
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	QPushButton* tmpQPushButton;
	tmpQPushButton = new QPushButton( this, "PushButton_1" );
	tmpQPushButton->setGeometry( 20, 280, 100, 40 );
	tmpQPushButton->setText( klocale->translate("Save & Quit" ));
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(save()) );	
	tmpQPushButton = new QPushButton( this, "PushButton_2" );
	tmpQPushButton->setGeometry( 290, 280, 100, 40 );
	tmpQPushButton->setText( klocale->translate("Cancel" ));
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(quit()) );	
	QButtonGroup* tmpQButtonGroup;
	tmpQButtonGroup = new QButtonGroup( this, "ButtonGroup_1" );
	tmpQButtonGroup->setGeometry( 60, 120, 260, 100 );
	tmpQButtonGroup->setFrameStyle( 49 );
	tmpQButtonGroup->setTitle( klocale->translate("Output parameters " ));
	tmpQButtonGroup->setAlignment( 1 );
	tmpQButtonGroup->insert( cbvar[1] );
	tmpQButtonGroup->insert( cbvar[3] );
	tmpQButtonGroup->insert( cbvar[2] );
	tmpQButtonGroup->insert( cbvar[4] );
	tmpQButtonGroup->lower();
suf.sprintf("%s",sufi);
config->setGroup(suf);

if(config->readNumEntry("Param")>0) cbvar[config->readNumEntry("Param")]->setChecked(1);
var1Format->setText(config->readEntry("F1"));
var1Offset->setText(config->readEntry("O1"));
var2Format->setText(config->readEntry("F2"));
var2Offset->setText(config->readEntry("O2"));
outFormat->setText(config->readEntry("Out"));

	resize( 410, 330 );
show();
}

void Format::save()
{
QString sstr;
int i;
config->setGroup(suf);
for(i=1;i<=4;i++)
if(cbvar[i]->isChecked()) config->writeEntry("Param",i);
config->writeEntry("F1",var1Format->text());
config->writeEntry("O1",var1Offset->text());

config->writeEntry("F2",var2Format->text());
config->writeEntry("O2",var2Offset->text());

config->writeEntry("Out",outFormat->text());
config->setGroup("Talk");
close();
}

void Format::quit()
{
close();
}



#include "kfinger.moc"

int main( int argc, char **argv )
{
    sstr1=(char *) malloc(8000);  // Try to change it if you recieve a segmentation fault
    KApplication *a=new KApplication(argc,argv,QString("kfinger"));
    NetutilView  *w = new NetutilView;
    a->setMainWidget( w );
    w->show();
    return a->exec();
}
