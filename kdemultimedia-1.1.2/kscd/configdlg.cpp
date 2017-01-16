
/*
 *
 * kscd -- A simple CD player for the KDE project
 *
 * $Id$
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben 
 * wuebben@math.cornell.edu
 *
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#include "configdlg.h"
#define XOFF 75
#define YOFF 30
//#define XOFF 0
//#define YOFF 0

extern KApplication *mykapp;


ConfigDlg::ConfigDlg(QWidget *parent=0, struct configstruct *data = 0,const char *name=0)
  : QDialog(parent, name)
{

  configdata.background_color = black;
  configdata.led_color = green;
  configdata.tooltips = true;
  configdata.cd_device ="/dev/cdrom";
  configdata.mailcmd = "mail -s \"%s\" ";
  configdata.docking = true;
  configdata.autoplay = false;
  configdata.autodock = false;
  configdata.stopexit = true;
  configdata.ejectonfinish = false;

  if(data){
    configdata.background_color = data->background_color;
    configdata.led_color = data->led_color;
    configdata.tooltips = data->tooltips;
    configdata.cd_device = data->cd_device;
    configdata.mailcmd = data->mailcmd;
    configdata.browsercmd = data->browsercmd;
    configdata.use_kfm = data->use_kfm;
    configdata.docking = data->docking;
    configdata.autoplay = data->autoplay;
    configdata.autodock = data->autodock;
    configdata.stopexit = data->stopexit;
    configdata.ejectonfinish = data->ejectonfinish;
  }

  colors_changed = false;

  setCaption(klocale->translate("Configure kscd"));

  box = new QGroupBox(this, "box");
  box->setGeometry(10,10,520,420);

  label1 = new QLabel(this);
  label1->setGeometry(20+XOFF,25+YOFF,135,25);
  label1->setText(klocale->translate("LED Color:"));
  label1->setFixedSize( label1->sizeHint() );

  qframe1 = new QFrame(this);
  qframe1->setGeometry(155+XOFF,25+YOFF,30,25);
  qframe1->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  qframe1->setBackgroundColor(configdata.led_color);

  button1 = new QPushButton(this);
  button1->setGeometry(255+XOFF,25+YOFF,100,25);
  button1->setText(klocale->translate("Change"));
  connect(button1,SIGNAL(clicked()),this,SLOT(set_led_color()));

  label2 = new QLabel(this);
  label2->setGeometry(20+XOFF,55+YOFF,135,25);
  label2->setText(klocale->translate("Background Color:"));

  qframe2 = new QFrame(this);
  qframe2->setGeometry(155+XOFF,55+YOFF,30,25);
  qframe2->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  qframe2->setBackgroundColor(configdata.background_color);

  button2 = new QPushButton(this);
  button2->setGeometry(255+XOFF,55+YOFF,100,25);
  button2->setText(klocale->translate("Change"));
  connect(button2,SIGNAL(clicked()),this,SLOT(set_background_color()));

  label5 = new QLabel(this);
  label5->setGeometry(20+XOFF,85+YOFF,135,25);
  label5->setText(klocale->translate("CDROM Device:"));

  cd_device_edit = new QLineEdit(this);
  cd_device_edit->setGeometry(155+XOFF,85+YOFF,200,25);
  cd_device_edit->setText(configdata.cd_device);
  connect(cd_device_edit,SIGNAL(textChanged(const char*)),
	  this,SLOT(device_changed(const char*)));  

#if defined(sun) || defined(__sun__) || defined(__osf__) || defined(ultrix) || defined(__ultrix)

  label5->hide();
  cd_device_edit->hide();

#endif

  label6 = new QLabel(this);
  label6->setGeometry(20+XOFF,115+YOFF,135,25);
  label6->setText(klocale->translate("Unix mail command:"));

  mail_edit = new QLineEdit(this);
  mail_edit->setGeometry(155+XOFF,115+YOFF,200,25);
  mail_edit->setText(configdata.mailcmd);
  connect(mail_edit,SIGNAL(textChanged(const char*)),
	  this,SLOT(mail_changed(const char*)));  

  browserbox = new  QButtonGroup(klocale->translate("WWW-Browser"),this,"wwwbox");
//  browserbox->setGeometry(20+XOFF,145+YOFF,338,130);
  browserbox->setGeometry(20+XOFF,145+YOFF,338, 95);
  kfmbutton = new QRadioButton(klocale->translate("Use kfm as Browser"),
			       browserbox,"kfmbutton");
  kfmbutton->move(10,20);
  kfmbutton->adjustSize();
  kfmbutton->setChecked(configdata.use_kfm);
  connect(kfmbutton,SIGNAL(clicked()),this,SLOT(kfmbutton_clicked()));

  custombutton = new QRadioButton(klocale->translate("Use Custom Browser:"),
				  browserbox,"custombutton");
  custombutton->move(10,40);
  custombutton->adjustSize();
  custombutton->setChecked(!configdata.use_kfm);
  connect(custombutton,SIGNAL(clicked()),this,SLOT(custombutton_clicked()));

  custom_edit = new QLineEdit(browserbox,"customedit");
  custom_edit->setText(data->browsercmd);
  custom_edit->setEnabled(!configdata.use_kfm);
  custom_edit->setGeometry(30,60,198+70,25);

  ttcheckbox = new QCheckBox(klocale->translate("Show Tool Tips"), 
			     this, "tooltipscheckbox");
  ttcheckbox->setGeometry(30+XOFF,245+YOFF,135, 15);
  ttcheckbox->setFixedSize( ttcheckbox->sizeHint() );
  ttcheckbox->setChecked(configdata.tooltips);
  connect(ttcheckbox,SIGNAL(clicked()),this,SLOT(ttclicked()));

  dockcheckbox = new QCheckBox(klocale->translate("Enable KPanel Docking"), 
			       this, "dockcheckbox");
  dockcheckbox->setGeometry(30+XOFF,265+YOFF,200, 15);
  dockcheckbox->setFixedSize( dockcheckbox->sizeHint() );
  dockcheckbox->setChecked(configdata.docking);
  connect(dockcheckbox,SIGNAL(clicked()),this,SLOT(dockclicked()));

  cdAutoPlayCB = new QCheckBox(klocale->translate("Play on Tray Close"),
                               this, "cdAutoPlayCB");
  cdAutoPlayCB->setGeometry(30+XOFF, 285+YOFF, 200, 15);
  cdAutoPlayCB->setFixedSize( cdAutoPlayCB->sizeHint() );
  cdAutoPlayCB->setChecked(configdata.autoplay);
  connect(cdAutoPlayCB, SIGNAL(clicked()), this, SLOT(autoPlayClicked()));

  dockOnMinimizeCB = new QCheckBox(klocale->translate("AutoDock on Minimize"),
                                   this, "dockOnMinimizeCB");
  dockOnMinimizeCB->setGeometry(30+XOFF, 305+YOFF, 200, 15);
  dockOnMinimizeCB->setFixedSize( dockOnMinimizeCB->sizeHint() );
  dockOnMinimizeCB->setChecked(configdata.autodock);
  connect(dockOnMinimizeCB, SIGNAL(clicked()), this, SLOT(dockOnMinimizeClicked()));
  dockOnMinimizeCB->setEnabled(configdata.docking);

  stopOnExitCB = new QCheckBox(klocale->translate("Stop Playing on Exit"),
                               this, "stopOnExitCB");
  stopOnExitCB->setGeometry(30+XOFF, 325+YOFF, 200, 15);
  stopOnExitCB->setFixedSize( stopOnExitCB->sizeHint() );
  stopOnExitCB->setChecked(configdata.stopexit);
  connect(stopOnExitCB, SIGNAL(clicked()), this, SLOT(stopOnExitClicked()));

  ejectOnFinishCB = new QCheckBox(klocale->translate("Eject on Finish"),
                                  this, "ejectOnFinishCB");
  ejectOnFinishCB->setGeometry(30+XOFF, 345+YOFF, 200, 15);
  ejectOnFinishCB->setFixedSize( ejectOnFinishCB->sizeHint() );
  ejectOnFinishCB->setChecked(configdata.ejectonfinish);
  connect(ejectOnFinishCB, SIGNAL(clicked()), this, SLOT(ejectOnFinishClicked()));

  
  button3 = new QPushButton(this);
  button3->setGeometry( 420, 400, 90, 25 );
  //button3->setGeometry(255+XOFF,340+YOFF,100,25);
  button3->setText(klocale->translate("Help"));
  connect(button3,SIGNAL(clicked()),this,SLOT(help()));
}


void ConfigDlg::custombutton_clicked(){

    configdata.use_kfm = false;
    custom_edit->setEnabled(!configdata.use_kfm);
}


void ConfigDlg::kfmbutton_clicked(){

    configdata.use_kfm = true;
    custom_edit->setEnabled(!configdata.use_kfm);
}


void ConfigDlg::okbutton() {



}

void ConfigDlg::device_changed(const char* dev) {

  configdata.cd_device = dev;
}

void ConfigDlg::mail_changed(const char* dev) {

  configdata.mailcmd = dev;
}

void ConfigDlg::ttclicked(){

  if(ttcheckbox->isChecked())
    configdata.tooltips = TRUE;
  else
    configdata.tooltips = FALSE;

}

void ConfigDlg::dockclicked(){

    if(dockcheckbox->isChecked()){
        dockOnMinimizeCB->setEnabled(true);
        configdata.docking = TRUE;
    }else{
        dockOnMinimizeCB->setEnabled(false);
        configdata.docking = FALSE;
    }
}

void ConfigDlg::autoPlayClicked(){
    if(cdAutoPlayCB->isChecked())
        configdata.autoplay = TRUE;
    else
        configdata.autoplay = FALSE;
}

void ConfigDlg::dockOnMinimizeClicked()
{
    if(dockOnMinimizeCB->isChecked())
        configdata.autodock = TRUE;
    else
        configdata.autodock = FALSE;
}

void ConfigDlg::stopOnExitClicked()
{
    if(stopOnExitCB->isChecked())
        configdata.stopexit = TRUE;
    else
        configdata.stopexit = FALSE;
}

void ConfigDlg::ejectOnFinishClicked()
{
    if(ejectOnFinishCB->isChecked())
        configdata.ejectonfinish = TRUE;
    else
        configdata.ejectonfinish = FALSE;
}

void ConfigDlg::help(){

  if(mykapp)
    mykapp->invokeHTMLHelp("kscd/kscd.html","");
}

void ConfigDlg::cancelbutton() {
  reject();
}

void ConfigDlg::set_led_color(){


  KColorDialog::getColor(configdata.led_color);
  qframe1->setBackgroundColor(configdata.led_color);


}

void ConfigDlg::set_background_color(){

  KColorDialog::getColor(configdata.background_color);
  qframe2->setBackgroundColor(configdata.background_color);

}

struct configstruct * ConfigDlg::getData(){

  configdata.browsercmd = custom_edit->text();
  return &configdata;

}

#include "configdlg.moc"



