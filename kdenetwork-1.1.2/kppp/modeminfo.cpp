/*
 *            kPPP: A front end for pppd for the KDE project
 *
 * $Id: modeminfo.cpp,v 1.20.2.1 1999/03/11 15:15:17 porten Exp $
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben 
 *                    wuebben@math.cornell.edu
 *
 * This file contributed by: Markus Wuebben, mwuebben@fiwi02.wiwi.uni-tuebingen.de
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
#include <qregexp.h>
#include <kapp.h> 
#include <kwm.h>
#include "modeminfo.h"
#include "macros.h"
#include "modem.h"

ModemTransfer::ModemTransfer(QWidget *parent, const char *name)
  : QDialog(parent, name,TRUE, WStyle_Customize|WStyle_NormalBorder)
{
  setCaption(i18n("ATI Query"));
  KWM::setMiniIcon(winId(), kapp->getMiniIcon());

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
  
  progressBar = new KProgress(0, 8, 0, KProgress::Horizontal, this, "bar");
  progressBar->setBarStyle(KProgress::Blocked);
  
  statusBar = new QLabel(this,"sBar");
  statusBar->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  statusBar->setAlignment(AlignCenter);

  // This is a rather complicated case. Since we do not know which
  // message is the widest in the national language, we'd to
  // search all these messages. This is a little overkill, so I take
  // the longest english message, translate it and give it additional
  // 20 percent space. Hope this is enough.
  statusBar->setText(i18n("Sorry, can't create modem lock file."));
  statusBar->setMinimumWidth((statusBar->sizeHint().width() * 12) / 10);
  statusBar->setMinimumHeight(statusBar->sizeHint().height() + 4);

  // set original text
  statusBar->setText(i18n("Looking for Modem ..."));
  progressBar->setFixedHeight(statusBar->minimumSize().height());
  tl->addWidget(progressBar);
  tl->addWidget(statusBar);

  cancel = new QPushButton(i18n("Cancel"), this);
  cancel->setFocus();
  connect(cancel, SIGNAL(clicked()), SLOT(cancelbutton()));
  FIXED_SIZE(cancel);

  QHBoxLayout *l1 = new QHBoxLayout;
  tl->addLayout(l1);
  l1->addStretch(1);
  l1->addWidget(cancel);

  step = 0;

  ////////////////////////////////////////////////

  timeout_timer = new QTimer(this);
  connect(timeout_timer, SIGNAL(timeout()), SLOT(time_out_slot()));

  scripttimer = new QTimer(this);
  connect(scripttimer, SIGNAL(timeout()), SLOT(do_script()));

  timeout_timer->start(15000,TRUE); // 15 secs single shot
  QTimer::singleShot(500, this, SLOT(init()));

  tl->freeze();
}


void ModemTransfer::ati_done() {
  scripttimer->stop();
  timeout_timer->stop();
  Modem::modem->closetty();
  Modem::modem->unlockdevice();
  hide();

  // open the result window
  ModemInfo *mi = new ModemInfo(this);
  for(int i = 0; i < NUM_OF_ATI; i++)
    mi->setAtiString(i, ati_query_strings[i]);
  mi->exec();
  delete mi;

  accept();
}


void ModemTransfer::time_out_slot() {
  timeout_timer->stop();
  scripttimer->stop();

  QMessageBox::warning(this, 
		       i18n("Error"),
		       i18n("Modem Query timed out."));
  reject();
}


void ModemTransfer::init() {

  kapp->processEvents();

  int lock = Modem::modem->lockdevice();
  if (lock == 1) {
    
    statusBar->setText(i18n("Sorry, modem device is locked."));
    return;
  }

  if (lock == -1) {
    
    statusBar->setText(i18n("Sorry, can't create modem lock file."));
    return;
  }


  if(Modem::modem->opentty()) {
    if(Modem::modem->hangup()) {
      usleep(100000);  // wait 0.1 secs
      Modem::modem->writeLine("ATE0Q1V1"); // E0 don't echo the commands I send ...

      statusBar->setText(i18n("Modem Ready"));
      kapp->processEvents();
      usleep(100000);  // wait 0.1 secs
      kapp->processEvents();
      scripttimer->start(1000);	 	// this one does the ati query

      // clear modem buffer
      Modem::modem->flush();

      Modem::modem->notify(this, SLOT(readChar(unsigned char)));
      return;
    }
  }
  
  // opentty() or hangup() failed 
  statusBar->setText(Modem::modem->modemMessage());
  step = 99; // wait until cancel is pressed
  Modem::modem->unlockdevice();  
}                  


void ModemTransfer::do_script() {
  QString msg;
  QString query;

  switch(step) {
  case 0:
    readtty();
    statusBar->setText("ATI ...");
    progressBar->advance(1);
    Modem::modem->writeLine("ATI\n");
    break;

  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    readtty();
    msg.sprintf("ATI %d ...", step);
    query.sprintf("ATI%d\n", step);
    statusBar->setText(msg.data());
    progressBar->advance(1);
    Modem::modem->writeLine(query.data());
    break;

  default:
    readtty();
    ati_done();
  }
  step++;
}

void ModemTransfer::readChar(unsigned char c) {
  if(readbuffer.length() < 255)
    readbuffer += c;
}

void ModemTransfer::readtty() {

  if (step == 0)
    return;

  readbuffer.replace("\n"," ");         // remove stray \n
  readbuffer.replace("\r","");          // remove stray \r
  readbuffer = readbuffer.stripWhiteSpace(); // strip of leading or trailing white
                                                 // space

  if(step <= NUM_OF_ATI)
    ati_query_strings[step-1] = readbuffer.copy();

  readbuffer = "";
}


void ModemTransfer::cancelbutton() {
  scripttimer->stop();
  Modem::modem->stop();
  timeout_timer->stop();

  statusBar->setText(i18n("One Moment Please ..."));
  kapp->processEvents();
  
  Modem::modem->hangup();

  Modem::modem->closetty();
  Modem::modem->unlockdevice();
  reject();
}


void ModemTransfer::closeEvent( QCloseEvent *e ) {
  cancelbutton();
  e->accept();
}


ModemInfo::ModemInfo(QWidget *parent, const char* name)
  : QDialog(parent, name, TRUE, WStyle_Customize|WStyle_NormalBorder)
{
  QString label_text;

  setCaption(i18n("Modem Query Results"));
  KWM::setMiniIcon(winId(), kapp->getMiniIcon());

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

  QGridLayout *l1 = new QGridLayout(NUM_OF_ATI, 2, 5);
  tl->addLayout(l1, 1);
  for(int  i = 0 ; i < NUM_OF_ATI ; i++) {

    label_text = "";
    if ( i == 0)
      label_text.sprintf("ATI :");
    else
      label_text.sprintf("ATI %d:", i );

    ati_label[i] = new QLabel(label_text.data(), this);
    MIN_SIZE(ati_label[i]);
    l1->addWidget(ati_label[i], i, 0);

    ati_label_result[i] =  new QLineEdit(this);
    MIN_SIZE(ati_label_result[i]);
    ati_label_result[i]->setMinimumWidth(fontMetrics().width('H') * 24);
    FIXED_HEIGHT(ati_label_result[i]);
    l1->addWidget(ati_label_result[i], i, 1); 
  }
  //tl->addSpacing(1);

  QHBoxLayout *l2 = new QHBoxLayout;
  QPushButton *ok = new QPushButton(i18n("Close"), this);
  ok->setDefault(TRUE);
  ok->setFocus();

  // Motif default buttons + Qt layout do not work tight together
  if(ok->style() == MotifStyle) {
    ok->setFixedWidth(ok->sizeHint().width() + 10);
    ok->setFixedHeight(ok->sizeHint().height() + 10);
    tl->addSpacing(8);
  } else
    FIXED_SIZE(ok);

  tl->addLayout(l2);
  l2->addStretch(1);

  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  l2->addWidget(ok);
  
  tl->freeze();
}


void ModemInfo::setAtiString(int i, QString s) {
  if(i < NUM_OF_ATI)
    ati_label_result[i]->setText(s.data());
}

#include "modeminfo.moc"
