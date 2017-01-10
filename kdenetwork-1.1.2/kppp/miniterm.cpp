/*
 *            kPPP: A front end for pppd for the KDE project
 *
 * $Id: miniterm.cpp,v 1.23.2.1 1999/03/11 15:15:12 porten Exp $
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben 
 *                    wuebben@math.cornell.edu
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <kapp.h>
#include <kwm.h>
#include "pppdata.h"
#include "modem.h"
#include "miniterm.h"

#define T_WIDTH 550
#define T_HEIGHT 400

extern PPPData gpppdata;

MiniTerm::MiniTerm(QWidget *parent, const char *name)
  : QDialog(parent, name, TRUE)
{
  setCaption(i18n("Kppp Mini-Terminal"));
  KWM::setMiniIcon(winId(), kapp->getMiniIcon());

  m_file = new QPopupMenu;
  m_file->insertItem( i18n("&Close"),this, SLOT(cancelbutton()) );
  m_options = new QPopupMenu;
  m_options->insertItem(i18n("&Reset Modem"),this,SLOT(resetModem()));
  m_help = 
    kapp->getHelpMenu(TRUE, 
		      i18n("MiniTerm - A terminal emulation for KPPP\n\n"
			   "(c) 1997 Bernd Johannes Wuebben <wuebben@kde.org>\n"
			   "(c) 1998 Harri Porten <porten@kde.org>\n"
			   "(c) 1998 Mario Weilguni <mweilguni@kde.org>\n\n" 
			   "This program is published under the GNU GPL\n"
			   "(GNU General Public License)"
			   ));
  
  menubar = new QMenuBar( this );
  menubar->insertItem( i18n("&File"), m_file );
  menubar->insertItem( i18n("&Modem"), m_options );
  menubar->insertItem( i18n("&Help"), m_help);
  
  statusbar = new QLabel(this);
  statusbar->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  statusbar2 = new QLabel(this);
  statusbar2->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  terminal = new MyTerm(this,"term");

  setupToolbar();

  statusbar->setGeometry(0, T_HEIGHT - 20, T_WIDTH - 70, 20);
  statusbar2->setGeometry(T_WIDTH - 70, T_HEIGHT - 20, 70, 20);

  menubar->setGeometry(0,0,T_WIDTH,30);

  terminal->setGeometry(0, menubar->height() + toolbar->height() , 
   T_WIDTH,  T_HEIGHT - menubar->height() - toolbar->height() - statusbar->height());
 
  inittimer = new QTimer(this);
  connect(inittimer,SIGNAL(timeout()),this,SLOT(init()));
  inittimer->start(500);
}  


MiniTerm::~MiniTerm() {
  delete toolbar;
  delete statusbar;
  delete statusbar2;
}


void MiniTerm::setupToolbar() {
  toolbar = new KToolBar( this );

  KIconLoader *loader = kapp->getIconLoader();

  QPixmap pixmap;

  pixmap = loader->loadIcon("exit.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(cancelbutton()), TRUE, i18n("Close MiniTerm"));

  pixmap = loader->loadIcon("back.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(resetModem()), TRUE, i18n("Reset Modem"));

  pixmap = loader->loadIcon("help.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(help()), TRUE, i18n("Help"));

  toolbar->setBarPos( KToolBar::Top );
  toolbar->enableMoving(false);
  toolbar->updateRects(true);
}


void MiniTerm::resizeEvent(QResizeEvent*) {
  menubar->setGeometry(0,0,width(),30);
  toolbar->setGeometry(0,menubar->height(),width(),toolbar->height());
  terminal->setGeometry(0, menubar->height() + toolbar->height() , 
			width(),  height() - menubar->height() 
			- toolbar->height() - statusbar->height());
  statusbar->setGeometry(0, height() - 20, width() - 70, 20);
  statusbar2->setGeometry(width() - 70, height() - 20, 70, 20);
}


void MiniTerm::init() {
  inittimer->stop();
  statusbar->setText(i18n("Initializing Modem"));
  kapp->processEvents();

  int lock = Modem::modem->lockdevice();
  if (lock == 1) {
    statusbar->setText(i18n("Sorry, modem device is locked."));
    return;
  }

  if (lock == -1) {
    statusbar->setText(i18n("Sorry, can't create modem lock file."));
    return;
  }

  if(Modem::modem->opentty()) {
    if(Modem::modem->hangup()) {
    // send a carriage return and then wait a bit so that the modem will
    // let us issue commands.
    if(gpppdata.modemPreInitDelay() > 0) {
      usleep(gpppdata.modemPreInitDelay() * 5000);
      Modem::modem->writeLine("");
      usleep(gpppdata.modemPreInitDelay() * 5000);
    }
    Modem::modem->writeLine(gpppdata.modemInitStr());
    usleep(gpppdata.modemInitDelay() * 10000);
      
      statusbar->setText(i18n("Modem Ready"));
      terminal->setFocus();
      
      kapp->processEvents();
      kapp->processEvents();

      Modem::modem->notify(this, SLOT(readChar(unsigned char)));
      return;
    }
  }
  
  // opentty() or hangup() failed 
  statusbar->setText(Modem::modem->modemMessage());
  Modem::modem->unlockdevice();
}                  


void MiniTerm::readChar(unsigned char c) {

  switch((int)c) {
  case 8:
    terminal->backspace();
    break;
  case 10:
    terminal->mynewline();
    break;
  case 13:
    terminal->myreturn();
    break;
  case 127:
    terminal->backspace();
    break;
  default:
    terminal->insertChar(c);
  }
}


void MiniTerm::cancelbutton() {  
  Modem::modem->stop();

  statusbar->setText(i18n("Hanging up ..."));
  kapp->processEvents();
  kapp->flushX();

  Modem::modem->hangup();

  Modem::modem->closetty();
  Modem::modem->unlockdevice();

  reject();
}


void MiniTerm::resetModem() {
  statusbar->setText(i18n("Resetting Modem"));
  terminal->newLine();
  kapp->processEvents();
  kapp->flushX();

  Modem::modem->hangup();

  statusbar->setText(i18n("Modem Ready"));
}


void MiniTerm::closeEvent( QCloseEvent *e ) {
  cancelbutton();
  e->accept();
}


void MiniTerm::help() {
  kapp->invokeHTMLHelp("kppp/kppp.html","");
}


MyTerm::MyTerm(QWidget *parent, const char* name)
  : QMultiLineEdit(parent, name)
{
   this->setFont(QFont("courier",12,QFont::Normal));  
}

void MyTerm::keyPressEvent(QKeyEvent *k) {
  // ignore meta keys
  if (k->ascii() == 0)
    return;

  if(k->ascii() == 13)
    myreturn();

  Modem::modem->writeChar(k->ascii());
}


void MyTerm::insertChar(unsigned char c) {  
  QMultiLineEdit::insertChar(c);
}


void MyTerm::newLine() {  
  QMultiLineEdit::newLine();
}


void MyTerm::del() {
  QMultiLineEdit::del();
}


void MyTerm::backspace() {
  QMultiLineEdit::backspace();
}


void MyTerm::myreturn() {
  int column;
  int line;
  
  getCursorPosition(&line,&column);
  for (int i = 0; i < column;i++)
    QMultiLineEdit::cursorLeft();
}


void MyTerm::mynewline() {
  QMultiLineEdit::end(FALSE);
  QMultiLineEdit::newLine();
}

#include "miniterm.moc"
