/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: opener.cpp,v 1.26.2.5 1999/04/10 17:22:56 porten Exp $
 *
 *              Copyright (C) 1997,98 Bernd Johannes Wuebben,
 *		                      Mario Weilguni,
 *                                    Harri Porten
 *
 *
 * This file was contributed by Harri Porten <porten@tu-harburg.de>
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

/* A note to developers:
 *
 * Apart from the first dozen lines in main() the following code represents
 * the setuid root part of kppp. So please be careful !
 * o restrain from using X, Qt or KDE library calls
 * o check for possible buffer overflows
 * o handle requests from the parent process with care. They might be forged.
 * o be paranoid and think twice about everything you change.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <sys/un.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>

#include "kpppconfig.h"
#include "opener.h"
#include "devices.h"
#include "log.h"

#ifdef HAVE_RESOLV_H
#include <resolv.h>
#endif

#ifndef _PATH_RESCONF
#define _PATH_RESCONF "/etc/resolv.conf"
#endif

#define MY_ASSERT(x)  if (!(x)) { \
        fprintf(stderr, "ASSERT: \"%s\" in %s (%d)\n",#x,__FILE__,__LINE__); \
        exit(1); }

static void sighandler(int);
static pid_t pppdPid = -1;

Opener::Opener(int s) : socket(s), ttyfd(-1) {
  lockfile[0] = '\0';
  signal(SIGUSR1, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGCHLD, sighandler);
  mainLoop();
}

void Opener::mainLoop() {

  int len;
  int fd = -1;
  int flags, mode;
  const char *device;
  union AllRequests request;
  struct ResponseHeader response;
  struct msghdr	msg;
  struct iovec iov;

  iov.iov_base = IOV_BASE_CAST &request;
  iov.iov_len = sizeof(request);
  
  msg.msg_name = 0L;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = 0L;
  msg.msg_controllen = 0;

  // loop forever
  while(1) {
    len = recvmsg(socket, &msg, 0);
    if(len < 0) {
      switch(errno) {
      case EINTR:
	Debug("Opener: interrupted system call, continuing");
	break;
      default:
	perror("Opener: error reading from socket");
	_exit(1);
      }
    } else {
      switch(request.header.type) {

      case OpenDevice:
	Debug("Opener: received OpenDevice");
	MY_ASSERT(len == sizeof(struct OpenModemRequest));
	close(ttyfd);
	device = deviceByIndex(request.modem.deviceNum);
	response.status = 0;
	if ((ttyfd = open(device, O_RDWR|O_NDELAY|O_NOCTTY)) == -1) {
	  Debug("error opening modem device !");
	  fd = open(DEVNULL, O_RDONLY);
	  response.status = -errno;
	  sendFD(fd, &response);
	  close(fd);
	} else
	  sendFD(ttyfd, &response);
	break;

      case OpenLock:
	Debug("Opener: received OpenLock\n");
	MY_ASSERT(len == sizeof(struct OpenLockRequest));
	flags = request.lock.flags;
	MY_ASSERT(flags == O_RDONLY || flags == O_WRONLY|O_TRUNC|O_CREAT); 
	if(flags == O_WRONLY|O_TRUNC|O_CREAT)
	  mode = 0644;
	else
	  mode = 0;

	device = deviceByIndex(request.lock.deviceNum);
	MY_ASSERT(strlen(LOCK_DIR)+strlen(device) < MaxPathLen);
	strncpy(lockfile, LOCK_DIR"/LCK..", MaxPathLen);
	strncat(lockfile, device + strlen("/dev/"),
		MaxPathLen - strlen(lockfile));
	lockfile[MaxPathLen] = '\0';
	response.status = 0;
	// TODO:
	//   struct stat st;
	//   if(stat(lockfile.data(), &st) == -1) {
	//     if(errno == EBADF)
	//       return -1;
	//   } else {
	//     // make sure that this is a regular file
	//     if(!S_ISREG(st.st_mode)) 
	//       return -1;
	//   }
	if ((fd = open(lockfile, flags, mode)) == -1) {
	  Debug("error opening lockfile!");
	  lockfile[0] = '\0';
	  fd = open(DEVNULL, O_RDONLY);
	  response.status = -errno;
	} else
	  fchown(fd, 0, 0);
        sendFD(fd, &response);
	close(fd);
	break;

      case RemoveLock:
	Debug("Opener: received RemoveLock");
	MY_ASSERT(len == sizeof(struct RemoveLockRequest));
	close(ttyfd);
	ttyfd = -1;
	response.status = unlink(lockfile);
	lockfile[0] = '\0';
	sendResponse(&response);
	break;

      case OpenResolv:
	Debug("Opener: received OpenResolv");
	MY_ASSERT(len == sizeof(struct OpenResolvRequest));
	flags = request.resolv.flags;
	response.status = 0;
	if ((fd = open(_PATH_RESCONF, flags)) == -1) {
	  Debug("error opening resolv.conf!");
	  fd = open(DEVNULL, O_RDONLY);
	  response.status = -errno;
	}
        sendFD(fd, &response);
	close(fd);
	break;

      case OpenSysLog:
	Debug("Opener: received OpenSysLog");
	MY_ASSERT(len == sizeof(struct OpenLogRequest));
	response.status = 0;
	if ((fd = open("/var/log/messages", O_RDONLY)) == -1) {
	  if ((fd = open("/var/log/syslog.ppp", O_RDONLY)) == -1) {
	    Debug("error opening syslog file !");
	    fd = open(DEVNULL, O_RDONLY);
	    response.status = -errno;
	  }
        }
        sendFD(fd, &response);
	close(fd);
	break;

      case SetSecret:
	Debug("Opener: received SetSecret");
	MY_ASSERT(len == sizeof(struct SetSecretRequest));
	response.status = !createAuthFile(request.secret.authMethod,
					  request.secret.username,
					  request.secret.password);
	sendResponse(&response);
	break;

      case RemoveSecret:
	Debug("Opener: received RemoveSecret");
	MY_ASSERT(len == sizeof(struct RemoveSecretRequest));
	response.status = !removeAuthFile(request.remove.authMethod);
	sendResponse(&response);
	break;

      case SetHostname:
	Debug("Opener: received SetHostname");
	MY_ASSERT(len == sizeof(struct SetHostnameRequest));
	response.status = 0;
	if(sethostname(request.host.name, strlen(request.host.name)))
	  response.status = -errno;
	sendResponse(&response);
	break;

      case ExecPPPDaemon:
	Debug("Opener: received ExecPPPDaemon");
	MY_ASSERT(len == sizeof(struct ExecDaemonRequest));
	response.status = execpppd(request.daemon.arguments);
	sendResponse(&response);
	break;

      case KillPPPDaemon:
	Debug("Opener: received KillPPPDaemon");
	MY_ASSERT(len == sizeof(struct KillDaemonRequest));
	response.status = killpppd();
	sendResponse(&response);
	break;

      case Stop:
	Debug("Opener: received STOP command");
	_exit(0);
	break;

      default:
	Debug("Opener: unknown command type. Exiting ...");
	_exit(1);
      }
    } // else
  }
}


//
// Send an open fd over a UNIX socket pair
//
int Opener::sendFD(int fd, struct ResponseHeader *response) {

  struct { struct cmsghdr cmsg; int fd; } control;
  struct msghdr	msg;
  struct iovec iov;
  size_t cmsglen;

  msg.msg_name = 0L;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  // Send data
  iov.iov_base = IOV_BASE_CAST response;
  iov.iov_len = sizeof(struct ResponseHeader);

#ifdef CMSG_LEN
  cmsglen = CMSG_LEN(sizeof(int));
#else
  cmsglen = sizeof(struct cmsghdr) + sizeof(int);
#endif

  // Send the file descriptor
  control.cmsg.cmsg_len = cmsglen;
  control.cmsg.cmsg_level = SOL_SOCKET;
  control.cmsg.cmsg_type = MY_SCM_RIGHTS;

  msg.msg_control = (char *) &control;
  msg.msg_controllen = cmsglen;

#ifdef CMSG_DATA
  *((int *)CMSG_DATA(&control.cmsg)) = fd;
#else
  *((int *) &control.cmsg.cmsg_data) = fd;
#endif

  if (sendmsg(socket, &msg, 0) < 0) {
    perror("unable to send file descriptors");
    return -1;
  }

  return 0;
}

int Opener::sendResponse(struct ResponseHeader *response) {

  struct msghdr	msg;
  struct iovec iov;

  msg.msg_name = 0L;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = 0L;
  msg.msg_controllen = 0;

  // Send data
  iov.iov_base = IOV_BASE_CAST response;
  iov.iov_len = sizeof(struct ResponseHeader);

  if (sendmsg(socket, &msg, 0) < 0) {
    perror("unable to send response");
    return -1;
  }

  return 0;
}

const char* Opener::deviceByIndex(int idx) {

  const char *device = 0L;

  for(int i = 0; devices[i]; i++)
    if(i == idx)
      device = devices[i];
  MY_ASSERT(device);
  return device;
}

bool Opener::createAuthFile(int authMethod, const char *username,
                            const char *password) {
  const char *authfile, *oldName, *newName;
  char line[100];

  if(!(authfile = authFile(authMethod)))
    return false;

  if(!(newName = authFile(authMethod, New)))
    return false;

  // copy to new file pap- or chap-secrets
  int old_umask = umask(0077);
  FILE *fout = fopen(newName, "w");
  if(fout) {
    // copy old file
    FILE *fin = fopen(authfile, "r");
    if(fin) {
      while(fgets(line, sizeof(line), fin)) {
        // look for username, "username" or 'username'
        if(matchUser(line, username))
          continue;
        fputs(line, fout);
        // in case LF is missing at EOF
        if(line[strlen(line)-1] != '\n')
          putc('\n', fout);
      }
      fclose(fin);    
    }

    // append user/pass pair
    fprintf(fout, "\"%s\"\t*\t\"%s\"\n", username, password);
    fclose(fout);
  }

  // restore umask
  umask(old_umask);

  if(!(oldName = authFile(authMethod, Old)))
    return false;

  // delete old file if any
  unlink(oldName);

  rename(authfile, oldName);
  rename(newName, authfile);

  return true;
}


bool Opener::removeAuthFile(int authMethod) {
  const char *authfile, *oldName;

  if(!(authfile = authFile(authMethod)))
    return false;
  if(!(oldName = authFile(authMethod, Old)))
    return false;

  if(access(oldName, F_OK) == 0) {
    unlink(authfile);
    return (rename(oldName, authfile) == 0);
  } else
    return false;
}


const char* Opener::authFile(int authMethod, int version) {
  switch(authMethod|version) {
  case PAP|Original:
    return PAP_AUTH_FILE;
    break;
  case PAP|New:
    return PAP_AUTH_FILE".new";
    break;
  case PAP|Old:
    return PAP_AUTH_FILE".old";
    break;
  case CHAP|Original:
    return CHAP_AUTH_FILE;
    break;
  case CHAP|New:
    return CHAP_AUTH_FILE".new";
    break;
  case CHAP|Old:
    return CHAP_AUTH_FILE".old";
    break;
  default:
    return 0L;
  }
}


bool Opener::execpppd(const char *arguments) {
  char buf[MAX_CMDLEN];
  char *args[MaxArgs];
  pid_t pgrpid;

  if(ttyfd<0)
    return false;

  switch(pppdPid = fork())
    {
    case -1:
      fprintf(stderr,"In parent: fork() failed\n");
      return false;
      break;

    case 0:
      // let's parse the arguments the user supplied into UNIX suitable form
      // that is a list of pointers each pointing to exactly one word
      strcpy(buf, arguments);
      parseargs(buf, args);
      // become a session leader and let /dev/ttySx
      // be the controlling terminal.
      pgrpid = setsid();
      if(ioctl(ttyfd, TIOCSCTTY, 0)<0)
      fprintf(stderr, "ioctl() failed.\n");
      if(tcsetpgrp(ttyfd, pgrpid)<0)
      fprintf(stderr, "tcsetpgrp() failed.\n");

      dup2(ttyfd, 0);
      dup2(ttyfd, 1);

      execve(pppdPath(), args, 0L);
      _exit(0);
      break;

    default:
      Debug("In parent: pppd pid %d\n",pppdPid);
      close(ttyfd);
      ttyfd = -1;
      return true;
      break;
    }
}


bool Opener::killpppd() {
  if(pppdPid > 0) {
    Debug("In killpppd(): Sending SIGTERM to %d\n", pppdPid);    
    if(kill(pppdPid, SIGTERM) < 0) {
      Debug("Error terminating %d. Sending SIGKILL\n", pppdPid);
      if(kill(pppdPid, SIGKILL) < 0) {
        Debug("Error killing %d\n", pppdPid);
        return false;
      }
    }
  }
  return true;
}


void Opener::parseargs(char* buf, char** args) {
  int nargs = 0;
  int quotes;

  while(nargs < MaxArgs-1 && *buf != '\0') {
    
    quotes = 0;
    
    // Strip whitespace. Use nulls, so that the previous argument is
    // terminated automatically.
     
    while ((*buf == ' ' ) || (*buf == '\t' ) || (*buf == '\n' ) )
      *buf++ = '\0';
    
    // detect begin of quoted argument
    if (*buf == '"' || *buf == '\'') {
      quotes = *buf;
      *buf++ = '\0';
    }

    // save the argument
    if(*buf != '\0') {
      *args++ = buf;
      nargs++;
    }
    
    if (!quotes)
      while ((*buf != '\0') && (*buf != '\n') &&
	     (*buf != '\t') && (*buf != ' '))
	buf++;
    else {
      while ((*buf != '\0') && (*buf != quotes))
	buf++;
      *buf++ = '\0';
    } 
  }
 
  *args = 0L;
}


bool Opener::matchUser(const char *line, const char *user) {
  int quote = 0;
  const char *p = line;

  // skip leading space and tabs
  while(*p == ' ' || *p == '\t')
    p++;
  // quoted username ?
  if(*p == '"' || *p == '\'')
    quote = *p++;
  // compare usernames
  if(strncmp(user, p, strlen(user)) != 0)
    return false;
  // check for proper termination (closing quote or whitespace)
  p += strlen(user);
  if((quote && *p != quote) ||
     (!quote && *p != ' ' && *p != '\t' && *p != '\0' && *p != '\n'))
    return false;

  return true;
}


const char* pppdPath() {
  static char *PPPDPATH = 0L;
  char *p;

  if(PPPDPATH == 0L) {
    // wasting a few bytes
    PPPDPATH = new char[strlen(PPPDSEARCHPATH)+strlen(PPPDNAME)+1];
    if(PPPDPATH == 0L) {
      fprintf(stderr, "kppp: low memory\n");
      exit(1);
    }
    const char *c = PPPDSEARCHPATH;
    while(*c != '\0') {
      while(*c == ':')
        c++;
      p = PPPDPATH;
      while(*c != '\0' && *c != ':')
        *p++ = *c++;
      *p = '\0';
      strcat(p, "/");
      strcat(p, PPPDNAME);
      if(access(PPPDPATH, F_OK) == 0)
        return PPPDPATH;
    }
    delete PPPDPATH;
    PPPDPATH = 0L;
  }
  return PPPDPATH;
}


void sighandler(int) {
  pid_t pid;

  signal(SIGCHLD, sighandler);
  if(pppdPid>0) {
    pid = waitpid(pppdPid, 0L, WNOHANG);
    if(pid != pppdPid) {
      fprintf(stderr, "received SIGCHLD from unknown origin.\n");
    } else {
      Debug("It was pppd that died");
      pppdPid = -1;
      Debug("Sending %i a SIGUSR1", getppid());
      kill(getppid(), SIGUSR1);
    }
  } else
    fprintf(stderr, "received unexpected SIGCHLD.\n");
}
