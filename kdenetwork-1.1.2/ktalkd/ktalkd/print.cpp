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

#include "includ.h"
#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "print.h"

static char *types[] =
    { "leave_invite", "look_up", "delete", "announce" };
#define	NTYPES	(int) (sizeof (types) / sizeof (types[0]))
static	char *answers[] = 
    { "success", "not_here", "failed", "machine_unknown", "permission_denied",
      "unknown_request", "badversion", "badaddr", "badctladdr" };
#define	NANSWERS	(int) (sizeof (answers) / sizeof (answers[0]))

/** Dump a NEW_CTL_MSG structure in the logs.
 * It must be called with network byte order (after calls to hton*)
 * @param cp a string to identify the log output
 * @param mp the address of the NEW_CTL_MSG structure */
void print_request(char *cp,register NEW_CTL_MSG *mp)
{
  if (Options.debug_mode)
    {
	char tbuf[80], *tp;
	
	if (mp->type > NTYPES) {
		(void)snprintf(tbuf, sizeof(tbuf), "type %d", mp->type);
		tp = tbuf;
	} else
		tp = types[mp->type];
	syslog(LOG_DEBUG, "%s: %s: id %d, l_user %s, r_user %s, r_tty %s, pid %d",
	    cp, tp, ntohl(mp->id_num), mp->l_name, mp->r_name, mp->r_tty, ntohl(mp->pid));
        print_addr("    addr", (struct sockaddr_in *)&mp->addr);
        print_addr("    ctl_addr", (struct sockaddr_in *)&mp->ctl_addr);
    }
}

/** Dump a NEW_CTL_RESPONSE structure in the logs.
 * It must be called with network byte order (before calls to ntoh*)
 * @param cp a string to identify the log output
 * @param mp the address of the NEW_CTL_RESPONSE structure */
void print_response(char *cp,register NEW_CTL_RESPONSE *rp)
{
  if (Options.debug_mode)
    {
	char tbuf[80], *tp, abuf[80], *ap;
	
	if (rp->type > NTYPES) {
		(void)snprintf(tbuf, sizeof(tbuf), "type %d", rp->type);
		tp = tbuf;
	} else
		tp = types[rp->type];
	if (rp->answer > NANSWERS) {
		(void)snprintf(abuf, sizeof(abuf), "answer %d", rp->answer);
		ap = abuf;
	} else
		ap = answers[rp->answer];
	syslog(LOG_DEBUG, "%s: %s: %s, id %d", cp, tp, ap, ntohl(rp->id_num));
        if ((rp->type == LOOK_UP) && (rp->answer == SUCCESS))
            print_addr("    resp addr", (struct sockaddr_in *)&rp->addr);
    }
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

void message(const char *string)
{
  if (Options.debug_mode) syslog(LOG_DEBUG,string);
}

void message(const char *format,int value)
{
  if (Options.debug_mode)
    {
      int len = strlen(format)+10;
      char * buf = new char[len];
      snprintf(buf,len,format,value);
      syslog(LOG_DEBUG,buf);
      delete buf;
    }
}

void message(const char *format,const char *value)
{
  if (Options.debug_mode)
    {
      int len = strlen(format)+strlen(value);
      char * buf = new char[len];
      snprintf(buf,len,format,value);
      syslog(LOG_DEBUG,buf);
      delete buf;
    }
}
