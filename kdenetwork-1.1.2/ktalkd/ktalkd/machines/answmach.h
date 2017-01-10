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
#include "../includ.h"
#include "talkconn.h"
#include <stdio.h>

/** Implements the answering machine. */
class AnswMachine
{
  public:
    /** Constructor.
     * @param r_addr Remote machine IP address
     * @param r_name Remote user name
     * @param l_name Local user name
     * @param _mode Answering machine mode, cf defs.h */
    AnswMachine(struct in_addr r_addr,
                char * r_name,
                char * l_name,
                int _mode);

    /** Destructor. */
    virtual ~AnswMachine();

    /** Launch the machine */
    virtual void start();
    
    /** Create and start a new answering machine from the given info */
    static void launchAnswMach(NEW_CTL_MSG msginfo, int mode);

  protected:

    /** Read usercfg file to know if user wants it to be launched */
    int LaunchIt(char * key);
    
    int read_message(FILE * fd);   // message to mail
    void write_headers(FILE * fd, struct hostent * hp, char *
                       messg_myaddr, int usercfg); // mail headers

    /** Do the actual talk. */
    void talk();

    // Protected members
    /** Answering machine mode */
    int mode;
    /** Talk Connection to the caller */
    TalkConnection * talkconn;
    /** Local user name (for config file. Is also the default mail addr) */
    char local_user[NEW_NAME_SIZE];
    /** Non-existent user name, to be written in the mail. */
    char NEUperson[NEW_NAME_SIZE];
    /** Caller's machine address */
    struct in_addr caller_machine_addr;
    /** User config file */
    int usercfg;
};
