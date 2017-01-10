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
#ifndef FORWMACH_H
#define FORWMACH_H

#include "../includ.h"
#include "../table.h"
#include "talkconn.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

// Strangely enough, SIGUSR1 doesn't get caught ... Why ??

#define SIGDELETE SIGUSR2

/* Forwarding machine scheme : (ANNOUNCE request only)

   Caller (client)      ForwMach---        Answerer (client)
                   \       |       \                        
                    \      |        \                        
   Caller (daemon)   ----ktalkd      ------Answerer (daemon)

   Caller always talks to us using ntalk (if otalk then kotalkd translates it)
   But answerer might be using otalk. Let's check for it in processLookup.
 */
/** Implements the forwarding machine. */
class ForwMachine
{

  public:
    /** Constructor.
     * @param mp Request received
     * @param forward User@host to forward the talk
     * @param _forwardMethod 3 letters to choose the method (see .talkdrc)
     * @param c_id_num announce id_num, got from caller
     * @param o_id_num our id_num in our table
     * */
    ForwMachine(const NEW_CTL_MSG * mp,
                char * forward,
                char * _forwardMethod,
                int c_id_num);

    /** Destructor. */
    virtual ~ForwMachine();

    // Child methods
    
    /** Process the incoming ANNOUNCE request. */
    void processAnnounce();

    /** Processes the LOOK_UP request from answerer. */
    void processLookup(const NEW_CTL_MSG * mp);

    /** Processes the DELETE request from caller. Called by sig_handler. */
    void processDelete();

    // Parent methods

    /** Send a DELETE signal to the child */
    void sendDelete() { kill(pid, SIGDELETE); pid = 0; }
    
    /** Checks if a LOOK_UP concerns this Forwarding Machine */
    int isLookupForMe(const NEW_CTL_MSG * mp);
    /** Used by forwMachFindMatch for NEUuser or local forward */
    char * findMatch(NEW_CTL_MSG * mp);
    
    /** Calls processLookup after finding the correct forwmachine instance in the table */
    static int forwMachProcessLookup(TABLE_ENTRY * table, const NEW_CTL_MSG * mp);

    /** Tries to find a match in the table for the given REQUEST structure.
     * @see findMatch */
    static char * forwMachFindMatch(TABLE_ENTRY * table, NEW_CTL_MSG * mp);

    /** Get static ref to current ForwMachine. For sig_handler */
    static ForwMachine * getForwMachine() { return pForwMachine; }

    /** Start the ForwMachine process. Processes the announcement and waits for signals
     * @param o_id_num Our id num in our table. */
    void start(int o_id_num);
    
  protected:
    /** Static ref to current forwmachine. For sig_handler */
    static ForwMachine * pForwMachine;

    /** Pid of the child forwmachine */
    int pid;
    
    /** Fills privates fields from forward
     * @param forward user@host to forward the talk */
    int getNames(char * forward);

    /** Respond to caller's daemon
     * @param target the caller's machine address
     * @param rp the response to send to it */
    void sendResponse(const struct sockaddr target, NEW_CTL_RESPONSE * rp);

    /** FWT Method : transmit characters from sockt1 to sockt2 */
    int transmit_chars(int sockt1, int sockt2, unsigned char * buf);

    /** FWT Method : we want to connect to both sides */
    void connect_FWT(TalkConnection * tcCaller);
    
    /** Method for the forwarding. */
    enum {FWA, FWR, FWT} forwardMethod;

    /** Answerer user name */
    char answ_user[NEW_NAME_SIZE];
    /** Answerer machine name */
    char * answ_machine_name;
    /** Answerer machine address */
    struct in_addr answ_machine_addr;
    /** Talk Connection to the answerer */
    TalkConnection * tcAnsw;
    /** id_num for the announce on answerer's machine. */
    int answ_id_num;
    
    /** Local user name, the original 'callee' */
    char local_user[NEW_NAME_SIZE];
    /** Our id_num (in our table) */
    int our_id_num;

    /** Caller's user name */
    char caller_username[NEW_NAME_SIZE];
    /** Caller's ctl_addr*/
    struct sockaddr caller_ctl_addr;
    /** Caller's machine address */
    struct in_addr caller_machine_addr;
    /** Caller's announce id_num */
    int caller_id_num;
    /** Caller's protocol */
    ProtocolType callerProtocol;
};
#endif
