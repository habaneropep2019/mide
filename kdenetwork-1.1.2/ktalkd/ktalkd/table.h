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
#ifndef TABLE_H
#define TABLE_H

#include "includ.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif
#else
#include <time.h>
#endif

class ForwMachine;

typedef struct table_entry TABLE_ENTRY;

struct table_entry {
    NEW_CTL_MSG request;
    long	time;
    ForwMachine * fwm;
    TABLE_ENTRY *next;
    TABLE_ENTRY *last;
};

class KTalkdTable
{
  public:
    KTalkdTable() : table (0L) {};
    ~KTalkdTable();
    void insert_table(NEW_CTL_MSG *request, NEW_CTL_RESPONSE *response, ForwMachine * fwm);
    int delete_invite(int id_num);
    NEW_CTL_MSG * find_match(register NEW_CTL_MSG *request);
    NEW_CTL_MSG * find_request(register NEW_CTL_MSG *request);
    int new_id();
    TABLE_ENTRY * getTable() { return table; }

  private:
    void delete_entry(register TABLE_ENTRY *ptr);

    struct timeval tp;
    struct timezone txp;

    TABLE_ENTRY *table;
};
#endif
