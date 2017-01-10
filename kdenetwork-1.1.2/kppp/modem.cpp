/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: modem.cpp,v 1.18.2.10 1999/08/17 16:26:53 porten Exp $
 *
 *              Copyright (C) 1997 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 *
 * This file was added by Harri Porten <porten@tu-harburg.de> 
 *
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

#include <errno.h>
#include <sys/ioctl.h>
#include <setjmp.h>
#include <qregexp.h>
#include <qtimer.h>
#include <assert.h>

#include "modem.h"
#include "pppdata.h"
#include "log.h"
#include "requester.h"

static sigjmp_buf jmp_buffer;

Modem *Modem::modem = 0;

Modem::Modem() : 
  modemfd(-1), 
  data_mode(false),
  modem_is_locked(false)
{
  sn = 0L;
  assert(modem==0);
  modem = this;
}


Modem::~Modem() {
  modem = 0;
}


speed_t Modem::modemspeed() {
  // convert the string modem speed int the gpppdata object to a t_speed type
  // to set the modem.  The constants here should all be ifdef'd because
  // other systems may not have them
  int i = atoi(gpppdata.speed())/100;

  switch(i) {
  case 24:
    return B2400;
    break;
  case 96:
    return B9600;
    break;
  case 192:
    return B19200;
    break;
  case 384:
    return B38400;
    break;
#ifdef B57600
  case 576:
    return B57600;
    break;
#endif

#ifdef B115200
  case 1152:
    return B115200;
    break;
#endif

#ifdef B230400
  case 2304:
    return B230400;
    break;
#endif

#ifdef B460800 
  case 4608:
    return 4608;
    break;
#endif

  default:            
    return B38400;
    break;
  }
}


bool Modem::opentty() {
  int flags;

  if(modemfd>=0) {
    fprintf(stderr, "kppp warning: closing old modem fd\n");
    ::close(modemfd);
    modemfd = -1;
  }

  if((modemfd = Requester::rq->openModem(gpppdata.modemDevice()))<0) {
    errmsg = i18n("Sorry, can't open modem.");
    return false;
  }
  
#if 0
  if(gpppdata.UseCDLine()) {
    if(ioctl(modemfd, TIOCMGET, &flags) == -1) {
      errmsg = i18n("Sorry, can't detect state of CD line.");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
    if ((flags&TIOCM_CD) == 0) {
      errmsg = i18n("Sorry, the modem is not ready.");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
  }
#endif
	
  tcdrain (modemfd);
  tcflush (modemfd, TCIOFLUSH);

  if(tcgetattr(modemfd, &tty) < 0){
    // try if that helps
    tcsendbreak(modemfd, 0);
    sleep(1);
    if(tcgetattr(modemfd, &tty) < 0){
      errmsg = i18n("Sorry, the modem is busy.");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
    // Ask for success stories since we don't know if the tcsendbreak()
    // makes any difference. No serious need for a translation.
    QMessageBox::warning(0L, "Warning", 
			 "kppp had to resort to an experimental method to get "
			 "the terminal attributes.\n\nPlease send me "
			 "(porten@kde.org) a short note so we can make this "
			 "fix\npermanent in the next release. Thanks.");
  }

  memset(&initial_tty,'\0',sizeof(initial_tty));

  initial_tty = tty;

  tty.c_cc[VMIN] = 0; // nonblocking 
  tty.c_cc[VTIME] = 0;
  tty.c_oflag = 0;
  tty.c_lflag = 0;

  tty.c_cflag &= ~(CSIZE | CSTOPB | PARENB);  
  tty.c_cflag |= CS8 | CREAD;
  tty.c_cflag |= CLOCAL;                   // ignore modem status lines      
  tty.c_iflag = IGNBRK | IGNPAR /* | ISTRIP */ ;
  tty.c_lflag &= ~ICANON;                  // non-canonical mode
  tty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHOKE);


  if(strcmp(gpppdata.flowcontrol(), "None") != 0) {
    if(strcmp(gpppdata.flowcontrol(), "CRTSCTS") == 0) {
      tty.c_cflag |= CRTSCTS;
    }
    else {
      tty.c_iflag |= IXON | IXOFF;
      tty.c_cc[VSTOP]  = 0x13; /* DC3 = XOFF = ^S */
      tty.c_cc[VSTART] = 0x11; /* DC1 = XON  = ^Q */
    }
  }
  else {
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF);
  }

  cfsetospeed(&tty, modemspeed());
  cfsetispeed(&tty, modemspeed());

  tcdrain(modemfd);	

  if(tcsetattr(modemfd, TCSANOW, &tty) < 0){
    errmsg = i18n("Sorry, the modem is busy.");
    ::close(modemfd);
    modemfd=-1;
    return false;
  }

  errmsg = i18n("Modem Ready.");
  return true;
}


bool Modem::closetty() {
  if(modemfd >=0 ) {
    stop();
    /* discard data not read or transmitted */
    tcflush(modemfd, TCIOFLUSH);
    
    if(tcsetattr(modemfd, TCSANOW, &initial_tty) < 0){
      errmsg = i18n("Can't restore tty settings: tcsetattr()\n");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
    ::close(modemfd);
    modemfd = -1;
  }

  return true;
}


void Modem::readtty(int) {
  char buffer[200];
  unsigned char c;
  int len;

  // read data in chunks of up to 200 bytes
  if((len = ::read(modemfd, buffer, 200)) > 0) {
    // split buffer into single characters for further processing
    for(int i = 0; i < len; i++) {
      c = buffer[i] & 0x7F;
      emit charWaiting(c);
    }
  }
}


void Modem::notify(const QObject *receiver, const char *member) {
  connect(this, SIGNAL(charWaiting(unsigned char)), receiver, member);
  startNotifier();
}


void Modem::stop() {
  disconnect(SIGNAL(charWaiting(unsigned char)));
  stopNotifier();
}


void Modem::startNotifier() {
  if(modemfd >= 0) {
    if(sn == 0) {
      sn = new QSocketNotifier(modemfd, QSocketNotifier::Read, this);
      connect(sn, SIGNAL(activated(int)), SLOT(readtty(int)));
      Debug("QSocketNotifier started!");
    } else {
      //    Debug("QSocketNotifier re-enabled!");
      sn->setEnabled(true);
    }
  }
}


void Modem::stopNotifier() {
  if(sn != 0) {
    sn->setEnabled(false);
    disconnect(sn);
    delete sn;
    sn = 0;
    Debug("QSocketNotifier stopped!");
  }
}


void Modem::resumeNotifier() {
  if(modemfd >= 0) {
    if(sn != 0) {
      sn->setEnabled(true);
      Debug("QSocketNotifier resumed operation.");
    }
  }
}


void Modem::flush() {
  char c;
  while(read(modemfd, &c, 1) == 1);
}


bool Modem::writeChar(unsigned char c) {
  return write(modemfd, &c, 1) == 1;
}


bool Modem::writeLine(const char *buf) {
  // TODO check return code and think out how to proceed
  // in case of trouble.
  write(modemfd, buf, strlen(buf));

  // Let's send an "enter"
  // which enter we send depends on what the user has selected
  // I haven't seen this on other dialers but this seems to be
  // necessary. I have tested this with two different modems and 
  // one needed an CR the other a CR/LF. Am i hallucinating ?
  // If you know what the scoop is on this please let me know. 
  if(strcmp(gpppdata.enter(), "CR/LF") == 0)
    write(modemfd, "\r\n", 2);
  else if(strcmp(gpppdata.enter(), "LF") == 0)
    write(modemfd, "\n", 1);
  else if(strcmp(gpppdata.enter(), "CR") == 0)
    write(modemfd, "\r", 1);
 
  return true;
}


bool Modem::hangup() {
  // this should really get the modem to hang up and go into command mode
  // If anyone sees a fault in the following please let me know, since
  // this is probably the most imporant snippet of code in the whole of
  // kppp. If people complain about kppp being stuck, this piece of code
  // is most likely the reason.
  struct termios temptty;

  if(modemfd >= 0) {

    // is this Escape & HangupStr stuff really necessary ? (Harri)

    if (data_mode) escape_to_command_mode(); 

    // Then hangup command
    writeLine(gpppdata.modemHangupStr());
    
    usleep(gpppdata.modemInitDelay() * 10000); // 0.01 - 3.0 sec 

    if (sigsetjmp(jmp_buffer, 1) == 0) {
      // set alarm in case tcsendbreak() hangs 
      signal(SIGALRM, alarm_handler);
      alarm(2);
      
      tcsendbreak(modemfd, 0);
      
      alarm(0);
      signal(SIGALRM, SIG_IGN);
    } else {
      // we reach this point if the alarm handler got called
      closetty();
      close(modemfd);
      modemfd = -1;
      errmsg = i18n("Sorry, the modem doesn't respond.");
      return false;
    }

    tcgetattr(modemfd, &temptty);
    cfsetospeed(&temptty, B0);
    cfsetispeed(&temptty, B0);
    tcsetattr(modemfd, TCSAFLUSH, &temptty);

    usleep(gpppdata.modemInitDelay() * 10000); // 0.01 - 3.0 secs 

    cfsetospeed(&temptty, modemspeed());
    cfsetispeed(&temptty, modemspeed());
    tcsetattr(modemfd, TCSAFLUSH, &temptty);
   
    return true;
  } else
    return false;
}


void Modem::escape_to_command_mode() {
  // Send Properly bracketed escape code to put the modem back into command state.
  // A modem will accept AT commands only when it is in command state.
  // When a modem sends the host the CONNECT string, that signals
  // that the modem is now in the connect state (no long accepts AT commands.)
  // Need to send properly timed escape sequence to put modem in command state.
  // Escape codes and guard times are controlled by S2 and S12 values.
  // 
  tcflush(modemfd, TCIOFLUSH);

  // +3 because quiet time must be greater than guard time.
  usleep((gpppdata.modemEscapeGuardTime()+3)*20000);
  write(modemfd, gpppdata.modemEscapeStr(), strlen(gpppdata.modemEscapeStr()) );  
  tcflush(modemfd, TCIOFLUSH);
  usleep((gpppdata.modemEscapeGuardTime()+3)*20000);

  data_mode = false;
}


const char *Modem::modemMessage() const {
  return errmsg.data();
}


QString Modem::parseModemSpeed(const QString &s) {
  // this is a small (and bad) parser for modem speeds
  int rx = -1;
  int tx = -1;
  int i;
  QString result;

  Debug("Modem reported result string: %s", s.data());

  const int RXMAX = 7;
  const int TXMAX = 2;
  QRegExp rrx[RXMAX] = {
    QRegExp("[0-9]+[:/ ]RX", false),
    QRegExp("[0-9]+RX", false),
    QRegExp("[/: -][0-9]+[/: ]", false),
    QRegExp("[/: -][0-9]+$", false),
    QRegExp("CARRIER [^0-9]*[0-9]+", false),
    QRegExp("CONNECT [^0-9]*[0-9]+", false),
    QRegExp("[0-9]+") // panic mode
  };

  QRegExp trx[TXMAX] = {
    QRegExp("[0-9]+[:/ ]TX", false),
    QRegExp("[0-9]+TX", false)
  };

  for(i = 0; i < RXMAX; i++) {
    int len, idx, result;
    if((idx = rrx[i].match(s.data(), 0, &len)) > -1) {
      // find first digit
      QString sub = s.mid(idx, len);
      sub = sub.mid(sub.find(QRegExp("[0-9]")), 255);
      sscanf(sub.data(), "%d", &result);
      if(result > 0) {
	rx = result;
	break;
      }
    }	
  }
  
  for(i = 0; i < TXMAX; i++) {
    int len, idx, result;
    if((idx = trx[i].match(s.data(), 0, &len)) > -1) {
      // find first digit
      QString sub = s.mid(idx, len);
      sub = sub.mid(sub.find(QRegExp("[0-9]")), 255);
      sscanf(sub.data(), "%d", &result);
      if(result > 0) {
	tx = result;
	break;
      }
    }	
  }

  if(rx == -1 && tx == -1)
    result = i18n("Unknown speed");
  else if(tx == -1)
    result.sprintf("%d", rx);
  else if(rx == -1) // should not happen
    result.sprintf("%d", tx);
  else
    result.sprintf("%d/%d", rx, tx);

  Debug("The parsed result is: %s\n", result.data());
  
  return result;
}


// Lock modem device. Returns 0 on success 1 if the modem is locked and -1 if
// a lock file can't be created ( permission problem )
int Modem::lockdevice() {
  int fd;
  char newlock[80]=""; // safe

  if(!gpppdata.modemLockFile()) {
    Debug("The user doesn't want a lockfile.");
    return 0;
  }

  if (modem_is_locked) 
    return 1;

  QString lockfile = LOCK_DIR"/LCK..";
  // append everything after /dev/
  lockfile += QString(gpppdata.modemDevice()).mid(5, 0xffff);
  
  if(access(lockfile.data(), F_OK) == 0) {
    if ((fd = Requester::rq->openLockfile(gpppdata.modemDevice(),
                                          O_RDONLY)) >= 0) {
      // Mario: it's not necessary to read more than lets say 32 bytes. If
      // file has more than 32 bytes, skip the rest
      char oldlock[33]; // safe
      int sz = read(fd, &oldlock, 32);
      close (fd);
      if (sz <= 0)
        return 1;
      oldlock[sz] = '\0';
      
      Debug("Device is locked by: %s\n", &oldlock);
      
      int oldpid;
      int match = sscanf(oldlock, "%d", &oldpid);

      // found a pid in lockfile ?
      if (match < 1 || oldpid <= 0)
        return 1;
    
      // check if process exists
      if (kill((pid_t)oldpid, 0) == 0)
        return 1;
      if (errno != ESRCH)
        return 1;
      
      Debug("lockfile is stale\n");
    }
  }

  fd = Requester::rq->openLockfile(gpppdata.modemDevice(),
                                   O_WRONLY|O_TRUNC|O_CREAT);
  if(fd >= 0) {
    sprintf(newlock,"%010d\n", getpid());
    Debug("Locking Device: %s\n",newlock);

    write(fd, newlock, strlen(newlock));
    close(fd);
    modem_is_locked=true;

    return 0;
  }

  return -1;

}
  

// UnLock modem device
void Modem::unlockdevice() {
  if (modem_is_locked) {
    Debug("UnLocking Modem Device\n");
    Requester::rq->removeLockfile();
    modem_is_locked=false;
  }
}  

void alarm_handler(int) {
  Debug("alarm_handler(): Received SIGALRM\n");

  // jump 
  siglongjmp(jmp_buffer, 1);
}

#ifndef HAVE_USLEEP

// usleep for those of you out there who don't have a BSD 4.2 style usleep

extern "C" int select();

void usleep(long usec){
  
  struct {
      long tv_sec;
      long tv_usec;
  } tval;

  tval.tv_sec = usec / 1000000;
  tval.tv_usec = usec % 1000000;
  select(0, 0, 0, 0, (struct timeval *) &tval);
  return;

}

#endif /* HAVE_USLEEP */

#include "modem.moc"
