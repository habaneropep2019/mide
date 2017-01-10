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

/*
 * Routines to handle insertion, deletion, etc on the table
 * of requests kept by the daemon. Nothing fancy here, linear
 * search on a double-linked list. A time is kept with each 
 * entry so that overly old invitations can be eliminated.
 */

#include "table.h"
#include <sys/param.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "print.h"
#include "defs.h"
#include "machines/forwmach.h"

#define MAX_ID 16000	/* << 2^15 so I don't have sign troubles */

#define NIL ((TABLE_ENTRY *)0)

/*
 * Look in the table for an invitation that matches the current
 * request looking for an invitation
 */
NEW_CTL_MSG * KTalkdTable::find_match(register NEW_CTL_MSG *request)
{
    register TABLE_ENTRY *ptr;
    time_t current_time;

    (void) gettimeofday(&tp, &txp);
    current_time = tp.tv_sec;
    for (ptr = table; ptr != NIL; ptr = ptr->next) {
        if ((ptr->time - current_time) > MAX_LIFE) {
            /* the entry is too old */
            message("deleting expired entry : id %d",
                     ptr->request.id_num);
            delete_entry(ptr);
            continue;
        }
        if ((strcmp(request->l_name, ptr->request.r_name) == 0) &&
            (strcmp(request->r_name, ptr->request.l_name) == 0) &&
            (ptr->request.type == LEAVE_INVITE) && (ptr->fwm == 0L))
        {   /* Not the forw. machines : they aren't stored to match LOOK_UPs */
            message("Found match : id %d", ptr->request.id_num);
            return (&ptr->request);
        }
    }
    return ((NEW_CTL_MSG *)0);
}

/*
 * Look for an identical request, as opposed to a complimentary
 * one as find_match does 
 */
NEW_CTL_MSG * KTalkdTable::find_request(register NEW_CTL_MSG *request)
{
    register TABLE_ENTRY *ptr;
    time_t current_time;

    (void) gettimeofday(&tp, &txp);
    current_time = tp.tv_sec;
    /*
     * See if this is a repeated message, and check for
     * out of date entries in the table while we are it.
     */
    for (ptr = table; ptr != NIL; ptr = ptr->next) {
        if ((ptr->time - current_time) > MAX_LIFE) {
            /* the entry is too old */
            message("deleting expired entry : id %d",
                     ptr->request.id_num);
            delete_entry(ptr);
            continue;
        }
        if (strcmp(request->r_name, ptr->request.r_name) == 0 &&
            strcmp(request->l_name, ptr->request.l_name) == 0 &&
            request->type == ptr->request.type &&
            request->pid == ptr->request.pid) {
            /* update the time if we 'touch' it */
            ptr->time = current_time;
            message("Found identical request : id %d", ptr->request.id_num);
            return (&ptr->request);
        }
    }
    return ((NEW_CTL_MSG *)0);
}

void KTalkdTable::insert_table(NEW_CTL_MSG *request, NEW_CTL_RESPONSE *response, ForwMachine * fwm)
{
    register TABLE_ENTRY *ptr;
    time_t current_time;

    gettimeofday(&tp, &txp);
    current_time = tp.tv_sec;
    request->id_num = new_id();
    message("Stored as id %d",request->id_num);
    if (response != 0L) response->id_num = htonl(request->id_num);
    /* insert a new entry into the top of the list */
    ptr = new TABLE_ENTRY;
    if (ptr == NIL) {
        syslog(LOG_ERR, "insert_table: Out of memory");
        _exit(1);
    }
    ptr->fwm = fwm;
    ptr->time = current_time;
    ptr->request = *request;
    ptr->next = table;
    if (ptr->next != NIL)
        ptr->next->last = ptr;
    ptr->last = NIL;
    table = ptr;
}

/*
 * Generate a unique non-zero sequence number
 */
int KTalkdTable::new_id()
{
    static int current_id = 0;

    current_id = (current_id + 1) % MAX_ID;
    /* 0 is reserved, helps to pick up bugs */
    if (current_id == 0)
        current_id = 1;
    return (current_id);
}

/*
 * Delete the invitation with id 'id_num'
 */
int KTalkdTable::delete_invite(int id_num)
{
    register TABLE_ENTRY *ptr;

    ptr = table;
    message("delete_invite(%d)", id_num);
    for (ptr = table; ptr != NIL; ptr = ptr->next) {
        if (ptr->request.id_num == id_num)
            break;
    }
    if (ptr != NIL) {
        if (ptr->fwm) {
            ptr->fwm->sendDelete(); // Calls processDelete() in the child process.
        }
        message("Deleted : id %d", ptr->request.id_num);
        delete_entry(ptr);
        return (SUCCESS);
    }
    return (NOT_HERE);
}

/*
 * Classic delete from a double-linked list
 */
void KTalkdTable::delete_entry(register TABLE_ENTRY *ptr)
{
    if (table == ptr)
        table = ptr->next;
    else if (ptr->last != NIL)
        ptr->last->next = ptr->next;
    if (ptr->next != NIL)
        ptr->next->last = ptr->last;
    delete ptr;
}

KTalkdTable::~KTalkdTable()
{
      register TABLE_ENTRY *ptr;
      ptr = table;
      message("final_clean()");
      for (ptr = table; ptr != 0L; ptr = ptr->next) {
          if (ptr->fwm != 0L)
          {
              message("CLEAN : Found a forwarding machine to clean : id %d",ptr->request.id_num);
              delete ptr->fwm;
          }
          message("CLEAN : Deleting id %d", ptr->request.id_num);
          delete_entry(ptr);
      }
}
