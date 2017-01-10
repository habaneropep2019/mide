/*
 * Copyright (c) 1983 Regents of the University of California, (c) 1998 David Faure.
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

#if defined(LIBM_SCCS) && !defined(lint)
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

/*
 * The top level of the daemon, the format is heavily borrowed
 * from rwhod.c. Basically: find out who and where you are; 
 * disconnect all descriptors and ttys, and then endless
 * loop on waiting for and processing requests
 */

#include "includ.h"

#include <sys/wait.h>
#include <sys/utsname.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "print.h"
#include "readconf.h"
#include "defs.h"
#include "threads.h"
#include "table.h"
#include "machines/answmach.h"
#include "machines/talkconn.h"

#define TIMEOUT 20
 /* TIMEOUT was 30, but has been reduced to remove the
    zombie process (answering machine) as soon as possible */
#define MAXIDLE 120
 /* #define MAXIDLE 30     for debugging purposes */

#define NB_MAX_CHILD 20 /* Max number of answering / forwarding machines at a time */

long	lastmsgtime;

void	timeout(int dummy);

KTalkdTable * ktable;

/* the following copies defaults values to 'Options.*' variables so that
 * configuration file can overwrite them */

int main(int argc, char *argv[])
{
        ktable = new KTalkdTable();
        NEW_CTL_MSG		request;
        NEW_CTL_RESPONSE	response;
        NEW_CTL_MSG *mp = &request;
        int  ret_value = PROC_REQ_OK; /* return value from process_request */
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

        /* get local machine name */
        // using 'uname' instead of 'gethostname', as suggested by Stephan Kulow
        struct utsname buf;
        if (uname (&buf) == -1) {
            syslog (LOG_ERR, "Unable to get name of local host : %m");
            _exit(1);
        }
        strcpy(Options.hostname, buf.nodename);

	/* if (gethostname(Options.hostname, sizeof (Options.hostname) - 1) < 0) {
		syslog(LOG_ERR, "gethostname: %m");
		_exit(1);
	}*/
	if (chdir(_PATH_DEV) < 0) {
		syslog(LOG_ERR, "chdir: %s: %m", _PATH_DEV);
		_exit(1);
	}
	if (argc > 1 && strcmp(argv[1], "-d") == 0)
		Options.debug_mode = 1;

        TalkConnection::init(); /* global initialization */

        signal(SIGALRM, timeout);
        alarm(TIMEOUT);

	process_config_file(); /* read configuration */

	for (;;) {

                if (Options.answmach && (ret_value>=PROC_REQ_MIN_A))
                {
                    message("Launch answer machine, mode %d.", ret_value);
                    AnswMachine::launchAnswMach(request, ret_value);
                    new_process();
                }
                
                cc = recv(talkd_sockt, (char *)mp, sizeof (*mp), 0);
		if (cc != sizeof (*mp)) {
			if ((cc < 0) && (errno != EINTR))
                            syslog(LOG_WARNING, "recv: %m");
			continue;
		}
		lastmsgtime = time(0);
		ret_value = process_request(mp, &response);
                if (ret_value != PROC_REQ_FORWMACH)
                {
                    /* can block here, is this what I want? */
                    cc = sendto(talkd_sockt, (char *)&response,
                                sizeof (response), 0, (struct sockaddr *)&mp->ctl_addr,
                                sizeof (mp->ctl_addr));
                    if (cc != sizeof (response))
			syslog(LOG_WARNING, "sendto: %m");
                }
        }
}

void timeout(int dummy)
{
    int ok_exit = ack_process();
    (void)dummy; /* to avoid warning */
    if (ok_exit && (time(0) - lastmsgtime >= MAXIDLE)) {
        delete ktable;
        _exit(0);
    }
    alarm(TIMEOUT);
}
