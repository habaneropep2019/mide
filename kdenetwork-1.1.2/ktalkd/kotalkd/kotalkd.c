/*
 * Copyright (c) 1998 David Faure.
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

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  256             /* max hostname size */
#endif

#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print.h"
#include "process.h"

#define TIMEOUT 30
#define MAXIDLE 120

int	debug_mode = 0; /* set to 1 to have verbose reports from kotalkd */

long	lastmsgtime;
char	hostname[MAXHOSTNAMELEN];

void	timeout(int dummy);

int main(int argc, char *argv[])
{
        OLD_CTL_MSG	 request;
        OLD_CTL_RESPONSE response;
        OLD_CTL_MSG *mp = &request;
	int cc;
        int talkd_sockt = 0; /* if I understand well, it's inetd that opens the 
                                socket 0 for us... */

	if (getuid()) {
		fprintf(stderr, "%s: getuid: not super-user", argv[0]);
		exit(1);
	}
#ifdef LOG_PERROR
	openlog(*argv, LOG_PID || LOG_PERROR, LOG_DAEMON);
#else
	openlog(*argv, LOG_PID, LOG_DAEMON);
#endif
	if (gethostname(hostname, sizeof (hostname) - 1) < 0) {
		syslog(LOG_ERR, "gethostname: %m");
		_exit(1);
	}
	if (argc > 1 && strcmp(argv[1], "-d") == 0)
		debug_mode = 1;

        signal(SIGALRM, timeout);
        alarm(TIMEOUT);

        open_local_socket(hostname);
        
	for (;;) {
            cc = recv(talkd_sockt, (char *)mp, sizeof (*mp), 0);
            if (cc != sizeof (*mp)) {
                if ((cc < 0) && (errno != EINTR))
                    syslog(LOG_WARNING, "recv: %m");
                continue;
            }
            lastmsgtime = time(0);
            process_request(mp, &response);
        }
}

void timeout(int dummy)
{
    (void)dummy; /* to avoid warning */
    if (time(0) - lastmsgtime >= MAXIDLE)
    {
        close_local_socket();
        _exit(0);
    }
    signal(SIGALRM, timeout);
    alarm(TIMEOUT);
}
