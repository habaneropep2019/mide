/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: runtests.cpp,v 1.30.2.4 1999/08/26 16:13:25 porten Exp $
 * 
 *            Copyright (C) 1997 Bernd Johannes Wuebben 
 *                   wuebben@math.cornell.edu
 *
 * This file was contributed by Mario Weilguni <mweilguni@sime.com>
 * Thanks Mario !
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
#include "runtests.h"
#include <kapp.h>
#include <unistd.h>
#include <qmsgbox.h>
#include <qregexp.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <netinet/in.h>

#ifdef HAVE_RESOLV_H
#include <resolv.h>
#endif

#ifndef _PATH_RESCONF
#define _PATH_RESCONF "/etc/resolv.conf"
#endif

#ifdef linux
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#if __GLIBC__ >= 2
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#else
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/route.h>
#include <linux/if_ether.h>
#endif

#include <sys/types.h>
#include <net/ppp_defs.h>
#include "if_ppp.h"
#include "requester.h"
#endif // linux

#include "main.h"

// initial effective uid (main.cpp)
extern uid_t euid;

#ifdef linux
// shamelessly stolen from pppd-2.3.5

/********************************************************************
 *
 * Procedure to determine if the PPP line discipline is registered.
 */

int ppp_disc = N_PPP;

/********************************************************************
 *
 * Internal routine to decode the version.modification.patch level
 */

static void decode_version (char *buf, int *version,
			    int *modification, int *patch)
  {
    *version      = (int) strtoul (buf, &buf, 10);
    *modification = 0;
    *patch        = 0;
    
    if (*buf == '.')
      {
	++buf;
	*modification = (int) strtoul (buf, &buf, 10);
	if (*buf == '.')
	  {
	    ++buf;
	    *patch = (int) strtoul (buf, &buf, 10);
	  }
      }
    
    if (*buf != '\0')
      {
	*version      =
	*modification =
	*patch        = 0;
      }
  }


bool ppp_registered(void) {
  int local_fd;
  int init_disc = -1;
  int initfdflags;

  local_fd = Requester::rq->openModem(gpppdata.modemDevice());

  if (local_fd < 0)
    {
      return false;
    }

  initfdflags = fcntl(local_fd, F_GETFL);
  if (initfdflags == -1)
    {
      close (local_fd);
      return false;
    }
 
  initfdflags &= ~O_NONBLOCK;
  fcntl(local_fd, F_SETFL, initfdflags);
  /*
   * Read the initial line dicipline and try to put the device into the
   * PPP dicipline.
   */
  if (ioctl(local_fd, TIOCGETD, &init_disc) < 0)
    {
      close (local_fd);
      return false;
    }
 
  if (ioctl(local_fd, TIOCSETD, &ppp_disc) < 0)
    {
      close (local_fd);
      return false;
    }
 
  if (ioctl(local_fd, TIOCSETD, &init_disc) < 0)
    {
      close (local_fd);
      return false;
    }
 
  close (local_fd);
  return true;
}

/********************************************************************
 *
 * ppp_available - check whether the system has any ppp interfaces
 * (in fact we check whether we can do an ioctl on ppp0).
 *
 *********************************************************************/
bool ppp_available(void)
{
  int s, ok;
  struct ifreq ifr;
  struct utsname kernelInfo;
  int major, minor;

  // our check doesn't work with 2.3 kernels, yet. In order not to cause a
  // false alarm we simply return success for anything higher than 2.2.
  if(uname(&kernelInfo) == -1)
    return true;
  if(sscanf(kernelInfo.release, "%d.%d.", &major, &minor) != 2)
    return true;
  if(major > 2 || (major == 2 && minor > 2))
    return true;

  /*
   * Open a socket for doing the ioctl operations.
   */
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0)
    {
      return false;
    }
 
  strncpy (ifr.ifr_name, "ppp0", sizeof (ifr.ifr_name));
  ok = ioctl(s, SIOCGIFFLAGS, (caddr_t) &ifr) >= 0;
  /*
   * If the device did not exist then attempt to create one by putting the
   * current tty into the PPP discipline. If this works then obtain the
   * flags for the device again.
   */
  if (!ok)
    {
      if (ppp_registered())
        {
	  strncpy (ifr.ifr_name, "ppp0", sizeof (ifr.ifr_name));
	  ok = ioctl(s, SIOCGIFFLAGS, (caddr_t) &ifr) >= 0;
        }
    }
  /*
   * Ensure that the hardware address is for PPP and not something else
   */
  if (ok)
    {
      ok = ioctl (s, SIOCGIFHWADDR, (caddr_t) &ifr) >= 0;
    }
 
  if (ok && ((ifr.ifr_hwaddr.sa_family & ~0xFF) != ARPHRD_PPP))
    {
      ok = 0;
    }
 
  if (!ok)
    {
      return false;
    }
  /*
   *  This is the PPP device. Validate the version of the driver at this
   *  point to ensure that this program will work with the driver.
   */
    else
    {
	char   abBuffer [1024];
	int size;
	int driver_version, driver_modification, driver_patch;
	int my_version, my_modification, my_patch;

	ifr.ifr_data = abBuffer;
	size = ioctl (s, SIOCGPPPVER, (caddr_t) &ifr);
	if (size < 0) {
	    ok = 0;
	} else {
	    decode_version(abBuffer,
			   &driver_version,
			   &driver_modification,
			   &driver_patch);
	    /*
	     * Validate the version of the driver against the version that we used.
	     */
#define PPPD_VERSION "2.3.5"
	    decode_version(PPPD_VERSION,
			   &my_version,
			   &my_modification,
			   &my_patch);

	    /* The version numbers must match */
	    if (driver_version != my_version)
	    {
		ok = 0;
	    }

	    // no need to check this number
// 	    /* The modification levels must be legal */
// 	    if (driver_modification < my_modification)
// 	    {
// 	      /*
// 		if (driver_modification >= 2) {
// 		    /* we can cope with 2.2.0 and above */
// 		  driver_is_old = 1;
// 		} else {
// 		    ok = 0;
// 		}
// */
// 	    }

	    close (s);
	    if (!ok)
	    {
	      /*		sprintf (route_buffer,
			 "Sorry - PPP driver version %d.%d.%d is out of date\n",
			 driver_version, driver_modification, driver_patch);

			 no_ppp_msg = route_buffer;*/
	    }
	}
    }
  return (bool)ok;
}
#endif


int uidFromName(const char *uname) {
  struct passwd *pw;

  setpwent();
  while((pw = getpwent()) != NULL) {
    if(strcmp(uname, pw->pw_name) == 0) {
      int uid = pw->pw_uid;
      endpwent();
      return uid;
    }
  }

  endpwent();
  return -1;
}


const char *homedirFromUid(uid_t uid) {
  struct passwd *pw;
  char *d = 0;

  setpwent();
  while((pw = getpwent()) != NULL) {
    if(pw->pw_uid == uid) {
      d = strdup(pw->pw_dir);
      endpwent();
      return d;
    }
  }

  endpwent();
  return d;
}


const char *getHomeDir() {
  static const char *hd = 0;
  static bool ranTest = false;
  if(!ranTest) {
    hd = homedirFromUid(getuid());
    ranTest = true;
  }
  
  return hd;
}


int runTests() {
  int warning = 0;

  // Test pre-1: check if the user is allowed to dial-out
  if(access("/etc/kppp.allow", R_OK) == 0 && getuid() != 0) {
    bool access = FALSE;
    FILE *f;
    if((f = fopen("/etc/kppp.allow", "r")) != NULL) {
      char buf[2048]; // safe
      while(f != NULL && !feof(f)) {
	if(fgets(buf, sizeof(buf), f) != NULL) {
	  QString s(buf, sizeof(buf));

	  s = s.stripWhiteSpace();
	  if(s[0] == '#' || s.length() == 0)
	    continue;

	  if((uid_t)uidFromName(s.data()) == getuid()) {
	    access = TRUE;
	    fclose(f);
	    f = NULL;
	  }
	}
      }
      if(f)
	fclose(f);
    }

    if(!access) {
      QMessageBox::warning(0,
		 i18n("Error"),
		 i18n("You´re not allowed to dial out with "
				    "kppp.\nContact your system administrator."
				    ));
      shutDown(1);
    }
  }

#ifdef linux
  // Test linux-1: check if the the kernel has PPP support
  if(!ppp_available()) {
    // make sure that the problem does not come from missing permission to avoid false
    // alarms
    int fd = Requester::rq->openModem(gpppdata.modemDevice());
    if(fd>0) {
      close(fd);
      QMessageBox::warning(0,
			   i18n("Error"),
			   i18n("This kernel has no PPP support, neither\n"
				"compiled in nor via the kernel module\n"
				"loader.\n"
				"\n"
				"To solve this problem:\n"
				"  * contact your system adminstrator\n"
				"or\n"
				"  * install a kernel with PPP support\n"));
      warning++;
    }
  }
#endif

  // Test 1: search the pppd binary
  const char *f = gpppdata.pppdPath();

  if(!f) {
    QMessageBox::warning(0,
		 i18n("Error"),
		 i18n("Cannot find the PPP daemon!\n\n"
                      "Make sure that pppd is installed and\n"
                      "you have entered the correct path."));
    warning++;
  }

  // Test 2: check access to the pppd binary
  if(f) {
#if 0
    if(access(f, X_OK) != 0 /* && geteuid() != 0 */) {
      QMessageBox::critical(0,
		   i18n("Error"),
		   i18n("You do not have the permission\n"
			"to start pppd!\n\n"
			"Contact your system administrator\n"
			"and ask to get access to pppd."));
      return TEST_CRITICAL;
    } else {
      struct stat st;
      stat(f, &st);
      if((st.st_mode & S_ISUID) == 0 && getuid() != 0 ) {
	QMessageBox::warning(0,
		     i18n("Error"),
		     i18n("pppd is not properly installed!\n\n"
			  "The pppd binary must be installed\n"
			  "with the SUID bit set. Contact your\n"
			  "system administrator."));
	warning++;
      }
    }
#endif
    if(euid != 0) {
      struct stat st;
      stat(f, &st);
      if(st.st_uid != 0 || (st.st_mode & S_ISUID) == 0) {
	QMessageBox::warning(0,
		     i18n("Error"),
		     i18n("pppd is not properly installed!\n\n"
			  "The pppd binary must be installed\n"
			  "with the SUID bit set. Contact your\n"
			  "system administrator."));
        warning++;
      }
    }
  }

#if 0
  // Test 4: check for undesired 'lock' option in /etc/ppp/options
  QFile opt(SYSOPTIONS);
  if (opt.open(IO_ReadOnly)) {
    QTextStream t(&opt);
    QRegExp r1("^lock");
    QRegExp r2("\\slock$");   // \s matches white space (9,10,11,12,13,32)
    QRegExp r3("\\slock\\s");
    QString s;
    int lines = 0;
    bool match = false;
    
    while (!t.eof() && lines++ < 100) {
      s = t.readLine();
      
      // truncate comments
      if (s.find('#') >= 0)
        s.truncate(s.find('#'));

      if (r1.match(s) >= 0 || r2.match(s) >= 0 || r3.match(s) >= 0)
        match = true;
    }
    opt.close();
    if (match) {
      QMessageBox::warning(0,
                           i18n("Error"),
                           i18n("kppp has detected a 'lock' option in "
                                "/etc/ppp/options.\n\nThis option has "
                                "to be removed since kppp takes care "
                                "of device locking itself.\n"
                                "Contact your system administrator."));
      warning++;
    }
  } 
#endif

  // Test 5: check for existence of /etc/resolv.conf
  int fd;
  if ((fd = open(_PATH_RESCONF, O_RDONLY)) >= 0)
    close(fd);
  else {
    if (geteuid() == 0) {
      if ((fd = open(_PATH_RESCONF, O_WRONLY|O_CREAT)) >= 0) {
	// file will be owned by root and world readable
	fchown(fd, 0, 0);
	fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	close (fd);
      }
    } else {
      QString msgstr;
      msgstr = _PATH_RESCONF" ";
      msgstr += i18n("is missing!\n\n"
		     "Ask your system administrator to create\n"
		     "a non-empty file that has appropriate\n"
		     "read and write permissions.");
      QMessageBox::warning
	(0, i18n("Error"), msgstr);
      warning ++;
    }
  }

  if(warning == 0)
    return TEST_OK;
  else
    return TEST_WARNING;
}

