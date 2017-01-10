/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: general.cpp,v 1.48.2.4 1999/08/17 16:26:51 porten Exp $
 * 
 *            Copyright (C) 1997 Bernd Johannes Wuebben 
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include <kapp.h>
#include <qlayout.h>
#include <kintegerline.h>
#include <kquickhelp.h>
#include <kslider.h>
#include "general.h"
#include "version.h"
#include "macros.h"
#include "log.h"
#include "groupbox.h"
#include "newwidget.h"
#include "miniterm.h"
#include "modeminfo.h"
#include "modemcmds.h"
#include "devices.h"


/////////////////////////////////////////////////////////////////////////////
//
// Widget containing misc. configuration options
//
/////////////////////////////////////////////////////////////////////////////
GeneralWidget::GeneralWidget( QWidget *parent, const char *name)
  : KGroupBox(i18n("kppp Setup"), parent, name)
{
  QVBoxLayout *tl = new QVBoxLayout(peer(), 10, 10);
  QHBoxLayout *l1 = new QHBoxLayout(10);
  tl->addLayout(l1);

  label6 = newLabel(i18n("pppd Timeout:"), peer());
  l1->addWidget(label6);
  
  pppdtimeout = new KIntegerLine(peer());
  pppdtimeout->setFixedHeight(pppdtimeout->sizeHint().height());
  pppdtimeout->setMaxLength(TIMEOUT_SIZE);
  pppdtimeout->setMinimumWidth(pppdtimeout->sizeHint().width()/4);
  pppdtimeout->setText(gpppdata.pppdTimeout());
  connect(pppdtimeout, SIGNAL(textChanged(const char*)),
	  SLOT(pppdtimeoutchanged(const char*)));
  l1->addWidget(pppdtimeout);
  KQuickHelp::add(label6,
  KQuickHelp::add(pppdtimeout, 
		  i18n("<i>kppp</i> will wait this number of seconds\n"
		       "to see if a PPP connection is established.\n"
		       "If no connection is made in this time frame,\n"
		       "<i>kppp</i> will give up and kill pppd.")));

  labeltmp = newLabel(i18n("Seconds"), peer());
  l1->addWidget(labeltmp);

  tl->addStretch(1);
  QHBoxLayout *lh = new QHBoxLayout();
  tl->addLayout(lh);
  QVBoxLayout *l2 = new QVBoxLayout();
  lh->addLayout(l2, 1);
  QVBoxLayout *l3 = new QVBoxLayout();
  lh->addLayout(l3, 0);
  QVBoxLayout *l4 = new QVBoxLayout();
  lh->addLayout(l4, 1);

  chkbox6 = newCheckBox(i18n("Dock into Panel on Connect"), peer());
  KQuickHelp::add(chkbox6, 
		  i18n("After a connection is established, the\n"
		       "window is minimized and a small icon\n"
		       "in the <i>kpanel</i> represents this window.\n"
		       "\n"
		       "Clicking on this icon will restore the\n"
		       "window to it's original location and\n"
		       "size."));

  chkbox6->setChecked(gpppdata.get_dock_into_panel());
  connect(chkbox6,SIGNAL(toggled(bool)), 
	  this, SLOT(docking_toggled(bool)));
  l3->addWidget(chkbox6);

  chkbox2 = newCheckBox(i18n("Automatic Redial on Disconnect"), peer());
  chkbox2->setChecked(gpppdata.automatic_redial());
  connect(chkbox2,SIGNAL(toggled(bool)),
	  this, SLOT(redial_toggled(bool)));
  l3->addWidget(chkbox2);
  KQuickHelp::add(chkbox2, 
		  i18n("When a connection is established and\n"
		       "it somehow gets disconnected, <i>kppp</i>\n"
		       "will try to reconnect to the same account.\n"
		       "\n"
		       "See <link #redial>here</link> for more on this topic."));

  chkbox3 = newCheckBox(i18n("Show Clock on Caption"), peer());
  chkbox3->setChecked(gpppdata.get_show_clock_on_caption());
  connect(chkbox3, SIGNAL(toggled(bool)),
	  this, SLOT(caption_toggled(bool)));
  l3->addWidget(chkbox3);
  KQuickHelp::add(chkbox3, 
		  i18n("When this option is checked, the window\n"
		       "title shows the time since a connection\n"
		       "was established. Very useful, so you \n"
		       "should turn this on"));

  chkbox4 = newCheckBox(i18n("Disconnect on X-server shutdown"), peer());
  chkbox4->setChecked(gpppdata.get_xserver_exit_disconnect());
  connect(chkbox4, SIGNAL(toggled(bool)),
	  this, SLOT(xserver_toggled(bool)));
  l3->addWidget(chkbox4);
  KQuickHelp::add(chkbox4, 
		  i18n("Checking this option will close any\n"
		       "open connection when the X-server is\n"
		       "shut down. You should enable this option\n"
		       "unless you know what you are doing.\n"
		       "\n"
		       "See <link #disxserver>here</link> for more on this."));

  chkbox7 = newCheckBox(i18n("Quit on Disconnect"), peer());
  chkbox7->setChecked(gpppdata.quit_on_disconnect());
  connect(chkbox7, SIGNAL(toggled(bool)),
	  this, SLOT(quit_toggled(bool)));
  l3->addWidget(chkbox7);
  KQuickHelp::add(chkbox7, 
		  i18n("When this option is turned on, <i>kppp</i>\n"
		       "will be closed when you disconnect"));

  chkbox5 = newCheckBox(i18n("Minimize Window on Connect"), peer());
  chkbox5->setChecked(gpppdata.get_iconify_on_connect());
  connect(chkbox5,SIGNAL(toggled(bool)),
	  this,SLOT(iconify_toggled(bool)));
  l3->addWidget(chkbox5);
  KQuickHelp::add(chkbox5, 
		  i18n("Iconifies <i>kppp</i>'s window when a\n"
		       "connection is established"));

  tl->activate();
}


void GeneralWidget::docking_toggled(bool on){
  gpppdata.set_dock_into_panel(on);
}


void GeneralWidget::iconify_toggled(bool on){
  gpppdata.set_iconify_on_connect(on);
}

 
void GeneralWidget::caption_toggled(bool on){
  gpppdata.set_show_clock_on_caption(on);
}


void GeneralWidget::redial_toggled(bool on){
  gpppdata.set_automatic_redial(on);
}


void GeneralWidget::xserver_toggled(bool on){
  gpppdata.set_xserver_exit_disconnect(on);
}


void GeneralWidget::quit_toggled(bool on){
  gpppdata.set_quit_on_disconnect(on);
}


void GeneralWidget::pppdtimeoutchanged(const char *n) {
  gpppdata.setpppdTimeout(n);

}


/////////////////////////////////////////////////////////////////////////////
//
// The About Dialog
//
/////////////////////////////////////////////////////////////////////////////
AboutWidget::AboutWidget( QWidget *parent, const char *name)
  : KGroupBox(i18n("About kppp"), parent, name)
{
  QVBoxLayout *tl = new QVBoxLayout(peer(), 10);
  QLabel *label1 = new QLabel(peer());
  label1->setAlignment(AlignLeft|ExpandTabs);

  QString string;
  string = "kppp "KPPPVERSION;
  string += i18n("\nA dialer and front-end to pppd\n\n"
		 "(c) 1997, 1998\n"
		 "    Bernd Johannes Wuebben <wuebben@kde.org>\n"
		 "    Harri Porten <porten@kde.org>\n"
		 "    Mario Weilguni <mweilguni@kde.org>\n\n"
		 "Original author: Bernd Johannes Wuebben\n"
		 "Currently maintained by Harri Porten and Mario\n"
		 "Weilguni. Please send all bug reports to the\n"
		 "current maintainer.\n\n"
		 "This program is distributed under the GNU GPL\n"
		 "(GNU General Public License)."
		 );
  label1->setText(string);
  label1->setMinimumSize(label1->sizeHint());
  tl->addWidget(label1);
  tl->activate();
}



ModemWidget::ModemWidget( QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  int k;

  QGridLayout *tl = new QGridLayout(this, 11, 4, 10, 10);
  tl->addRowSpacing(0, fontMetrics().lineSpacing() - 10); // magic

  box = new QGroupBox(i18n("Serial device"), this);
  tl->addMultiCellWidget(box, 0, 9, 0, 3);

  label1 = newLabel(i18n("Modem Device:"), this);
  tl->addWidget(label1, 1, 1);
  
  modemdevice = new QComboBox(false,this);

  for(k = 0; devices[k]; k++)
    modemdevice->insertItem(devices[k]);

  modemdevice->setMinimumWidth(modemdevice->sizeHint().width());
  modemdevice->setFixedHeight(modemdevice->sizeHint().height());
  tl->addWidget(modemdevice, 1, 2);
  connect(modemdevice, SIGNAL(activated(int)), SLOT(setmodemdc(int)));
  KQuickHelp::add(label1,
  KQuickHelp::add(modemdevice, 
		  i18n("This specifies the serial port your modem is attached \n"
		       "to. On Linux/x86, typically this is either /dev/ttyS0 \n"
		       "(COM1 under DOS) or /dev/ttyS1 (COM2 under DOS).\n"
		       "\n"
		       "If you have an internal ISDN card with AT command\n"
		       "emulation (most cards under Linux support this), you\n"
		       "should select one of the /dev/ttyIx devices.")));


  label2 = newLabel(i18n("Flow Control:"), this);
  tl->addWidget(label2, 2, 1);

  flowcontrol = new QComboBox(false,this);
  flowcontrol->insertItem("CRTSCTS");
  flowcontrol->insertItem("XON/XOFF");
  flowcontrol->insertItem(i18n("None"));
  flowcontrol->setMinimumWidth(flowcontrol->sizeHint().width());
  flowcontrol->setFixedHeight(flowcontrol->sizeHint().height());
  tl->addWidget(flowcontrol, 2, 2);
  connect(flowcontrol, SIGNAL(activated(int)), 
	  SLOT(setflowcontrol(int)));

  KQuickHelp::add(label2,
  KQuickHelp::add(flowcontrol, 
		  i18n("Specifies how the serial port and modem\n"
		       "communicate. You should not change this unless\n"
		       "you know what you are doing.\n"
		       "\n"
		       "<b>Default</b>: CRTSCTS")));

  labelenter = newLabel(i18n("Line Termination:"), this);
  tl->addWidget(labelenter, 3, 1);

  enter = new QComboBox(false,this);
  enter->insertItem("CR");
  enter->insertItem("LF");
  enter->insertItem("CR/LF");
  enter->setMinimumWidth(enter->sizeHint().width());
  enter->setFixedHeight(enter->sizeHint().height());
  tl->addWidget(enter, 3, 2);
  connect(enter, SIGNAL(activated(int)), SLOT(setenter(int)));
  KQuickHelp::add(labelenter,
  KQuickHelp::add(enter, 
		  i18n("Specifies how AT commands are send to your\n"
		       "modem. Most modems will work fine with the\n"
		       "default <i>CR</i>. If your modem does not react\n"
		       "to the init string, you should try different\n"
		       "settings here\n"
		       "\n"
		       "<b>Default</b>: CR")));

  baud_label = newLabel(i18n("Connection Speed:"), this);
  tl->addWidget(baud_label, 4, 1);  
  
  QHBoxLayout *l1 = new QHBoxLayout;
  tl->addLayout(l1, 4, 2);
  baud_c = new QComboBox(this);

  static const char *baudrates[] = {
    
#ifdef B460800 
    "460800",
#endif

#ifdef B230400
    "230400",
#endif

#ifdef B115200
    "115200",
#endif

#ifdef B57600
    "57600",
#endif

    "38400",
    "19200",
    "9600",
    "2400",
    0};

  for(k = 0; baudrates[k]; k++)
    baud_c->insertItem(baudrates[k]);
  
  connect(baud_c, SIGNAL(activated(int)),
	  this, SLOT(speed_selection(int)));
  FIXED_HEIGHT(baud_c);
  MIN_WIDTH(baud_c);
  l1->addWidget(baud_c);
  l1->addStretch(1);
  KQuickHelp::add(baud_label,
  KQuickHelp::add(baud_c, 
		  i18n("Specifies the speed your modem and the serial\n"
		       "port talk to each other. You should begin with\n"
		       "the default of 38400 bits/sec. If everything\n"
		       "works you can try to increase this value, but to\n"
		       "no more than 115200 bits/sec (unless you know\n"
		       "that your serial port supports higher speeds).")));

  for(int i=0; i <= enter->count()-1; i++) {
    if(strcmp(gpppdata.enter(), enter->text(i)) == 0)
      enter->setCurrentItem(i);
  }



  //Modem Lock File
  modemlockfile = newCheckBox(i18n("Use Lock File"), this);

  modemlockfile->setChecked(gpppdata.modemLockFile());
  connect(modemlockfile, SIGNAL(toggled(bool)),
          SLOT(modemlockfilechanged(bool)));
  QHBoxLayout *l12 = new QHBoxLayout;
  tl->addLayout(l12, 6, 1);
  //  l12->addStretch(1);
  l12->addWidget(modemlockfile);
  //  l12->addStretch(1);
  KQuickHelp::add(modemlockfile, 
		  i18n("To prevent other programs from accessing the\n"
		       "modem while a connection is established, a\n"
		       "file can be created to indicate that the modem\n"
		       "is in use. On Linux an example file would be\n"
                       "<i>/var/lock/LCK..ttyS1</i>\n"
                       "Here you can select whether this locking will\n"
		       "be done.\n"
		       "\n"
                       "<b>Default</b>: On"));
  
  // Modem Timeout Line Edit Box
  label3 = newLabel(i18n("Modem Timeout:"), this);
  tl->addWidget(label3, 7, 1);

  QHBoxLayout *l2 = new QHBoxLayout;
  tl->addLayout(l2, 7, 2);

  modemtimeout = new KIntegerLine(this);
  modemtimeout->setFixedHeight(modemtimeout->sizeHint().height());
  modemtimeout->setMaxLength(TIMEOUT_SIZE);
  modemtimeout->setMinimumWidth(fontMetrics().width('8')*3);
  modemtimeout->setText(gpppdata.modemTimeout());
  connect(modemtimeout, SIGNAL(textChanged(const char*)),
	  SLOT(modemtimeoutchanged(const char*)));  
  l2->addWidget(modemtimeout, 1);

  labeltmp = newLabel(i18n("Seconds"), this);
  l2->addWidget(labeltmp, 2);
  KQuickHelp::add(label3,
  KQuickHelp::add(modemtimeout, 
		  i18n("This specifies how long <i>kppp</i> waits for a\n"
		       "<i>CONNECT</i> response from your modem. The\n"
		       "recommended value is 30 seconds.")));


  //set stuff from gpppdata
  for(int i=0; i <= modemdevice->count()-1; i++) {
    if(strcmp(gpppdata.modemDevice(), modemdevice->text(i)) == 0)
      modemdevice->setCurrentItem(i);
  }

  for(int i=0; i <= flowcontrol->count()-1; i++) {
    if(strcmp(gpppdata.flowcontrol(), flowcontrol->text(i)) == 0)
      flowcontrol->setCurrentItem(i);
  }     

  //set the modem speed
  for(int i=0; i < baud_c->count(); i++)
    if(strcmp(baud_c->text(i), gpppdata.speed()) == 0)
      baud_c->setCurrentItem(i);

  tl->activate();
}


void ModemWidget::speed_selection(int) {
  gpppdata.setSpeed(baud_c->text(baud_c->currentItem()));
}


void ModemWidget::setenter(int ) {
  gpppdata.setEnter(enter->text(enter->currentItem()));
}


void ModemWidget::setmodemdc(int i) {
  gpppdata.setModemDevice(modemdevice->text(i));
}


void ModemWidget::setflowcontrol(int i) {
  gpppdata.setFlowcontrol(flowcontrol->text(i));
}


void ModemWidget::modemlockfilechanged(bool set) {
  gpppdata.setModemLockFile(set);
}


void ModemWidget::modemtimeoutchanged(const char *n) {
  gpppdata.setModemTimeout(n);
}


ModemWidget2::ModemWidget2( QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  QGridLayout *tl = new QGridLayout(this, 3, 3, 10, 10);
  tl->addRowSpacing(0, fontMetrics().lineSpacing() - 10); // magic

  box = new QGroupBox(i18n("Modem"), this);
  tl->addMultiCellWidget(box, 0, 2, 0, 2);

  QVBoxLayout *l1 = new QVBoxLayout;
  tl->addLayout(l1, 1, 1);
  tl->setColStretch(1, 1);
  tl->setRowStretch(1, 1);
  l1->addStretch(1);

  QHBoxLayout *l10 = new QHBoxLayout;
  l1->addLayout(l10);
  label4 = newLabel(i18n("Busy Wait:"), this);
  l10->addStretch(1);
  l10->addWidget(label4);

  busywait = new KIntegerLine(this);
  busywait->setFixedHeight(busywait->sizeHint().height());
  busywait->setMaxLength(TIMEOUT_SIZE);
  busywait->setText(gpppdata.busyWait());
  busywait->setMinimumWidth(busywait->sizeHint().width()/3);
  connect(busywait, SIGNAL(textChanged(const char*)),
	  SLOT(busywaitchanged(const char*)));
  l10->addWidget(busywait);

  labeltmp = newLabel(i18n("Seconds"), this);
  l10->addWidget(labeltmp, 1);
  l10->addStretch(1);
  KQuickHelp::add(label4,
  KQuickHelp::add(busywait, 
		  i18n("Specifies the number of seconds to wait before\n"
		       "redial if all dialed numbers are busy. This is\n"
		       "necessary because some modems get stuck if the\n"
		       "same number is busy too often.\n"
		       "\n"
		       "The default is 0 seconds, you should not change\n"
		       "this unless you need to.")));

  l1->addSpacing(10);
  l1->addStretch(1);

  QHBoxLayout *hbl = new QHBoxLayout;
  l1->addLayout(hbl);
  QLabel *volumeLabel = newLabel(i18n("Modem volume"), this);
  volumeLabel->setAlignment(AlignVCenter|AlignRight);
  hbl->addStretch(1);
  hbl->addWidget(volumeLabel);
  volume = new KSlider(0, 2, 1, gpppdata.volume(), KSlider::Horizontal, this);
  volume->setFixedSize(120, 25);  
  hbl->addWidget(volume);
  hbl->addStretch(1);
  connect(volume, SIGNAL(valueChanged(int)),
	  this, SLOT(volumeChanged(int)));
  KQuickHelp::add(volumeLabel,
  KQuickHelp::add(volume, 
		  i18n("Most modems have a speaker which makes\n"
		       "a lot of noise when dialing. Here you can\n"
		       "either turn this completely off or select a\n"
		       "lower volume.\n"
		       "\n"
		       "If this does not work for your modem,\n"
		       "you must modify the modem volume command")));

#if 0
  QHBoxLayout *l12 = new QHBoxLayout;
  l1->addLayout(l12);
  l12->addStretch(1);
  chkbox1 = newCheckBox(i18n("Modem Asserts CD Line."), this);
  chkbox1->setChecked(gpppdata.UseCDLine());
  connect(chkbox1,SIGNAL(toggled(bool)),
	  this,SLOT(use_cdline_toggled(bool)));
  l12->addWidget(chkbox1);
  l12->addStretch(1);
  l1->addStretch(1);
  KQuickHelp::add(chkbox1, 
		  i18n("This controls how <i>kppp</i> detects that the modem\n"
		       "is not responding. Unless you are having\n"
		       "problems with this, do not modify this setting.\n"
		       "\n"
		       "<b>Default</b>: off"));
#endif

  // add the buttons 
  QHBoxLayout *l11 = new QHBoxLayout;
  l1->addLayout(l11); 
  l11->addStretch(1);
  QVBoxLayout *l111 = new QVBoxLayout;
  l11->addLayout(l111);
  modemcmds = new QPushButton(i18n("Modem Commands"), this);
  KQuickHelp::add(modemcmds,
		  i18n("Allows you to change the AT command for\n"
		       "your modem."));
  
  modeminfo_button = new QPushButton(i18n("Query Modem"), this);
  KQuickHelp::add(modeminfo_button, 
		  i18n("Most modems support the ATI command set to\n"
		       "find out vendor and revision of your modem.\n"
		       "\n"
		       "Press this button to query your modem for\n"
		       "this information.  It can be useful to help\n"
		       "you setup the modem"));

  terminal_button = new QPushButton(i18n("Terminal"), this);
  KQuickHelp::add(terminal_button, 
		  i18n("Opens the built-in terminal program. You\n"
		       "can use this if you want to play around\n"
		       "with your modem's AT command set"));

  modemcmds->setMinimumWidth(modemcmds->sizeHint().width());
  modemcmds->setFixedHeight(modemcmds->sizeHint().height());
  modeminfo_button->setMinimumWidth(modeminfo_button->sizeHint().width());
  modeminfo_button->setFixedHeight(modeminfo_button->sizeHint().height());
  terminal_button->setMinimumWidth(terminal_button->sizeHint().width());
  terminal_button->setFixedHeight(terminal_button->sizeHint().height());
  l111->addWidget(modemcmds);
  l111->addWidget(modeminfo_button);
  l111->addWidget(terminal_button);
  l11->addStretch(1);
  l1->addStretch(1);

  tl->activate();

  connect(modemcmds, SIGNAL(clicked()), SLOT(modemcmdsbutton()));
  connect(modeminfo_button, SIGNAL(clicked()), SLOT(query_modem()));
  connect(terminal_button, SIGNAL(clicked()), SLOT(terminal()));
}


void ModemWidget2::modemcmdsbutton() {
  ModemCommands mc(this);
  mc.exec();
}


void ModemWidget2::query_modem() {  
  modemtrans = new ModemTransfer(this);
  modemtrans->exec();
  delete modemtrans;
}


void ModemWidget2::terminal() {
  MiniTerm terminal(NULL,NULL);
  terminal.exec();
}

#if 0
void ModemWidget2::use_cdline_toggled(bool on) {
    gpppdata.setUseCDLine(on);
}
#endif

void ModemWidget2::busywaitchanged(const char *n) {
  gpppdata.setbusyWait(n);
}


void ModemWidget2::volumeChanged(int v) {
  gpppdata.setVolume(v);
}


/////////////////////////////////////////////////////////////////////////////
//
// Setup widget for the graph
//
/////////////////////////////////////////////////////////////////////////////
GraphSetup::GraphSetup(QWidget *parent, const char *name) :
  KCheckGroupBox(i18n("Throughput graph"), parent, name)
{
  connect(this, SIGNAL(toggled(bool)), SLOT(enableToggled(bool)));
  QGridLayout *tl = new QGridLayout(peer(), 4, 2, 10, 10);

  bool enable;
  QColor bg, text, in, out;
  gpppdata.graphingOptions(enable, bg, text, in, out);

  bg_text = newLabel(i18n("Background color"), peer());
  bg_text->setAlignment(AlignRight|AlignVCenter);
  tl->addWidget(bg_text, 0, 0);  
  bg_color = new KColorButton(bg, peer());
  bg_color->setFixedSize(80, 24);
  tl->addWidget(bg_color, 0, 1);

  text_text = newLabel(i18n("Text color"), peer());
  text_text->setAlignment(AlignRight|AlignVCenter);
  tl->addWidget(text_text, 1, 0);
  text_color = new KColorButton(text, peer());
  text_color->setFixedSize(80, 24);
  tl->addWidget(text_color, 1, 1);

  in_text = newLabel(i18n("Input bytes color"), peer());
  in_text->setAlignment(AlignRight|AlignVCenter);
  tl->addWidget(in_text, 2, 0);
  in_color = new KColorButton(in, peer());
  in_color->setFixedSize(80, 24);
  tl->addWidget(in_color, 2, 1);

  out_text = newLabel(i18n("Output bytes color"), peer());
  out_text->setAlignment(AlignRight|AlignVCenter);
  tl->addWidget(out_text, 3, 0);
  out_color = new KColorButton(out, peer());
  out_color->setFixedSize(80, 24);
  tl->addWidget(out_color, 3, 1);

  connect(bg_color, SIGNAL(changed(const QColor &)), 
	  SLOT(colorChanged(const QColor&)));
  connect(text_color, SIGNAL(changed(const QColor &)), 
	  SLOT(colorChanged(const QColor&)));
  connect(in_color, SIGNAL(changed(const QColor &)), 
	  SLOT(colorChanged(const QColor&)));
  connect(out_color, SIGNAL(changed(const QColor &)), 
	  SLOT(colorChanged(const QColor&)));

  tl->activate();

  setChecked(enable);
  enableToggled(enable);
}

void GraphSetup::enableToggled(bool b) {
  out_text->setEnabled(b);
  in_text->setEnabled(b);
  text_text->setEnabled(b);
  bg_text->setEnabled(b);
  
  out_color->setEnabled(b);
  in_color->setEnabled(b);
  bg_color->setEnabled(b);
  text_color->setEnabled(b);
  
  save();
}


void GraphSetup::colorChanged(const QColor &) {
  save();
}


void GraphSetup::save() {
  gpppdata.setGraphingOptions(isChecked(),
			      bg_color->color(),
			      text_color->color(),
			      in_color->color(),
			      out_color->color());
}
  

#include "general.moc"
