/*
 * Copyright (c) 1983  Regents of the University of California, (c) 1997 David Faure
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "answmach.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/ioctl.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <syslog.h>
#include <netdb.h>
#include "../defs.h"
#include "../print.h"
#include "../readconf.h"

#define A_LONG_TIME 10000000  /* seconds before timeout */

/** AnswMachine constructor */
AnswMachine::AnswMachine(struct in_addr r_addr,
                         char * r_name,
                         char * l_name,
                         int _mode)
{
    /* Copy the answering machine mode */
    mode = _mode;
    /* Copy the local user name (i.e. the callee, existent or not) */
    strcpy(local_user, r_name);
    /* Copy the caller's machine address */
    caller_machine_addr = r_addr;

    /* Create a talk connection */
    talkconn = new TalkConnection(r_addr,
                                  l_name, /* the caller (remote) */
                                  local_user,
                                  ntalkProtocol);     /* the callee (local) */
    if (mode==PROC_REQ_ANSWMACH_NOT_HERE)
    {
        /* The caller is trying to talk to somebody this system doesn't know.
           We can display a NEU banner (non-existent user) and take a message
           for Options.NEU_user (root?). */                  
        strncpy(NEUperson,local_user,NEW_NAME_SIZE); /* the person the talk was aimed to */
        strncpy(local_user,Options.NEU_user,NEW_NAME_SIZE); /* for mail address, config file... */
    } else *NEUperson='\0';
}

AnswMachine::~AnswMachine()
{
    delete talkconn;
}

int AnswMachine::LaunchIt(char * key)
{
    int launchIt = 1;
    if (usercfg) {
        read_bool_user_config(key,&launchIt);
        if (launchIt==0)
            message("Not launched. Option set to 0 : ", key);
    }
    return launchIt;
}

void AnswMachine::start()
{
    /* Only wait if somebody could possibly answer.
       (The 'ringing your party again' has just been displayed,
       we want to leave a second chance for the callee to answer.)

       If NEU/not logged, start quickly. (Wait just a little for the 
       LEAVE_INVITE to come.) */
    sleep( (mode==PROC_REQ_ANSWMACH) ? Options.time_before_answmach : 1 );
    
    usercfg = init_user_config(local_user);
    
    if (LaunchIt("Answmach"))
    {
        talkconn->open_sockets();
        
        if (talkconn->look_for_invite(1/*mandatory*/))
            /* otherwise, either the caller gave up before we 
               started or the callee answered ... */
        {
            /* There was an invitation waiting for us,
             * so connect with the other (hopefully waiting) party */
            if (talkconn->connect())
            {
                /* send the first 3 chars, machine dependent */
                talkconn->set_edit_chars();
                
                /* Do the talking */
                talk();
            }
        }
    }
    if (usercfg) end_user_config();
}

/** The actual talking (user to answering machine) */
void AnswMachine::talk()
{
     char command[S_COMMAND];
     char messg_myaddr [S_MESSG];
     struct hostent *hp;
     FILE * fd = 0;      /* file descriptor, to write the message */
     char customline[S_CFGLINE];
     
#ifndef OLD_POPEN_METHOD  // never defined. Kept for systems without e.g. mkstemp.
     char fname[256];
     int fildes;
     int oldumask = umask(066);
       /* set permissions for temp file to rw- --- --- */
     int emptymail; /* 1 if empty mail allowed */
#endif
     int something_entered;
     
     hp = gethostbyaddr((char *)&caller_machine_addr, sizeof (struct in_addr), AF_INET);
     if (hp == (struct hostent *)0) 
	  TalkConnection::p_error("Answering machine : Remote machine unknown.");

     if ((!usercfg) || (!read_user_config("Mail",messg_myaddr,S_MESSG-1)))
       strcpy(messg_myaddr,local_user);

#ifdef OLD_POPEN_METHOD   // never defined
     snprintf(command,S_COMMAND,"%s %s",Options.mailprog,messg_myaddr);

    fd = popen(command,"w");
    if (!fd) 
      {
	snprintf(customline,S_CFGLINE,"Unable to open a pipe towards %s.",Options.mailprog);
	TalkConnection::p_error(customline);
      }
#else

    sprintf(fname, _PATH_TMP"/ktalkdXXXXXX");
    if ((fildes = mkstemp(fname)) == -1 || (fd = fdopen(fildes, "w+")) == 0) {
        TalkConnection::p_error("Unable to open temporary file");
    }

    umask(oldumask);
#endif
    
    write_headers(fd, hp, messg_myaddr, usercfg);

    /* read other options before setting usercfg to 0, below. */
    if ((!usercfg) || (!read_bool_user_config("EmptyMail",&emptymail)))
      /* try from user config file, otherwise default : */
      emptymail = 1;

    message("Connection established");

    if (usercfg) {
      if (!read_user_config("Msg1",customline,S_CFGLINE-1))
	{ message("Error reading Msg1"); end_user_config(); usercfg=0; }
    }

    /* No user-config'ed banner */
    if (!usercfg)
    { /* => Display Options.invitelines */
         talkconn->write_banner(Options.invitelines);
    }
    else if (mode==PROC_REQ_ANSWMACH_NOT_HERE)
    { /* => Display Options.NEUBanner* */
         talkconn->write_banner(Options.NEUBanner1);
         talkconn->write_banner(Options.NEUBanner2);
         talkconn->write_banner(Options.NEUBanner3);
    } else {
	 int linenr = 1; 
	 /* number of the Msg[1-*] line. is set to 0 after displaying banner*/
	 char m[]="Msg1"; /* used as key to read configuration */
	 
	 while (linenr) /* still something to write to the caller */
	      {
   		   talkconn->write_banner(customline);
                   
		   /* read next line in custom file. */
		   m[3]=(++linenr)+'0';
		   if (!read_user_config(m,customline,S_CFGLINE-1))
			linenr=0; /* end of message */			
	      }
    }
    /* Banner displayed. Let's take the message. */
    something_entered = read_message(fd);
#ifdef OLD_POPEN_METHOD   // never defined
    pclose(fd);
#else
    fclose(fd);

    if (something_entered || emptymail)
    { /* Don't send empty message, except if 'EmptyMail' has been set */
        int retcode;
	snprintf(command,S_COMMAND,"cat %s | %s %s",fname,Options.mailprog,messg_myaddr);
        retcode = system(command);
        if ((retcode==127) || (retcode==-1))
          syslog(LOG_ERR,"system() error : %m");
        else if (retcode!=0)
          syslog(LOG_WARNING,"%s : %m", command);
    }
    (void)unlink(fname);
#endif
}

void AnswMachine::write_headers(FILE * fd, struct hostent * hp, char *
                          messg_myaddr,  int usercfg)
{
    char messg [S_MESSG];
    char messg_tmpl [S_MESSG];
    char * r_user = talkconn->get_caller_name();

    /* if using mail.local, set 'Date:' and 'From:', because they will be missing otherwise */
    int ismaillocal = (strstr(Options.mailprog,"mail.local")!=NULL);
    if (ismaillocal)
            /* should we check only the end of the name ? */
      {
	time_t tmp = time(0);
	snprintf(messg,S_MESSG,"Date: %s",ctime(&tmp)); /* \n is included in ctime */
	fwrite(messg,strlen(messg),1,fd); /* Date */

	snprintf(messg,S_MESSG,"From: %s@%s\n",r_user,hp->h_name);
	fwrite(messg,strlen(messg),1,fd); /* From */
      }
	
    snprintf(messg,S_MESSG,"To: %s\n",messg_myaddr);
    fwrite(messg,strlen(messg),1,fd); /* To */

    if ((!usercfg) || (!read_user_config("Subj",messg_tmpl,S_CFGLINE)))
      /* try from user config file, otherwise default subject: */
      strcpy(messg_tmpl,"Message from %s");
    snprintf(messg,S_MESSG,messg_tmpl,r_user);
    fwrite("Subject: ",9,1,fd);
    fwrite(messg,strlen(messg),1,fd); /* Subject */
    fwrite("\n",1,1,fd);

    if (!ismaillocal) 
      {  /* No need to set Reply-To if From has been set correctly */
	snprintf(messg,S_MESSG,"Reply-To: %s@%s\n",r_user,hp->h_name);
	fwrite(messg,strlen(messg),1,fd); /* Reply-To */
      }

    fwrite("\n",1,1,fd); /* empty line -> end of headers */

    if ((!usercfg) || (!read_user_config("Head",messg_tmpl,S_CFGLINE)))
      /* try from user config file, otherwise default headline: */
      strcpy(messg_tmpl,"Message left in the answering machine, by %s@%s");
    snprintf(messg,S_MESSG,messg_tmpl,r_user,hp->h_name);

    if (mode==PROC_REQ_ANSWMACH_NOT_HERE) 
    {
        strcat(messg," => '");
        strcat(messg,NEUperson);
        strcat(messg,"'");
    }
    fwrite(messg,strlen(messg),1,fd); /* First line of the message */
    fwrite("\n\n",2,1,fd);
}

int AnswMachine::read_message(FILE * fd) // returns 1 if something has been entered
{
     register int read_template, sockt_mask;
     int read_set, nb;
     int pos = 0;       // position on the line. left=0.
     int something = 0; // nothing entered by caller up to now.
     struct timeval wait;
     char buff[BUFSIZ];
     char line[80] = ""; // buffer for current line
     char char_erase = talkconn->get_char_erase();

     sockt_mask = (1<<talkconn->get_sockt());
     read_template = sockt_mask;

     for (;;) {
	  read_set = read_template;
	  wait.tv_sec = A_LONG_TIME;
	  wait.tv_usec = 0;

	  nb = select(32, (fd_set *) &read_set, 0, 0, &wait);
	  if (nb <= 0) {
	       if (errno == EINTR) {
		    read_set = read_template;
		    continue;
	       } /* panic, we don't know what happened */
	       TalkConnection::p_error("Unexpected error from select");
	  }
	  if (read_set & sockt_mask) { 
	       int i;
	       /* There is data on sockt */
	       nb = read(talkconn->get_sockt(), buff, sizeof buff);
	       if (nb <= 0) {
		    message("Connection closed. Exiting");
		    break;
	       }
               something = 1;
	       for (i=0; i<nb; i++ ) {
		    if ((buff[i]==char_erase) && (pos>0)) /* backspace */
			 pos--;
		    else {
			 if (pos == 79) {
			      fwrite(line,pos,1,fd);
			      pos = 0;
			 }
			 line[pos++]=buff[i];
			 if (buff[i]=='\n') {
			      fwrite(line,pos,1,fd);
			      pos = 0;
			 }
		    }
	       }
	  } /* if read_set ... */
     }
     if (pos>0) { line[pos++]='\n'; fwrite(line,pos,1,fd); }
                                    /* last line */
     return something; // 1 if something entered.
}

/** Create and start a new answering machine from the given info */
void AnswMachine::launchAnswMach(NEW_CTL_MSG msginfo, int mode) 
 {
  if ((fork()) == 0) /* let's fork to let the daemon process other messages */
    {

#define	satosin(sa)	((struct sockaddr_in *)(sa))

        AnswMachine * am = new AnswMachine(
             (satosin(&msginfo.ctl_addr))->sin_addr, /* Caller's machine address */
             msginfo.r_name,
             msginfo.l_name,
             mode);
        am->start();
        delete am;

        // exit the child
        exit(-1);
    }
 }

#if defined(ultrix) || defined(_UNIXWARE)

/*
 * Copyright (c) 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

extern "C" {

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)mktemp.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

static int _gettemp(char *path, register int *doopen);

mkstemp(char *path)
{
	int fd;

	return (_gettemp(path, &fd) ? fd : -1);
}

/*
char *
mktemp(char *path)
{
	return(_gettemp(path, (int *)0) ? path : (char *)0);
}
*/

static
_gettemp(char *path, register int *doopen)
{
	extern int errno;
	register char *start, *trv;
	struct stat sbuf;
	u_int pid;

	pid = getpid();
	for (trv = path; *trv; ++trv);		/* extra X's get set to 0's */
	while (*--trv == 'X') {
		*trv = (pid % 10) + '0';
		pid /= 10;
	}

	/*
	 * check the target directory; if you have six X's and it
	 * doesn't exist this runs for a *very* long time.
	 */
	for (start = trv + 1;; --trv) {
		if (trv <= path)
			break;
		if (*trv == '/') {
			*trv = '\0';
			if (stat(path, &sbuf))
				return(0);
			if (!S_ISDIR(sbuf.st_mode)) {
				errno = ENOTDIR;
				return(0);
			}
			*trv = '/';
			break;
		}
	}

	for (;;) {
		if (doopen) {
			if ((*doopen =
			    open(path, O_CREAT|O_EXCL|O_RDWR, 0600)) >= 0)
				return(1);
			if (errno != EEXIST)
				return(0);
		}
		else if (stat(path, &sbuf))
			return(errno == ENOENT ? 1 : 0);

		/* tricky little algorithm for backward compatibility */
		for (trv = start;;) {
			if (!*trv)
				return(0);
			if (*trv == 'z')
				*trv++ = 'a';
			else {
				if (isdigit(*trv))
					*trv = 'a';
				else
					++*trv;
				break;
			}
		}
	}
	/*NOTREACHED*/
}

}

#endif
