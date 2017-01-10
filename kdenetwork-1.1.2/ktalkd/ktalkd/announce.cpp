/*
 * Copyright (c) 1983 Regents of the University of California.
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
 */

/* Autoconf: */
#include <config.h>

// strange symbol for HP-UX. It should not hurt on other systems.
#ifndef notdef
#define notdef 1
#endif

#include "includ.h"
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif
#else
#include <time.h>
#endif
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <netdb.h>
#include "print.h"
#include "announce.h"
#include "readconf.h"
#include "defs.h"
#include "threads.h"
#include "unixsock.h"

#ifdef HAVE_SGTTY_H
#include <sgtty.h>
#else
#ifdef HAVE_BSD_SGTTY_H
#include <bsd/sgtty.h>
#endif
#endif

#define max(a,b) ( (a) > (b) ? (a) : (b) )
#define N_LINES 5

/*
 * Announce an invitation to talk.
 *
 * Because the tty driver insists on attaching a terminal-less
 * process to any terminal that it writes on, we must fork a child
 * to protect ourselves
 */
int announce(NEW_CTL_MSG *request, const char *remote_machine, char *disp, int
             usercfg, char * callee) {

    int pid, status;
    int returncode;

    if (strstr(remote_machine,"\033")) {
        syslog(LOG_WARNING, 
               "blocked VT100 FLASH BOMB to user: %s (from field in packet contains bomb)", 
               request->r_name);
    } else if (strstr(request->l_name,"\033")) {
        syslog(LOG_WARNING,
               "blocked VT100 FLASH BOMB to user: %s (apparently from: %s)", 
               request->r_name, remote_machine);
    } else {
        if ((pid = fork())) {
            /* we are the parent, so wait for the child */
            if (pid == -1)		/* the fork failed */
                return (FAILED);
            status = wait_process(pid);
            if ((status&0377) > 0)	/* we were killed by some signal */
                return (FAILED);
            /* Get the second byte, this is the exit/return code */
            return ((status >> 8) & 0377);
        }
        /* we are the child, go and do it */

        struct passwd * pw_buf;
        /** Change our uid. Once and for all that follows. */
        pw_buf = getpwnam(request->r_name);
        if (setgid(pw_buf -> pw_gid) || setuid(pw_buf -> pw_uid))
            syslog(LOG_WARNING, "Warning: setuid: %m");

        /** Also set $HOME once and for all that follows (not used for text
            announce, but doesn't harm) */
#ifdef HAVE_FUNC_SETENV
        setenv("HOME", pw_buf -> pw_dir, 1);
#else
        char env[256] = "HOME=";
        strcat (env, pw_buf -> pw_dir);
        putenv(env);
#endif

#ifdef HAVE_KDE
        returncode = try_Xannounce(request, remote_machine, disp, usercfg, callee);
        if (returncode != SUCCESS)
#endif
            returncode = print_std_mesg( request, remote_machine, usercfg, 0 );
        _exit(returncode);
        
    }
    return (FAILED);
}

#ifdef HAVE_KDE

int try_Xannounce(NEW_CTL_MSG *request, const char *remote_machine,
                  char *disp, int usercfg, char * callee) {
  int readPipe[2];
  char ch_aux;
  int pid;
  struct timeval clock;
  struct timezone zone;
  struct tm *localclock;
  char line_buf[N_CHARS];

  char * adisp_begin, *disp_ptr, * disp_end;
  char extprg [S_MESSG]; /* Program used for notification. */
  int Xannounceok = 0; /* Set to 1 if at least one Xannounceok succeeded */
  
#ifdef PROC_FIND_USER
  if ((Options.XAnnounce) && (strlen(disp) >= 2)) {
#else
  if ((Options.XAnnounce) && ((int) request->r_tty[3] > (int) 'f')) {
#endif /* PROC_FIND_USER */

    /*
     * He is in X (probably :-) -> try to connect with external program
     */
      
   /*  No more needed : ktalkdlg will play sound itself.
        Other external programs can do whatever they want...
        Maybe a config for this could be useful to know whether the
        extprg handles the sound announcement...
    sound_or_beep(usercfg); */

    gettimeofday(&clock, &zone);
    localclock = localtime( (const time_t *) &clock.tv_sec );
    (void)snprintf(line_buf, N_CHARS, "%s@%s", request->l_name,
                   remote_machine);

#ifndef DONT_TRY_KTALK // never defined. Define if you want...
    Xannounceok = sendToKtalk ( request->r_name, line_buf );
    if (!Xannounceok) {
#endif
    if ((!usercfg) || (!read_user_config("ExtPrg", extprg, S_MESSG)))
        /* try to read extprg from user config file, otherwise : */
        strcpy(extprg,Options.extprg); /* take default */            

    /* disp can be a LIST of displays, such as ":0 :1", or
         ":0 hostname:0". Let's announce on ALL displays found.
       disp_end is the end of the display list (saved because we'll
         insert zeros).
       adisp_begin will point to a display in the list.
       disp_ptr will go char by char through disp.*/

    adisp_begin=disp;
    disp_end=disp+strlen(disp);
    for (disp_ptr=disp; disp_ptr<disp_end; disp_ptr++) {
      if (*disp_ptr==' ') {  /* the final display will be taken also,
                                as a final space has been inserted in
                                find_X_process */
        *disp_ptr='\0'; /* mark the end of the display name in the
                           list */

        pipe( readPipe );
        switch (pid = fork()) {
            case -1: {
                syslog(LOG_ERR,"Announce : Fork failed ! - %m");
                return ( FAILED );
            }
            case 0: {                

                /* set DISPLAY if it is not set yet (it is not) */
#ifdef HAVE_FUNC_SETENV
                setenv("DISPLAY", adisp_begin, 0);
#else
	        char env[256] = "DISPLAY=";
		strcat (env, adisp_begin);
                putenv(env);
#endif

                if (Options.debug_mode)
                {
                    syslog(LOG_DEBUG, "Trying to run '%s' at '%s' as '%s'", extprg, 
                           getenv("DISPLAY"), request->r_name );
                    if (callee) message("With mention of callee : %s",callee);
                }
                /*
                 * point stdout of external program to the pipe
                 * announce a talk request by execing external program
                 */
                dup2( readPipe[1], STDOUT_FILENO );

                if (callee)
                    execl( extprg, extprg, line_buf, callee, 0 );
                else
                    execl( extprg, extprg, line_buf, 0 );
                
                /*
                 * failure - tell it to father
                 */
                ch_aux = '!';
                write( readPipe[1], &ch_aux, 1 );
                syslog(LOG_WARNING, "execl: %m");
                _exit( 0 );
            }
            default: {
                /* Register the new process for waitpid */
                new_process();
                /*
                 * I don't know any way how to find that external program run ok
                 * so parent returns always SUCCESS
                 * Well I know a solution - read the pipe :-) - It blocks, so
                 * we can't wait the reaction of the user
                 */
                read( readPipe[0], &ch_aux, 1 );
                if (Options.debug_mode) syslog(LOG_DEBUG, "Child process sent : %c",ch_aux);  
                close( readPipe[0] );
                close( readPipe[1] );
                if (ch_aux == '#') {
                    message("OK - Child process responded"); /* for debugging */
                    Xannounceok = 1;
                } else message("KO - External program failed");
            } /* default */
        } /* switch */
        adisp_begin=disp_ptr+1;
      } /* if */
    } /* for */
#ifndef DONT_TRY_KTALK
    } // if
#endif

    if (Xannounceok) {
#ifndef NO_TEXT_ANNOUNCE_IF_X
        // never defined. Define it if you don't want text announce in 
        // addition to X announce
        if ((request->r_tty[0]!='\0') && ((int)request->r_tty[3]<(int)'f'))  {
	  // Not a pseudo terminal (such as an xterm, ...)
            message("Doing also text announce on %s",request->r_tty);
            print_std_mesg(request, remote_machine, usercfg, 1 /*force no sound*/);
        }
#endif
        return (SUCCESS);
    }
  }
  return (FAILED);
}
#endif /* HAVE_KDE */

/*
 * See if the user is accepting messages. If so, announce that 
 * a talk is requested.
 */

int print_std_mesg( NEW_CTL_MSG *request, const char *remote_machine, int
                    usercfg, int force_no_sound ) {

  char full_tty[32];
  FILE *tf;
  struct stat stbuf;
  
  (void)snprintf(full_tty, sizeof(full_tty), "%s/%s", _PATH_DEV, request->r_tty);
  if (access(full_tty, 0) != 0)
    return (FAILED);
  if ((tf = fopen(full_tty, "w")) == 0)
    return (PERMISSION_DENIED);
  /*
   * On first tty open, the server will have
   * it's pgrp set, so disconnect us from the
   * tty before we catch a signal.
   */
#ifdef _SCO_DS
#warning "Not sure what to do here..."
#else
  ioctl(fileno(tf), TIOCNOTTY, (struct sgttyb *) 0);
#endif
  if (fstat(fileno(tf), &stbuf) < 0)
    return (PERMISSION_DENIED);
  if ((stbuf.st_mode&020) == 0)
    return (PERMISSION_DENIED);
  print_mesg(tf, request, remote_machine, usercfg, force_no_sound);
  fclose(tf);
  return (SUCCESS);
}

/*
 * Build a block of characters containing the message. 
 * It is sent blank filled and in a single block to
 * try to keep the message in one piece if the recipient
 * in in vi at the time
 */
void print_mesg(FILE * tf, NEW_CTL_MSG * request, const char *
                remote_machine, int usercfg, int force_no_sound)
{
	struct timeval clock;
	struct timezone zone;
	struct tm *localclock;
	char line_buf[N_LINES][N_CHARS];
	char buffer [N_CHARS];
	int sizes[N_LINES];
	char big_buf[N_LINES*N_CHARS];
	char *bptr, *lptr;
	int i, j, max_size;

	char * remotemach = new char[strlen(remote_machine)+1];
	strcpy(remotemach,remote_machine);
	/* We have to duplicate it because param_remote_machine is contained
	   in the hostent structure, and will be erased by gethostbyname. */
	
	char localname[SYS_NMLN];
	strcpy(localname,gethostbyname(Options.hostname)->h_name);
	/* We have to duplicate localname also. Same reasons as above ! */
	
	const char *remotename = gethostbyname(remotemach)->h_name;
	
	i = 0;
	max_size = 0;
	gettimeofday(&clock, &zone);
	localclock = localtime( (const time_t *) &clock.tv_sec );
	(void)snprintf(line_buf[i], N_CHARS, " ");
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	(void)snprintf(line_buf[i], N_CHARS, Options.announce1,
		      localclock->tm_hour , localclock->tm_min );
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	snprintf(buffer, N_CHARS, "%s@%s", request->l_name, remotename);
	snprintf(line_buf[i], N_CHARS, Options.announce2, buffer);
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;

	if (!(strcmp(localname,remotename))) {
	  snprintf(line_buf[i], N_CHARS, Options.announce3, request->l_name);
	} else {
	  snprintf(line_buf[i], N_CHARS, Options.announce3, buffer);
	}

	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	(void)snprintf(line_buf[i], N_CHARS, " ");
	sizes[i] = strlen(line_buf[i]);
	sizes[i] = strlen(line_buf[i]);
	max_size = max(max_size, sizes[i]);
	i++;
	bptr = big_buf;
        if (!force_no_sound) /* set if a X announce has been done */
            if (sound_or_beep(usercfg)) /* if no sound then : */
                *bptr++ = ''; /* send something to wake them up */
	*bptr++ = '\r';	/* add a \r in case of raw mode */
	*bptr++ = '\n';
	for (i = 0; i < N_LINES; i++) {
		/* copy the line into the big buffer */
		lptr = line_buf[i];
		while (*lptr != '\0')
			*(bptr++) = *(lptr++);
		/* pad out the rest of the lines with blanks */
		for (j = sizes[i]; j < max_size + 2; j++)
			*(bptr++) = ' ';
		*(bptr++) = '\r';	/* add a \r in case of raw mode */
		*(bptr++) = '\n';
	}
	*bptr = '\0';
	fprintf(tf, big_buf);
	fflush(tf);
#ifdef _SCO_DS
#warning "Not sure what to do here..."
#else
	ioctl(fileno(tf), TIOCNOTTY, (struct sgttyb *) 0);
#endif
        delete remotemach;
}

int play_sound(int usercfg)
{
     int pid, status, returncode;
     char sSoundPlayer[S_CFGLINE], sSoundFile[S_CFGLINE];
     char sSoundPlayerOpt[S_CFGLINE];
     /* We must absolutely read the configuration before forking,
        because the father will close the file soon !! */
     if ((!usercfg) || (!read_user_config("SoundPlayer",sSoundPlayer,S_CFGLINE)))
         strcpy(sSoundPlayer,Options.soundplayer);
     message(sSoundPlayer);
     if ((!usercfg) || (!read_user_config("SoundPlayerOpt",sSoundPlayerOpt,S_CFGLINE)))
         strcpy(sSoundPlayerOpt,Options.soundplayeropt);
     message(sSoundPlayerOpt);
     if ((!usercfg) || (!read_user_config("SoundFile",sSoundFile,S_CFGLINE)))
         strcpy(sSoundFile,Options.soundfile);
     message(sSoundFile);

     if ((pid = fork())) {
         /* we are the parent, so wait for the child */
         if (pid == -1)          /* the fork failed */
         {
             syslog(LOG_ERR,"Fork before play_sound : FAILED.");
             return (FAILED);
         }
         status = wait_process(pid);
         if ((status&0377) > 0)    /* we were killed by some signal */
             return (FAILED);
         /* Get the second byte, this is the exit/return code */
         return ((status >> 8) & 0377);
     }
     /* we are the child, go and do it */

     if (strlen(sSoundPlayerOpt)>0)
         returncode = execl(sSoundPlayer,sSoundPlayer/*arg0*/,sSoundPlayerOpt,sSoundFile,NULL);
     else
         returncode = execl(sSoundPlayer,sSoundPlayer/*arg0*/,sSoundFile,NULL);

     message(strerror(errno));
     syslog(LOG_ERR,"ERROR PLAYING SOUND FILE !!!");
     syslog(LOG_ERR, "%s,%s,%s",sSoundPlayer,sSoundFile,sSoundPlayerOpt);
     syslog(LOG_ERR,"RETURN-CODE : %d",returncode);
     
     _exit(returncode);
     /*NOTREACHED*/
     return 0;
}

/*
 *  Calls play_sound if necessary. Returns 1 if a tty beep is needed.
 */
int sound_or_beep(int usercfg)
{
     int bSound;
     int ret;
     if ((!usercfg) || (!read_bool_user_config("Sound",&bSound)))
         bSound=Options.sound;
     
     message("Sound option in sound_or_beep : %d",bSound);
     
     if (bSound)
       {
	/* try to play sound, otherwise beeps (if not in X) */
	  ret = play_sound(usercfg); /* 1 = it didn't work. ^G needed */
       }
     else ret = 1; /* ^G needed */
     return ret;
}
