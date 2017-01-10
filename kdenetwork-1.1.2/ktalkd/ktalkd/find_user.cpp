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

#include "includ.h"
#include <sys/param.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <netdb.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>
#define USE_UT_HOST
#ifndef UT_LINESIZE
#define UT_LINESIZE 12
#endif
#ifndef UT_HOSTSIZE
#define UT_HOSTSIZE 12 /*whatever*/
#undef USE_UT_HOST
#endif

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream.h>

#ifdef ALL_PROCESSES_AND_PROC_FIND_USER
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#endif

#include "print.h"
#include "defs.h"

#ifdef PROC_FIND_USER

#define DISPLAYS_LIST_MAX 200
#define DISPLAY_MAX 50

/* get DISPLAY variable of a process */
char *get_display(pid_t pid) {
    static char buf[1024];

    sprintf(buf, "/proc/%d/environ", pid);

    ifstream ifstr(buf);
    if (ifstr.fail()) return 0;
    /* message("reading %s...", fname); */

    char * dpy = 0;
    while (!ifstr.eof()) {
        ifstr.getline(buf, sizeof buf - 1, '\0');
        if (ifstr.fail()) {
            syslog(LOG_ERR, "getline: %m");
            return 0;        /* read error */
        }
        if (!strncmp("DISPLAY=", buf, 8)) {
            int l = strlen(buf + 8);
            if (l >= 2 && l < (int) sizeof buf - 1) {
                dpy = new char[ l+2 ];
                strcpy(dpy," ");
                strcat(dpy, buf + 8); /* returns " "+dpy */
                /* message("- %s",dpy); */
            }
            break;
        }
    }
    return dpy;
}

/* As utmp isn't reliable (neither xdm nor kdm logs into it ! :( ),
   we have to look at processes directly. /proc helps a lot, under linux.
   How to do it under other unixes ? */
#ifdef ALL_PROCESSES_AND_PROC_FIND_USER

/* awful global variable, but how to pass it to select_process() otherwise ?
   scandir() doesn't allow this, of course... */
unsigned int user_uid;

/* selection function used by scandir */
int select_process(const struct dirent *direntry) {
    /* returns 1 if username owns <direntry> */
    struct stat statbuf;

    /* make absolute path   (would sprintf be better ?)*/
    char abspath[20]="/proc/";
    strcat(abspath,direntry->d_name);

    if (isdigit(direntry->d_name[0])) {    /* starts with [0-9]*/
        if (!stat(abspath, &statbuf)) {      /* if exists */
            if (S_ISDIR(statbuf.st_mode)) {     /* and is a directory and */
                if (statbuf.st_uid == user_uid)  /* is owned by user_uid*/
                {
          /* We have to force errno=0, because otherwise, scandir will stop ! */
          /* the problem is that glibc sets errno in getpwnam and syslog
                (glibc-2.0.5c/6, with libstdc++ 2.7.2) */
                    errno=0;
                    return 1;
                }
                /* else message("st_uid=%d", statbuf.st_uid); */
            } /* else message("st_mode=%d", statbuf.st_mode); */
        } else message("stat error : %m");
    }

    errno=0;
    return 0;
}

/* scan /proc for any process owned by 'name'.
   If DISPLAY is set, set 'disp'.

   Called only if no X utmp entry found. */

int find_X_process(char *name, char *disp) {
    char displays_list[DISPLAYS_LIST_MAX] = " "; /* yes, one space */
    char * dispwithblanks;
    struct dirent **namelist;
    int n;

    struct passwd * pw = getpwnam(name);
    message("find_X_process");
    /* find uid */
    if ((pw) && ((user_uid=pw->pw_uid)>10))
    { /* uid<10 : no X detection because any suid program will be taken
         as owned by root, not by its real owner */
        /* scan /proc */
        n = scandir("/proc", &namelist, select_process, 0 /*no sort*/);
        if (n < 0)
            message("scandir: %m");
        else
            while(n--)
            {
                /* find DISPLAY */
                dispwithblanks = get_display(atoi(namelist[n]->d_name));
                if (dispwithblanks) {
                    /* This way, if :0.0 is in the list, :0 is not inserted */
                    if (!strstr(displays_list,dispwithblanks))
                    { /* not already in the list? */
                        char * pointlocation=strstr(dispwithblanks,".");
                        if (pointlocation) *pointlocation='\0';
                        if (!strstr(displays_list,dispwithblanks))
                        { /* display up to the '.' mustn't be already in the list */
                            strcat(displays_list,dispwithblanks+1); /* insert display (no ' ') */
                            strcat(displays_list," "); /* and a blank */
                        }
                    } /* if strtsr */
                    delete dispwithblanks;
                } /* if dispwithblanks */
            } /* while */
        if (strlen(displays_list)>1)
        {
            strcpy(disp,displays_list+1); /* removes the leading white space
                             but leave the final one, needed by announce.c */
            return 1;
        }
    } /* if pw */
    return 0;
}

#endif /* ALL_PROCESSES_AND_PROC_FIND_USER */

#ifdef UTMP_AND_PROC_FIND_USER

/*
 * Search utmp for the local user
 *
 * Priorities:
 *   login from xdm
 *   login from pseudo terminal with $DISPLAY set
 *   login from pseudo terminal
 *   other login
 */
#define PRIO_LOGIN     1       /* user is logged in */
#define PRIO_PTY       2       /* user is logged in on a
                                  pseudo terminal */
#define PRIO_DISPLAY   3       /* user is logged in on a
                                  pseudo terminal and has
                                  $DISPLAY set. */
#define PRIO_XDM       4       /* user is logged in from xdm */

#define SCMPN(a, b)	strncmp(a, b, sizeof (a))

int find_user(char *name, char *tty, char *disp) {
    struct utmp *ubuf;
    int prio = 0, status = NOT_HERE;
    struct stat statb;
    char ftty[20];
    char *ntty, *dpy;
    char ttyFound[UT_LINESIZE] = "";
    char dispFound[UT_HOSTSIZE] = "";
    
    strcpy(ftty, _PATH_DEV);
    ntty = ftty + strlen(ftty);
    setutent();
    while ((ubuf = getutent())) {
        if ((ubuf->ut_type == USER_PROCESS) &&
            (!SCMPN(ubuf->ut_name, name))) {
            
            if (*tty == '\0') {    /* no particular tty was requested */

                if (Options.XAnnounce && ubuf->ut_line[0] == ':') {
                    /* this is a XDM login (really?). GREAT! */
                    syslog(LOG_DEBUG, "XDM login: %s at %s", name, ubuf->ut_line);
                    status = SUCCESS;
                    if (prio < PRIO_XDM) {
                        strcpy(dispFound, ubuf->ut_line);
			strcat(dispFound, " ");
                        prio = PRIO_XDM;
                    }
                    continue;
                }

                strcpy(ntty, ubuf->ut_line);
                if (stat(ftty, &statb) != 0 || (!(statb.st_mode & 020)))
                {
                   message("Permission denied on %s", ntty);
                   continue; /* not a char dev */
                }

                /* device exists and is a character device */
                status = SUCCESS;
		if (Options.debug_mode) syslog(LOG_DEBUG, "Found %s at %s", name, ubuf->ut_line);
                if (prio < PRIO_LOGIN) {
                    prio = PRIO_LOGIN;
                    strcpy(ttyFound, ubuf->ut_line);
                    *dispFound = '\0';
                }

                /* the following code is Linux specific...
                 * is there a portable way to
                 * 1) determine if a device is a pseudo terminal and
                 * 2) get environment variables of an arbitrary process?
                 */
                if (strncmp("tty", ubuf->ut_line, 3) != 0 ||
                    !strchr("pqrstuvwxyzabcde", ubuf->ut_line[3])) 
                    continue; /* not a pty */

                if (!strchr(ubuf->ut_host, ':'))
                    continue; /* ut_host is not a display (e.g. telnet connection) */

                /* device is a pseudo terminal (ex : a xterm) */
		if (Options.debug_mode) syslog(LOG_DEBUG, "PTY %s, ut_host=%s",
                                                ubuf->ut_line, ubuf->ut_host);
                if (prio < PRIO_PTY) {
                    prio = PRIO_PTY;
                    strcpy(ttyFound, ubuf->ut_line);
                    strcpy(dispFound, ubuf->ut_host);
                    strcat(dispFound, " ");
                }

                dpy = get_display(ubuf->ut_pid);
                if (!dpy) continue; /* DISPLAY not set or empty */

                /* $DISPLAY is set. */
                if (Options.debug_mode) syslog(LOG_DEBUG, "Found display %s on %s",
                                                dpy, ubuf->ut_line);
                if (prio < PRIO_DISPLAY) {
                    prio = PRIO_DISPLAY;
                    strcpy(ttyFound, ubuf->ut_line);
                    strcpy(dispFound, dpy+1); /*no space*/
		    strcat(dispFound, " ");
                }
                delete dpy;
                continue;
            }
            if (!strcmp(ubuf->ut_line, tty)) {
                status = SUCCESS;
                break;
            }
        }
    }
    endutent();
    
    message("End of Utmp reading");
#if defined(HAVE_KDE) && defined(ALL_PROCESSES_AND_PROC_FIND_USER)
    if (Options.XAnnounce && prio < PRIO_DISPLAY)
        if (find_X_process(name, dispFound))
            { message(dispFound); status=SUCCESS; }
#endif        
    if (status == SUCCESS) {
        (void) strcpy(tty, ttyFound);
        (void) strcpy(disp, dispFound);
        if (Options.debug_mode)
         syslog(LOG_DEBUG, "Returning tty '%s', display '%s'", ttyFound, dispFound);
    } else message("Returning status %d",status);
    return (status);
}

#endif /*UTMP_AND_PROC_FIND_USER*/

#else  /*not PROC_FIND_USER*/

int find_user(char *name, char *tty, char *disp) {

    struct utmp ubuf;
    int status;
    FILE *fd;
    struct stat statb;
    char ftty[20];
    char ttyFound[UT_LINESIZE] = "";
    char dispFound[UT_HOSTSIZE] = "";
  
    if (!(fd = fopen(_PATH_UTMP, "r"))) {
        fprintf(stderr, "talkd: can't read %s.\n", _PATH_UTMP);
        return (FAILED);
    }
#define SCMPN(a, b)	strncmp(a, b, sizeof (a))
    status = NOT_HERE;
    (void) strcpy(ftty, _PATH_DEV);
    while (fread((char *) &ubuf, sizeof ubuf, 1, fd) == 1)
        if (!SCMPN(ubuf.ut_name, name)) {
            if (*tty == '\0') {
                /* no particular tty was requested */
                /* if (ttyFound == "")
                   status = PERMISSION_DENIED; */
                (void) strcpy(ftty+5, ubuf.ut_line);
                if (stat(ftty,&statb) == 0) {
                    if (!(statb.st_mode & 020)) /* ?character device? */
                        continue;
                    (void) strcpy(ttyFound, ubuf.ut_line);
#ifdef USE_UT_HOST
                    (void) strcpy(dispFound, ubuf.ut_host);
                    strcat(dispFound, " ");
#endif
                    status = SUCCESS;

                    syslog(LOG_DEBUG, "%s", ttyFound);
                    if ((int) ttyFound[3] > (int) 'f') {
#ifdef USE_UT_HOST
                        if (Options.debug_mode) {
                            syslog(LOG_DEBUG, "I wanna this:%s", ttyFound);
                            syslog(LOG_DEBUG, "ut_host=%s", ubuf.ut_host);
                            syslog(LOG_DEBUG, "%s", ubuf.ut_line);
                        }
#endif
                        break;
                    }
                }
            }
            if (!strcmp(ubuf.ut_line, tty)) {
                status = SUCCESS;
                break;
            }
        }
    fclose(fd);
    if (status == SUCCESS) {
        (void) strcpy(tty, ttyFound);
        (void) strcpy(disp, dispFound);
    }
    return (status);
}
#endif /*PROC_FIND_USER*/
