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

/* print.c : debug print routines */

#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include "print.h"

extern int debug_mode;

static char *types[] =
    { "leave_invite", "look_up", "delete", "announce" };
#define	NTYPES	(sizeof (types) / sizeof (types[0]))
static	char *answers[] = 
    { "success", "not_here", "failed", "machine_unknown", "permission_denied",
      "unknown_request", "badversion", "badaddr", "badctladdr" };
#define	NANSWERS	(sizeof (answers) / sizeof (answers[0]))

/** Dump a NEW_CTL_MSG structure in the logs.
 * It must be called with network byte order (after calls to hton*)
 * @param cp a string to identify the log output
 * @param mp the address of the NEW_CTL_MSG structure */
void print_request(char *cp,register NEW_CTL_MSG *mp)
{
	char tbuf[80], *tp;
        short int type = mp->type;
	
	if (type > NTYPES) {
		(void)snprintf(tbuf, sizeof(tbuf), "type %d", type);
		tp = tbuf;
	} else
		tp = types[type];
	syslog(LOG_DEBUG, "%s: %s: id %d, l_user %s, r_user %s, r_tty %s, pid %d",
	    cp, tp, ntohl(mp->id_num), mp->l_name, mp->r_name, mp->r_tty, ntohl(mp->pid));
        print_addr("    addr", (struct sockaddr_in *)&mp->addr);
        print_addr("    ctl_addr", (struct sockaddr_in *)&mp->ctl_addr);
}

/** Dump a OLD_CTL_MSG structure in the logs.
 * It must be called with network byte order (after calls to hton*)
 * @param cp a string to identify the log output
 * @param mp the address of the OLD_CTL_MSG structure */
void print_old_request(char *cp,register OLD_CTL_MSG *mp)
{
	char tbuf[80], *tp;
        short int type = mp->type;
	
	if (type > NTYPES) {
		(void)snprintf(tbuf, sizeof(tbuf), "type %d", type);
		tp = tbuf;
	} else
		tp = types[type];
	syslog(LOG_DEBUG, "%s: %s: id %d, l_user %s, r_user %s, r_tty %s, pid %d",
	    cp, tp, ntohl(mp->id_num), mp->l_name, mp->r_name, mp->r_tty, ntohl(mp->pid));
        print_addr("    addr", (struct sockaddr_in *)&mp->addr);
        print_addr("    ctl_addr", (struct sockaddr_in *)&mp->ctl_addr);
}

/** Dump a NEW_CTL_RESPONSE structure in the logs.
 * It must be called with network byte order (before calls to ntoh*)
 * @param cp a string to identify the log output
 * @param mp the address of the NEW_CTL_RESPONSE structure */
void print_response(char *cp,register NEW_CTL_RESPONSE *rp)
{
	char tbuf[80], *tp, abuf[80], *ap;
        short int type = rp->type;
        short int answer = rp->answer;
	
	if (type > NTYPES) {
		(void)snprintf(tbuf, sizeof(tbuf), "type %d", type);
		tp = tbuf;
	} else
		tp = types[type];
	if (answer > NANSWERS) {
		(void)snprintf(abuf, sizeof(abuf), "answer %d", answer);
		ap = abuf;
	} else
		ap = answers[answer];
	syslog(LOG_DEBUG, "%s: %s: %s, id %d", cp, tp, ap, ntohl(rp->id_num));
        if ((rp->type == LOOK_UP) && (rp->answer == SUCCESS))
            print_addr("    resp addr", (struct sockaddr_in *)&rp->addr);
}

/** Dump a OLD_CTL_RESPONSE structure in the logs.
 * It must be called with network byte order (before calls to ntoh*)
 * @param cp a string to identify the log output
 * @param mp the address of the OLD_CTL_RESPONSE structure */
void print_old_response(char *cp,register OLD_CTL_RESPONSE *rp)
{
	char tbuf[80], *tp, abuf[80], *ap;
        short int type = rp->type;
        short int answer = rp->answer;
	
	if (type > NTYPES) {
		(void)snprintf(tbuf, sizeof(tbuf), "type %d", type);
		tp = tbuf;
	} else
		tp = types[type];
	if (answer > NANSWERS) {
		(void)snprintf(abuf, sizeof(abuf), "answer %d", answer);
		ap = abuf;
	} else
		ap = answers[answer];
	syslog(LOG_DEBUG, "%s: %s: %s, id %d", cp, tp, ap, ntohl(rp->id_num));
        if ((rp->type == LOOK_UP) && (rp->answer == SUCCESS))
            print_addr("    resp addr", (struct sockaddr_in *)&rp->addr);
}

/* print_addr is a debug print routine for sockaddr_in structures.
 * Call with a structure in network byte order.
 * @param cp a string to identify the log output
 * @param addr the address to read */
void print_addr(char *cp, struct sockaddr_in * addr)
{
	int i;
        unsigned int s_add = addr->sin_addr.s_addr;
        char s[20] = "", d[5];
	for (i = 0; i<4; i++) {
            sprintf(d, "%ld.", s_add % 256L);
            strcat(s, d);
            s_add = s_add / 256L;
        }
	syslog(LOG_DEBUG,"%s: addr = %s port = %o, family = %o",
		cp, s, ntohs(addr->sin_port), ntohs(addr->sin_family));
}

/*
 * Debug messages
 */

#if 1

/* Set to 0 if using glibc and libstdc++2.7.2 (syslog doesn't work) */
/* Set to 1 with egcs-1.0.1 or gcc 2.8, or without KDE (c compiling) */
void message(const char *string)
{
  if (debug_mode) syslog(LOG_DEBUG,string);
}

#else

void message(const char *string)
{
  if (debug_mode) {
	char cmd[250];
	sprintf(cmd,"echo `date +%%H:%%M:%%S` %s >>/tmp/ktalkd_debug",string);
	system(cmd);
  }
}

#endif

void message2(const char *format,int value)
{
  if (debug_mode)
    {
      int len = strlen(format)+10;
      char * buf = (char *)malloc(len);
      snprintf(buf,len,format,value);
      message(buf);
      free(buf);
    }
}

void message_s(const char *format,const char *value)
{
  if (debug_mode)
    {
      int len = strlen(format)+strlen(value);
      char * buf = (char *)malloc(len);
      snprintf(buf,len,format,value);
      message(buf);
      free(buf);
    }
}
