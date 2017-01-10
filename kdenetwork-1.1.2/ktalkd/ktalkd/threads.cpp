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

#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "print.h"

static int nb_child_to_wait=0;

#undef DEBUG_THREADS

/** Register a new process (by just increasing the number of children */
void new_process()
{
	nb_child_to_wait++;
#ifdef DEBUG_THREADS
	message("New Child. Nb child to wait : %d",nb_child_to_wait);
#endif
}

/** Wait for a given process - not registered - and see if others exited */
int wait_process(int pid)
{
    int val, status;
    do {
        val = wait(&status);
        if (val == -1) {
            if (errno == EINTR)
                continue;
            /* shouldn't happen */
            syslog(LOG_WARNING, "announce: wait: %m");
            return 1;
        }
        if (val != pid) {
            nb_child_to_wait--;
#ifdef DEBUG_THREADS
            message("Child exited. Nb child to wait : %d",nb_child_to_wait);
#endif
        }
    } while (val != pid);
    return status;
}

/** Wait for children (if any) to exit. Return 1 if no more child to wait */
int ack_process()
{
   if (nb_child_to_wait>0)
   {
        int pid;
        do {
            pid = waitpid(-1,0,WNOHANG);
            if (pid==-1)
                syslog(LOG_ERR,"Timeout. Error waiting for child.");
            if ((pid!=0) & (nb_child_to_wait>0)) {
                nb_child_to_wait--;
#ifdef DEBUG_THREADS
                message("Child exited as expected. Nb child to wait : %d",nb_child_to_wait);
#endif
            }
        } while ((pid>0) && (nb_child_to_wait>0));
	return 0;
   } else return 1; /* ok for exiting */
}

