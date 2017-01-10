/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: connect.cpp,v 1.98.2.10 1999/08/08 15:07:55 porten Exp $
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

#include <config.h>

#include <qdir.h>
#include <qregexp.h> 
#include <kmsgbox.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include <errno.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef linux
#include "runtests.h"
#endif

#include "auth.h"
#include "requester.h"
#include "connect.h"
#include "main.h"
#include "kpppconfig.h"
#include "pppdata.h"
#include "macros.h"
#include "docking.h"
#include "log.h"
#include "modem.h"

extern KPPPWidget *p_kppp;
extern int if_is_up();
extern QString local_ip_address;
extern bool quit_on_disconnect;

QString old_hostname;
bool modified_hostname;

extern int totalbytes;

ConnectWidget::ConnectWidget(QWidget *parent, const char *name)
  : QWidget(parent, name),
    // initialize some important variables
    myreadbuffer(""),
    main_timer_ID(0),
    vmain(0),  
    scriptindex(0),
    loopnest(0),
    loopend(false),
    semaphore(false),
    expecting(false),
    readbuffer(""),
    scanvar(""),
    scanning(false),
    pausing(false),
    termwindow(0),
    dialnumber(0)
{
  modified_hostname = false;

  QVBoxLayout *tl = new QVBoxLayout(this, 8, 10);
  QString tit = i18n("Connecting to: ");
  setCaption(tit);

  QHBoxLayout *l0 = new QHBoxLayout(10);
  tl->addLayout(l0);
  l0->addSpacing(10);
  messg = new QLabel(this, "messg");
  messg->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  messg->setAlignment(AlignCenter);
  messg->setText(i18n("Sorry, can't create modem lock file."));  
  messg->setMinimumHeight(messg->sizeHint().height() + 5);
  int messw = (messg->sizeHint().width() * 12) / 10;
  messw = QMAX(messw,280);
  messg->setMinimumWidth(messw);
  messg->setText(i18n("Looking for Modem ..."));
  l0->addSpacing(10);
  l0->addWidget(messg);
  l0->addSpacing(10);

  QHBoxLayout *l1 = new QHBoxLayout(10);
  tl->addLayout(l1);
  l1->addStretch(1);
  
  debug = new QPushButton(i18n("Log"), this);
  FIXED_HEIGHT(debug);
  connect(debug, SIGNAL(clicked()), SLOT(debugbutton()));

  cancel = new QPushButton(i18n("Cancel"), this);
  FIXED_HEIGHT(cancel);
  cancel->setFocus();
  connect(cancel, SIGNAL(clicked()), SLOT(cancelbutton()));

  int maxw = QMAX(cancel->sizeHint().width(),
		 debug->sizeHint().width());
  maxw = QMAX(maxw,65);
  debug->setFixedWidth(maxw);
  cancel->setFixedWidth(maxw);
  l1->addWidget(debug);
  l1->addWidget(cancel);

  tl->freeze();

  pausetimer = new QTimer(this);
  connect(pausetimer, SIGNAL(timeout()), SLOT(pause()));

  kapp->processEvents();

  timeout_timer = new QTimer(this);
  connect(timeout_timer, SIGNAL(timeout()), SLOT(script_timed_out()));
  
  inittimer = new QTimer(this);
  connect(inittimer, SIGNAL(timeout()), SLOT(init()));

  if_timeout_timer = new QTimer(this);
  connect(if_timeout_timer, SIGNAL(timeout()), SLOT(if_waiting_timed_out()));
  
  connect(this,SIGNAL(if_waiting_signal()),this,SLOT(if_waiting_slot()));

  prompt = new PWEntry( this, "pw" );         
  if_timer = new QTimer(this);
  connect(if_timer,SIGNAL(timeout()), SLOT(if_waiting_slot()));
}


ConnectWidget::~ConnectWidget() {
}


void ConnectWidget::preinit() {
  // this is all just to keep the GUI nice and snappy ....
  // you have to see to believe ...
  messg->setText(i18n("Looking for Modem ..."));
  inittimer->start(100);
}


void ConnectWidget::init() {
  gpppdata.setpppdError(0);
  inittimer->stop();
  vmain = 0;
  expecting = false;
  pausing = false;
  scriptindex = 0;
  myreadbuffer = "";
  scanning = false;
  scanvar = "";
  firstrunID = true;
  firstrunPW = true;
  totalbytes = 0;
  dialnumber = 0;

  p_kppp->con_speed = "";

  quit_on_disconnect = quit_on_disconnect || gpppdata.quit_on_disconnect(); 

  comlist = &gpppdata.scriptType();
  arglist = &gpppdata.script();

  QString tit = i18n("Connecting to: ");
  tit += gpppdata.accname();
  setCaption(tit);

  kapp->processEvents();

  int lock = Modem::modem->lockdevice();

  if (lock == 1) {
    messg->setText(i18n("Sorry, modem device is locked."));
    vmain = 20; // wait until cancel is pressed
    return;
  }

  if (lock == -1) {    
    messg->setText(i18n("Sorry, can't create modem lock file."));
    vmain = 20; // wait until cancel is pressed
    return;
  }

#ifdef linux
  // re-check PPP support. This will load the kernel module if it was meanwhile unloaded. This will
  // prevent us from the strange error "This kernel lacks ppp support" a lot of people had, though
  // PPP support was available as kernel module. I think this is due to a timing problem
  (void)ppp_available();
#endif

  if(Modem::modem->opentty()) {
    messg->setText(Modem::modem->modemMessage());
    kapp->processEvents();
    if(Modem::modem->hangup()) {

      kapp->processEvents();

      semaphore = false;

      Modem::modem->stop();
      Modem::modem->notify(this, SLOT(readChar(unsigned char)));
      
      // if we are stuck anywhere we will time out
      timeout_timer->start(atoi(gpppdata.modemTimeout())*1000); 
      
      // this timer will run the script etc.
      main_timer_ID = startTimer(10);

      return;
    }
  }

  // initialization failed
  messg->setText(Modem::modem->modemMessage());
  vmain = 20; // wait until cancel is pressed
  Modem::modem->unlockdevice();
}                  


void ConnectWidget::timerEvent(QTimerEvent *) {
  if (semaphore || pausing)
    return;

  if(vmain == 0) {
    messg->setText(i18n("Initializing Modem..."));
    emit debugMessage(i18n("Initializing Modem..."));

    // send a carriage return and then wait a bit so that the modem will
    // let us issue commands.
    if(gpppdata.modemPreInitDelay() > 0) {
      usleep(gpppdata.modemPreInitDelay() * 5000);
      writeline("");
      usleep(gpppdata.modemPreInitDelay() * 5000);
    }
    setExpect(gpppdata.modemInitResp());
    writeline(gpppdata.modemInitStr());
    usleep(gpppdata.modemInitDelay() * 10000); // 0.01 - 3.0 sec       
    vmain = 3;
    return;
  }

  if(vmain == 3)
    if(!expecting) {
      messg->setText(i18n("Setting speaker volume..."));
      emit debugMessage(i18n("Setting speaker volume..."));

      setExpect(gpppdata.modemInitResp());
      QString vol("AT");
      vol += gpppdata.volumeInitString();
      writeline(vol.data());
      usleep(gpppdata.modemInitDelay() * 10000); // 0.01 - 3.0 sec
      vmain = 1;
      return;
    }

  // dial the number and wait to connect
  if(vmain == 1) {
    if(!expecting) {

      timeout_timer->stop();
      timeout_timer->start(atoi(gpppdata.modemTimeout())*1000);

      QString bm = i18n("Dialing");
      QStrList &plist = gpppdata.phonenumbers();
      bm += " ";
      bm += plist.at(dialnumber);
      messg->setText(bm);
      emit debugMessage(bm);

      QString pn = gpppdata.modemDialStr();
      pn += plist.at(dialnumber);
      if(++dialnumber >= plist.count())
        dialnumber = 0;
      writeline(pn);
      
      setExpect(gpppdata.modemConnectResp());
      vmain = 100;  
      return;
    }
  }

  // wait for connect, but redial if BUSY or wait for user cancel
  // if NO CARRIER or NO DIALTONE
  if(vmain == 100) {
    if(!expecting) {
      myreadbuffer = gpppdata.modemConnectResp();
      setExpect("\n");
      vmain = 101;
      return;
    }

    if(readbuffer.contains(gpppdata.modemBusyResp())) {      
      timeout_timer->stop();
      timeout_timer->start(atoi(gpppdata.modemTimeout())*1000);

      messg->setText(i18n("Line Busy. Hanging up ..."));
      emit debugPutChar('\n');
      Modem::modem->hangup();

      if(gpppdata.busyWait() > 0) {
	QString bm = i18n("Line Busy. Waiting: ");
	bm += gpppdata.busyWait();
	bm += i18n(" seconds");
	messg->setText(bm);
	emit debugMessage(bm);
      
	pausing = true;
      
	pausetimer->start(atoi(gpppdata.busyWait())*1000, true);
	timeout_timer->stop();
      }

      Modem::modem->setDataMode(false); 
      vmain = 0;
      return;
    }

    if(readbuffer.contains(gpppdata.modemNoDialtoneResp())) {
      timeout_timer->stop();

      messg->setText(i18n("No Dialtone"));
      vmain = 20;
      Modem::modem->unlockdevice();
      return;
    }

    if(readbuffer.contains(gpppdata.modemNoCarrierResp())) {
      timeout_timer->stop();

      messg->setText(i18n("No Carrier"));
      vmain = 20;
      Modem::modem->unlockdevice();
      return;
    }
  }

  // wait for newline after CONNECT response (so we get the speed)
  if(vmain == 101) {
    if(!expecting) {
      Modem::modem->setDataMode(true); // modem will no longer respond to AT commands

      emit startAccounting();
      p_kppp->con_win->startClock();

      vmain = 2;
      scriptTimeout=atoi(gpppdata.modemTimeout())*1000;
      return;
    }
  }

  // execute the script
  if(vmain == 2) {
    if(!expecting && !pausing && !scanning) {

      timeout_timer->stop();
      timeout_timer->start(scriptTimeout);

      if((unsigned) scriptindex < comlist->count()) {
        scriptCommand = comlist->at(scriptindex);
        scriptArgument = arglist->at(scriptindex);
      } else {
        Debug("End of script\n");
	vmain = 10;
        return;
      }

      if(strcmp(scriptCommand, "Scan") == 0) {
	QString bm = i18n("Scanning ");
	bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

        setScan(scriptArgument);
	scriptindex++;
        return;
      }

      if(strcmp(scriptCommand, "Save") == 0) {
	QString bm = i18n("Saving ");
	bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

	if(stricmp(scriptArgument, "password") == 0) {
	  gpppdata.setPassword(scanvar.data());
	  p_kppp->setPW_Edit(scanvar.data());
	  if(gpppdata.storePassword())
	    gpppdata.setStoredPassword(scanvar.data());
	  firstrunPW = true;
	}

	scriptindex++;
        return;
      }


      if(strcmp(scriptCommand, "Send") == 0) {
	QString bm = i18n("Sending ");
	bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

	writeline(scriptArgument);
	scriptindex++;
        return;
      }

      if(strcmp(scriptCommand, "Expect") == 0) {
        QString bm = i18n("Expecting ");
        bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

        setExpect(scriptArgument);
	scriptindex++;
        return;
      }


      if(strcmp(scriptCommand, "Pause") == 0) {
	QString bm = i18n("Pause ");
	bm += scriptArgument;
	bm += i18n(" seconds");
	messg->setText(bm);
	emit debugMessage(bm);
	
	pausing = true;
	
	pausetimer->start(atoi(scriptArgument)*1000, true);
	timeout_timer->stop();
	
	scriptindex++;
	return;
      }

      if(strcmp(scriptCommand, "Timeout") == 0) {

	timeout_timer->stop();

	QString bm = i18n("Timeout ");
	bm += scriptArgument;
	bm += i18n(" seconds");
	messg->setText(bm);
	emit debugMessage(bm);
	
	scriptTimeout=atoi(scriptArgument)*1000;
        timeout_timer->start(scriptTimeout);
	
	scriptindex++;
	return;
      }

      if(strcmp(scriptCommand, "Hangup") == 0) {
	messg->setText(i18n("Hangup"));
	emit debugMessage(i18n("Hangup"));

	writeline(gpppdata.modemHangupStr());
	setExpect(gpppdata.modemHangupResp());
	
	scriptindex++;
	return;
      }

      if(strcmp(scriptCommand, "Answer") == 0) {
	
	timeout_timer->stop();

	messg->setText(i18n("Answer"));
	emit debugMessage(i18n("Answer"));

	setExpect(gpppdata.modemRingResp());
	vmain = 150;
	return;
      }

      if(strcmp(scriptCommand, "ID") == 0) {
	QString bm = i18n("ID ");
	bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

	QString idstring = gpppdata.storedUsername();
	
	if(!idstring.isEmpty() && firstrunID) {
	  // the user entered an Id on the main kppp dialog
	  writeline(idstring.data());
	  firstrunID = false;
	  scriptindex++;
	}
	else {
	  // the user didn't enter and Id on the main kppp dialog
	  // let's query for an ID
	     /* if not around yet, then post window... */
	     if (prompt->Consumed()) {
	       if (!(prompt->isVisible())) {
		 prompt->setPrompt(scriptArgument);
		 prompt->setEchoModeNormal();
		 prompt->show();
	       }
	     } else {
	       /* if prompt withdrawn ... then, */
	       if(!(prompt->isVisible())) {
		 writeline(prompt->text());
		 prompt->setConsumed();
		 scriptindex++;
		 return;
	       }
	       /* replace timeout value */
	     }
	}
      }

      if(strcmp(scriptCommand, "Password") == 0) {
	QString bm = i18n("Password ");
	bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

	QString pwstring = gpppdata.Password();
	
	if(!pwstring.isEmpty() && firstrunPW) {
	  // the user entered a password on the main kppp dialog
	  writeline(pwstring.data());
	  firstrunPW = false;
	  scriptindex++;
	}
	else {
	  // the user didn't enter a password on the main kppp dialog
	  // let's query for a password
	     /* if not around yet, then post window... */
	     if (prompt->Consumed()) {
	       if (!(prompt->isVisible())) {
		 prompt->setPrompt(scriptArgument);
		 prompt->setEchoModePassword();
		 prompt->show();
	       }
	     } else {
	       /* if prompt withdrawn ... then, */
	       if(!(prompt->isVisible())) {
		 p_kppp->setPW_Edit(prompt->text());
		 writeline(prompt->text());
		 prompt->setConsumed();
		 scriptindex++;
		 return;
	       }
	       /* replace timeout value */
	     }
	}
      }
 
      if(strcmp(scriptCommand, "Prompt") == 0) {
	QString bm = i18n("Prompting ");

        // if the scriptindex (aka the prompt text) includes a ## marker 
        // this marker should get substituted with the contents of our stored 
        // variable (from the subsequent scan).
	
	QString ts = scriptArgument;
	int vstart = ts.find( "##" );
	if( vstart != -1 ) {
		ts.remove( vstart, 2 );
		ts.insert( vstart, scanvar );
	}

	bm += ts;
	messg->setText(bm);
	emit debugMessage(bm);

	/* if not around yet, then post window... */
	if (prompt->Consumed()) {
	   if (!(prompt->isVisible())) {
		prompt->setPrompt( ts );
		prompt->setEchoModeNormal();
	        prompt->show();
	   }
	} else {
	    /* if prompt withdrawn ... then, */
	    if (!(prompt->isVisible())) {
	    	writeline(prompt->text());
	        prompt->setConsumed();
	        scriptindex++;
        	return;
	    }
	    /* replace timeout value */
	}
      }

      if(strcmp(scriptCommand, "PWPrompt") == 0) {
	QString bm = i18n("PW Prompt ");
	bm += scriptArgument;
	messg->setText(bm);
	emit debugMessage(bm);

	/* if not around yet, then post window... */
	if (prompt->Consumed()) {
	   if (!(prompt->isVisible())) {
		prompt->setPrompt(scriptArgument);
		prompt->setEchoModePassword();
	        prompt->show();
	   }
	} else {
	    /* if prompt withdrawn ... then, */
	    if (!(prompt->isVisible())) {
	    	writeline(prompt->text());
	        prompt->setConsumed();
	        scriptindex++;
        	return;
	    }
	    /* replace timeout value */
	}
      }

      if(strcmp(scriptCommand, "LoopStart") == 0) {

        QString bm = i18n("Loop Start ");
        bm += scriptArgument;

	if ( loopnest > (MAXLOOPNEST-2) ) {
		bm += i18n("ERROR: Nested too deep, ignored.");
		vmain=20;
		scriptindex++;
		cancelbutton();
	        QMessageBox::warning( 0, i18n("Error"),
				      i18n("Loops nested too deeply!"));
	} else {
        	setExpect(scriptArgument);
		loopstartindex[loopnest] = scriptindex + 1;
		loopstr[loopnest] = scriptArgument;
		loopend = false;
		loopnest++;
	}
	messg->setText(bm);
	emit debugMessage(bm);

	scriptindex++;
      }

      if(strcmp(scriptCommand, "LoopEnd") == 0) {
        QString bm = "Loop End ";
        bm += scriptArgument;
	if ( loopnest <= 0 ) {
		bm = i18n("LoopEnd without matching Start! Line: ") + bm ;
		vmain=20;
		scriptindex++;
		cancelbutton();
	        QMessageBox::warning( 0, i18n("Error"), bm );
		return;
	} else {
        	setExpect(scriptArgument);
		loopnest--;
		loopend = true;
	}
	messg->setText(bm);
	emit debugMessage(bm);

	scriptindex++;
      }
    }
  }

  // this is a subroutine for the "Answer" script option

  if(vmain == 150) {
    if(!expecting) {
      writeline(gpppdata.modemAnswerStr());
      setExpect(gpppdata.modemAnswerResp());

      vmain = 2;
      scriptindex++;
      return;
    }
  }

  if(vmain == 30) {
    if (termwindow->isVisible()) 
      return;
    if (termwindow->pressedContinue())
      vmain = 10;
    else
      cancelbutton();
  }

  if(vmain == 10) { 
    if(!expecting) {

      int result;

      timeout_timer->stop();
      if_timeout_timer->stop(); // better be sure.

      // stop reading of data
      Modem::modem->stop();

      if(gpppdata.authMethod() == AUTH_TERMINAL) {
	if (termwindow) {
	  delete termwindow;
	  termwindow = 0L;
	  this->show();
	} else {
	  termwindow = new LoginTerm(0L, 0L);
	  hide();
	  termwindow->show();
	  vmain = 30;
	  return;
	}
      }

      // Close the tty. This prevents the QTimer::singleShot() in
      // Modem::readtty() from re-enabling the socket notifier.
      // The port is still held open by the helper process.
      Modem::modem->closetty();

      killTimer( main_timer_ID );

      if_timeout_timer->start(atoi(gpppdata.pppdTimeout())*1000);
      Debug("started if timeout timer with %d\n",atoi(gpppdata.pppdTimeout())*1000);

      kapp->flushX();
      semaphore = true;
      result = execppp();

      emit debugMessage(i18n("Starting pppd ..."));
      Debug("execppp() returned with return-code %d\n", result);

      if(result) {
	adddns(); 

	// O.K we are done here, let's change over to the if_waiting loop
	// where we wait for the ppp if (interface) to come up.

	emit if_waiting_signal();
      }
      else {

	// starting pppd wasn't successful. Error messages were 
	// handled by execppp();
	if_timeout_timer->stop();
	this->hide();
	messg->setText("");
	p_kppp->quit_b->setFocus();
	p_kppp->show();
	kapp->processEvents();
	Modem::modem->hangup();
	emit stopAccounting();
	p_kppp->con_win->stopClock();
	Modem::modem->closetty();
        Modem::modem->unlockdevice();

      }

      return;
    }
  }

  // this is a "wait until cancel" entry

  if(vmain == 20) {
  }
}


void ConnectWidget::set_con_speed_string() {
  // Here we are trying to determine the speed at which we are connected.
  // Usually the modem responds after connect with something like
  // CONNECT 115200, so all we need to do is find the number after CONNECT
  // or whatever the modemConnectResp() is.
  p_kppp->con_speed = Modem::modem->parseModemSpeed(myreadbuffer);
}



void ConnectWidget::readChar(unsigned char c) {
  if(semaphore)
    return;

    readbuffer += c;

    // scan speed after CONNECT
    if(vmain == 101)
      myreadbuffer += c;

    // While in scanning mode store each char to the scan buffer
    // for use in the prompt command
    if( scanning )
       scanbuffer += c;
 
    // add to debug window
    emit debugPutChar(c);

    checkBuffers();
}


void ConnectWidget::checkBuffers() {
  // Let's check if we are finished with scanning:
  // The scanstring have to be in the buffer and the latest character
  // was a carriage return or an linefeed (depending on modem setup)
  if( scanning && scanbuffer.contains(scanstr) &&
      ( scanbuffer.right(1) == "\n" || scanbuffer.right(1) == "\r") ) {
      scanning = false;

      int vstart = scanbuffer.find( scanstr ) + scanstr.length();
      scanvar = scanbuffer.mid( vstart, scanbuffer.length() - vstart);
      scanvar = scanvar.stripWhiteSpace();

      // Show the Variabel content in the debug window
      QString sv = i18n("Scan Var: ");
      sv += scanvar;
      emit debugMessage(sv);
  }

  if(expecting) {
    if(readbuffer.contains(expectstr)) {
      expecting = false;
      // keep everything after the expected string
      readbuffer.remove(0, readbuffer.find(expectstr) + expectstr.length());

      QString ts = i18n("Found: ");
      ts += expectstr;
      emit debugMessage(ts);

      if (loopend) {
	loopend=false;
      }
    }

    if (loopend && readbuffer.contains(loopstr[loopnest])) {
      expecting = false;
      readbuffer = "";
      QString ts = i18n("Looping: ");
      ts += loopstr[loopnest];
      emit debugMessage(ts);
      scriptindex = loopstartindex[loopnest];
      loopend = false;
      loopnest++;
    }
    // notify event loop if expected string was found
    if(!expecting)
      timerEvent((QTimerEvent *) 0);
  }
}



void ConnectWidget::pause() {
  pausing = false;
  pausetimer->stop();
}


void ConnectWidget::cancelbutton() {
  Modem::modem->stop();
  killTimer(main_timer_ID);
  timeout_timer->stop();
  if_timeout_timer->stop();

  if (termwindow) {
    delete termwindow;
    termwindow = 0L;
    this->show();
  }

  messg->setText(i18n("One Moment Please ..."));

  // just to be sure
  Requester::rq->removeSecret(AUTH_PAP);
  Requester::rq->removeSecret(AUTH_CHAP);
  removedns();
  
  kapp->processEvents();

  Requester::rq->killPPPDaemon();  
  Modem::modem->hangup();

  this->hide();
  messg->setText("");
  p_kppp->quit_b->setFocus();
  p_kppp->show();
  emit stopAccounting();	// just to be sure
  p_kppp->con_win->stopClock();
  Modem::modem->closetty();
  Modem::modem->unlockdevice();

  //abort prompt window...
  if (prompt->isVisible()) {
  	prompt->hide();
  }
  prompt->setConsumed();

  if(quit_on_disconnect) 
    kapp->exit(0);
}


void ConnectWidget::script_timed_out() {
  if(vmain == 20) { // we are in the 'wait for the user to cancel' state
    timeout_timer->stop();
    emit stopAccounting();
    p_kppp->con_win->stopClock();
    return;
  }

  if (prompt->isVisible())
    prompt->hide();
  
  prompt->setConsumed();
  messg->setText(i18n("Script timed out!"));
  Modem::modem->hangup();
  emit stopAccounting();
  p_kppp->con_win->stopClock();

  vmain = 0; // let's try again.
}


void ConnectWidget::debugbutton() {
  emit toggleDebugWindow();
}


void ConnectWidget::setScan(const char *n) {
  scanning = true;
  scanstr = n;
  scanbuffer = "";

  QString ts = i18n("Scanning: ");
  ts += n;
  emit debugMessage(ts);
}


void ConnectWidget::setExpect(const char *n) {
  expecting = true;
  expectstr = n;

  QString ts = i18n("Expecting: ");
  ts += n;
  ts.replace(QRegExp("\n"), "<LF>");
  emit debugMessage(ts);

  // check if the expected string is in the read buffer already.
  checkBuffers();
}


void ConnectWidget::if_waiting_timed_out() {
  if_timer->stop();
  if_timeout_timer->stop();
  Debug("if_waiting_timed_out()\n");
  
  gpppdata.setpppdError(E_IF_TIMEOUT);

  // let's kill the stuck pppd
  Requester::rq->killPPPDaemon();

  emit stopAccounting();
  p_kppp->con_win->stopClock();


  // killing ppp will generate a SIGCHLD which will be caught in pppdie()
  // in main.cpp what happens next will depend on the boolean 
  // reconnect_on_disconnect which is set in ConnectWidget::init();
}


void ConnectWidget::if_waiting_slot() {
  messg->setText(i18n("Logging on to Network ..."));

  if(!if_is_up()) {

    if(gpppdata.pppdError() != 0) {
      // we are here if pppd died immediately after starting it.

      if_timer->stop();
      // error message handled in main.cpp: die_ppp()
      return;
    }

    if_timer->start(100, TRUE); // single shot 
    return;
  }

  // O.K the ppp interface is up and running
  // give it a few time to come up completly (0.2 seconds)
  if_timeout_timer->stop(); 
  if_timer->stop();
  usleep(200000);

  p_kppp->stats->take_stats(); // start taking ppp statistics
  auto_hostname();

  if(strcmp(gpppdata.command_on_connect(), "") != 0) {
    messg->setText(i18n("Running Startup Command ..."));

    // make sure that we don't get any async errors
    kapp->flushX(); 
    execute_command(gpppdata.command_on_connect());
    messg->setText(i18n("Done"));
  }

  // remove the authentication file
  Requester::rq->removeSecret(AUTH_PAP);
  Requester::rq->removeSecret(AUTH_CHAP);

  emit debugMessage(i18n("Done"));
  set_con_speed_string();

  p_kppp->con_win->setConnectionSpeed();
  this->hide();
  messg->setText("");

  // prepare the con_win so as to have the right size for
  // accounting / non-accounting mode
  if(p_kppp->acct != 0)
    p_kppp->con_win->accounting(p_kppp->acct->running());
  else
    p_kppp->con_win->accounting(false);

  if (gpppdata.get_dock_into_panel()) {
    DockWidget::dock_widget->dock();
    DockWidget::dock_widget->take_stats();
    this->hide();
  } 
  else {
    p_kppp->con_win->show();
    
    if(gpppdata.get_iconify_on_connect()) {
      p_kppp->con_win->iconify();
    }
  }
}


bool ConnectWidget::execppp() {
  QString command;

  command = "pppd";

  // as of version 2.3.6 pppd falls back to the real user rights when
  // opening a device given in a command line. To avoid permission conflicts
  // we'll simply leave this argument away. pppd will then use the default tty
  // which is the serial port we connected stdin/stdout to in opener.cpp.
  //  command += " ";
  //  command += gpppdata.modemDevice();

  command += " " ;
  command += gpppdata.speed();

  command += " -detach";

  if(strcmp(gpppdata.ipaddr(), "0.0.0.0") != 0 ||
     strcmp(gpppdata.gateway(), "0.0.0.0") != 0) {
    if(strcmp(gpppdata.ipaddr(), "0.0.0.0") != 0) {
      command += " "; 
      command += gpppdata.ipaddr();
      command +=  ":";
    }
    else {
      command += " ";
      command += ":";
    }

    if(strcmp(gpppdata.gateway(), "0.0.0.0") != 0)
      command += gpppdata.gateway();
  }

  if(strcmp(gpppdata.subnetmask(), "0.0.0.0") != 0) {
    command += " ";
    command += "netmask";
    command += " ";
    command += gpppdata.subnetmask();

  }

  if(strcmp(gpppdata.flowcontrol(), "None") != 0) {
    if(strcmp(gpppdata.flowcontrol(), "CRTSCTS") == 0) {
      command += " ";
      command +=  "crtscts";
    }
    else {
      command += " ";
      command += "xonxoff";
    }
  }

  if(gpppdata.defaultroute()) {
    command += " ";
    command +=  "defaultroute";
  }

  QStrList &arglist = gpppdata.pppdArgument();
  for (char *arg = arglist.first(); arg; arg = arglist.next()) {
    command += " ";
    command += arg;
  }

  // PAP settings
  if(gpppdata.authMethod() == AUTH_PAP) {
    command += " -chap user ";
    command = command + "\"" + gpppdata.storedUsername() + "\"";
  }

  // CHAP settings
  if(gpppdata.authMethod() == AUTH_CHAP) {
    command += " -pap user ";
    command = command + "\"" + gpppdata.storedUsername() + "\"";
  }

  // check for debug
  if(gpppdata.getPPPDebug())
    command += " debug";

  if (command.length() > MAX_CMDLEN) {
    QMessageBox::warning(this, 
			 i18n("Error"), 
			 i18n(
			      "pppd command + command-line arguments exeed\n"
			      "2024 characters in length. What are you doing?"
			      )
			 );	
    return false; // nonsensically long command which would bust my buffer buf.
  }

  kapp->flushX();

  return Requester::rq->execPPPDaemon(command.data());
}


void ConnectWidget::closeEvent( QCloseEvent *e ) {
  e->ignore();
  emit cancelbutton();
}


void ConnectWidget::setMsg(const char* msg) {
  messg->setText(msg);
}

void ConnectWidget::writeline(const char *s) {
  Modem::modem->writeLine(s);
}

// Set the hostname and domain from DNS Server
void auto_hostname() {
  struct in_addr local_ip;
  struct hostent *hostname_entry;
  QString new_hostname;
  int    dot;
  char   tmp_str[100]; // buffer overflow safe

  gethostname(tmp_str, sizeof(tmp_str));
  tmp_str[sizeof(tmp_str)-1]=0; // panic
  old_hostname=tmp_str; // copy to QString

  if (!local_ip_address.isEmpty() && gpppdata.autoname()) {
    local_ip.s_addr=inet_addr((const char*)local_ip_address);
    hostname_entry=gethostbyaddr((const char *)&local_ip,sizeof(in_addr),AF_INET);

    if (hostname_entry != 0L) {
      new_hostname=hostname_entry->h_name;
      dot=new_hostname.find('.');
      new_hostname=new_hostname.remove(dot,new_hostname.length()-dot);
      Requester::rq->setHostname(new_hostname.data());
      modified_hostname = TRUE;

      new_hostname=hostname_entry->h_name;
      new_hostname.remove(0,dot+1);

      add_domain(new_hostname);
    }
  }

}  

// Replace the DNS domain entry in the /etc/resolv.conf file and
// disable the nameserver entries if option is enabled
void add_domain(const char *domain) {

  int fd;
  char c;
  QString resolv[MAX_RESOLVCONF_LINES];

  if (domain == 0L || ! strcmp(domain, "")) 
    return;

  if((fd = Requester::rq->openResolv(O_RDONLY)) >= 0) {

    int i=0;
    while((read(fd, &c, 1) == 1) && (i < MAX_RESOLVCONF_LINES)) {
      if(c == '\n') {
	i++;
      }
      else {
	resolv[i] += c;
      }
    }
    close(fd);
    if ((c != '\n') && (i < MAX_RESOLVCONF_LINES)) i++;

    if((fd = Requester::rq->openResolv(O_WRONLY|O_TRUNC)) >= 0) {

      write(fd, "domain ", 7);
      write(fd, domain, strlen(domain));
      write(fd, " \t\t#kppp temp entry\n", 20);

      for(int j=0; j < i; j++) {
	if((resolv[j].contains("domain") ||
	      ( resolv[j].contains("nameserver") 
		&& !resolv[j].contains("#kppp temp entry") 
		&& gpppdata.exDNSDisabled())) 
	        && !resolv[j].contains("#entry disabled by kppp")) {

          write(fd, "# ", 2);
	  write(fd, resolv[j].data(), resolv[j].length());
          write(fd, " \t#entry disabled by kppp\n", 26);
	}
	else {
	  write(fd, resolv[j].data(), resolv[j].length());
	  write(fd, "\n", 1);
	}
      }
    }
    close(fd);
  }
}  


// adds the DNS entries in the /etc/resolv.conf file
void adddns() {
  int fd;

  if((fd = Requester::rq->openResolv(O_WRONLY|O_APPEND)) >= 0) {

    QStrList &dnslist = gpppdata.dns();
    for(char *dns = dnslist.first(); dns; dns = dnslist.next()) {
      write(fd, "nameserver ", 11);
      write(fd, dns, strlen(dns));
      write(fd, " \t#kppp temp entry\n", 19);
    }
    close(fd);
  }
  add_domain(gpppdata.domain());
}  


// remove the dns entries from the /etc/resolv.conf file
void removedns() {

  int fd;
  char c;
  QString resolv[MAX_RESOLVCONF_LINES];

  if((fd = Requester::rq->openResolv(O_RDONLY)) >= 0) {

    int i=0;
    while(read(fd, &c, 1) == 1 && i < MAX_RESOLVCONF_LINES) {
      if(c == '\n') {
	i++;
      }
      else {
	resolv[i] += c;
      }
    }
    close(fd);

    if((fd = Requester::rq->openResolv(O_WRONLY|O_TRUNC)) >= 0) {
      for(int j=0; j < i; j++) {
	if(resolv[j].contains("#kppp temp entry")) continue;
	if(resolv[j].contains("#entry disabled by kppp")) {
	  write(fd, resolv[j].data()+2, resolv[j].length() - 27);
	  write(fd, "\n", 1);
	}
	else {
	  write(fd, resolv[j].data(), resolv[j].length());
	  write(fd, "\n", 1);
	}
      }
    }
    close(fd);

  }

  if (  modified_hostname ) {
    Requester::rq->setHostname(old_hostname.data());
    modified_hostname = FALSE;
  }

}  

#include "connect.moc"
