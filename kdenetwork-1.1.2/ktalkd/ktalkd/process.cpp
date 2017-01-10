/*
 * Copyright (c) 1983 Regents of the University of California, 
 *           (c) 1998 David Faure.
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

/*
 * process.cpp handles the requests, which can be of three types:
 *	ANNOUNCE - announce to a user that a talk is wanted
 *	LEAVE_INVITE - insert the request into the table
 *	LOOK_UP - look up to see if a request is waiting in
 *		  in the table for the local user
 *	DELETE - delete invitation
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
#include <pwd.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#include "process.h"
#include "print.h"
#include "announce.h"
#include "find_user.h"
#include "table.h"
#include "readconf.h"
#include "defs.h"
#include "machines/forwmach.h"

extern KTalkdTable * ktable;

int prepare_response(register NEW_CTL_MSG *mp, register NEW_CTL_RESPONSE *rp)
{
	rp->type = mp->type;
	rp->id_num = htonl(0);
	rp->vers = mp->vers;
        if ((mp->vers != 0) && (mp->vers != TALK_VERSION)) {
            syslog(LOG_WARNING, "Bad protocol version %d", mp->vers);
            rp->answer = BADVERSION;
            return 0;
	}
	mp->id_num = ntohl(mp->id_num);
	mp->addr.sa_family = ntohs(mp->addr.sa_family);
	mp->ctl_addr.sa_family = ntohs(mp->ctl_addr.sa_family);
	if (mp->ctl_addr.sa_family != AF_INET) {
		syslog(LOG_WARNING, "Bad address, family %d",
		    mp->ctl_addr.sa_family);
		rp->answer = BADCTLADDR;
		return 0;
	}
	mp->pid = ntohl(mp->pid);
        return 1; /* Ok */
}

int  process_request(register NEW_CTL_MSG *mp, register NEW_CTL_RESPONSE *rp)
{
    register NEW_CTL_MSG *ptr;
    int ret;
    int usercfg = 0;    /* set if a user config file exists */

    print_request("process_request", mp);

    if (!prepare_response(mp, rp))
        return PROC_REQ_ERR;
        
        /* Ensure names end with '\0' */
    mp->l_name[NEW_NAME_SIZE-1] = '\0';
    mp->r_name[NEW_NAME_SIZE-1] = '\0';

    ret = PROC_REQ_OK;

    switch (mp->type) {

	case ANNOUNCE:
            /* Open user config file. */
            usercfg = init_user_config(mp->r_name);
            ret = do_announce(mp, rp, usercfg);
            if (usercfg) end_user_config();

            /* Store in table if normal announce or answmach replacing it.
               Not if re-announce, nor if error, nor for forwarding machine */
            if ((ret == PROC_REQ_OK) || (ret == PROC_REQ_ANSWMACH_NOT_LOGGED) 
                || (ret == PROC_REQ_ANSWMACH_NOT_HERE))
                ktable->insert_table(mp, rp, 0L);

	case LEAVE_INVITE:
            ptr = ktable->find_request(mp);
            if (ptr != (NEW_CTL_MSG *)0) {
                rp->id_num = htonl(ptr->id_num);
                rp->answer = SUCCESS;
            } else
                ktable->insert_table(mp, rp, 0L);
            break;

	case LOOK_UP:
            ptr = ktable->find_match(mp);
            if (ptr != (NEW_CTL_MSG *)0) {
                rp->id_num = htonl(ptr->id_num);
                rp->addr = ptr->addr;
                rp->addr.sa_family = htons(ptr->addr.sa_family);
                rp->answer = SUCCESS;
            } else {
                if (ForwMachine::forwMachProcessLookup(ktable->getTable(), mp)) {
                    ret = PROC_REQ_FORWMACH; // Don't send any response, forwmach will do it
                } else
                    rp->answer = NOT_HERE;
            }
            break;

	case DELETE:
            rp->answer = ktable->delete_invite(mp->id_num);
            break;

	default:
            rp->answer = UNKNOWN_REQUEST;
            break;
    }
    if (ret != PROC_REQ_FORWMACH)
        print_response("=> response", rp);
    if (mp->vers == 0) { // it's kotalkd talking to us.
        // Let's prepare an OTALK response, shifting the first 2 fields
        rp->vers /*type in otalk*/ = rp->type;
        rp->type /*answer in otalk*/ = rp->answer;
    }
    return ret;
}

int do_announce(register NEW_CTL_MSG *mp, NEW_CTL_RESPONSE *rp, int usercfg)
{
    struct hostent *hp;
    NEW_CTL_MSG *ptr;
    int result;
    char disp[DISPLAYS_LIST_MAX];
    char forward[S_CFGLINE], forwardMethod[4];
    char * callee;
    ForwMachine * fwm;

    /* Check if already in the table */
    ptr = ktable->find_request(mp);

    if ((ptr != NULL) && (
        (mp->id_num <= ptr->id_num) || (mp->id_num == (int)~0x0L))) {
        /* a duplicated request, so ignore it */
        message("dupannounce %d", mp->id_num);
        rp->id_num = htonl(ptr->id_num);
        rp->answer = SUCCESS;
        return PROC_REQ_ERR;

    } else {
        if (ptr == (NEW_CTL_MSG *) 0) {  /* Not already in the table */
            /* see if a forward has been set up */
            if (   (usercfg)
                   && (read_user_config("Forward",       forward, S_CFGLINE))
                   && (read_user_config("ForwardMethod", forwardMethod, 4  )) )
            {
                fwm = new ForwMachine(mp, forward, forwardMethod, mp->id_num);
                /* Store in table, because :
                   (Any method) : this allows to detect dupannounces.
                   (FWR & FWT) : we'll receive the LOOK_UP */
                ktable->insert_table(mp, 0L, fwm);
                fwm->start(mp->id_num);
                return PROC_REQ_FORWMACH;
            }
        }

        /* see if the user is logged */
	result = find_user(mp->r_name, mp->r_tty, disp);
        message("find_user : result = %d",result);
        
	if (result != SUCCESS) {
            message("Couldn t find user ...");
            if (result == NOT_HERE)
            { /* Not here ?  -> Start answering machine ! */
                if (getpwnam(mp->r_name)) /* Does the user exist ? */
                { /* Yes ! -> SUCCESS. */
                    message("Not logged.");
                    rp->answer = SUCCESS;
                    endpwent();
                    return PROC_REQ_ANSWMACH_NOT_LOGGED; /* answer machine. */
                } else 
                { /* Non-existent user ... */
                    endpwent();                    
                    /* output an error into the logs */

                    syslog(LOG_ERR,"User unknown : %s.",mp->r_name);
                    syslog(LOG_ERR,"The caller is : %s.",mp->l_name);
                    
                    switch (Options.NEU_behaviour) {
                        case 2: /* Paranoid setting. Do nothing. */
                            message("Paranoid setting. Do nothing.");
                            rp->answer = NOT_HERE;
                            return PROC_REQ_ERR;
                        case 0: /* Launch answering machine. */
                            message("Not here.");
                            rp->answer = SUCCESS;
                            return PROC_REQ_ANSWMACH_NOT_HERE;
                        case 1: /* NEU_user will take the talk. */
                            message("Not here. I ll take the talk.");
                            fwm = new ForwMachine(mp, Options.NEU_user, 
                                                  Options.NEU_forwardmethod, mp->id_num);
                            /* store in table, because we'll receive the LOOK_UP */
                            ktable->insert_table(mp, 0L, fwm);
                            fwm->start(mp->id_num);
                            return PROC_REQ_FORWMACH;
                    } /* switch */
                } /* getpwnam */
            } /* result */
            else {
                message("not SUCCESS, nor NOT_HERE");
                rp->answer = result; /* not SUCCESS, nor NOT_HERE*/
                return PROC_REQ_ERR;
            }
	}

#define	satosin(sa)	((struct sockaddr_in *)(sa))
	hp = gethostbyaddr((char *)&satosin(&mp->ctl_addr)->sin_addr,
                           sizeof (struct in_addr), AF_INET);
	if (hp == (struct hostent *)0) {
            rp->answer = MACHINE_UNKNOWN;
            return PROC_REQ_ERR;
	}
        
        /* Check if there is a forwarding machine on this machine,
           matching answerer = r_name and caller = l_name
           Then set callee to the initial callee, to display in ktalkdlg */
        callee = ForwMachine::forwMachFindMatch(ktable->getTable(), mp);
        
	if (ptr == (NEW_CTL_MSG *) 0) {  /* Not already in the table => announce */
            rp->answer = announce(mp, hp->h_name, disp, usercfg, callee);
            if (rp->answer == PERMISSION_DENIED) return PROC_REQ_ERR;
            message("Announce done.");
            return PROC_REQ_OK;
	} else {            
            /* This is an explicit re-announce, so update the id_num
             * field to avoid duplicates and re-announce the talk. */
            int new_id_num = ktable->new_id();
            if (Options.debug_mode)
                syslog(LOG_DEBUG, "reannounce : updating id %d to id %d",
                       ptr->id_num, new_id_num);
            ptr->id_num = new_id_num; /* update in the table */
            rp->id_num = htonl(ptr->id_num);
            rp->answer = announce(mp, hp->h_name, disp, usercfg, callee);
            return PROC_REQ_ANSWMACH;
        }
    }
}
