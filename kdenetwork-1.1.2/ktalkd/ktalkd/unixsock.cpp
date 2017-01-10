/*
 * Copyright (c) 1998 Burkhard Lehner and David Faure
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

#include <config.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <qstring.h>
#include <qstrlist.h>
#include "print.h"
#include "announce.h" // for N_CHARS

bool sendToKtalk (const char *username, const char *announce)
/*
   sends an announcement to all running ktalk clients.
      username: name of the user who shall receive the announce
      announce: name and IP address of the one who requests the talk

      return value: TRUE if at least one ktalk was found and running
                    FALSE otherwise
*/

{
    // Create a socket
    int sock;
    if ((sock = socket (AF_UNIX, SOCK_DGRAM, 0)) < 0) return FALSE;
    // Bind it to a temporary file in /tmp
    struct sockaddr_un tempAddr;
    tempAddr.sun_family = AF_UNIX;
    if (tmpnam (tempAddr.sun_path) == 0 || 
        bind (sock, (struct sockaddr *) &tempAddr, sizeof (tempAddr)) == -1) {
        close (sock);
        message("Couldn't create temporary socket!");
        return FALSE;
    }

    // find sockets of running ktalk clients
    QString tempDir = "/tmp";
    QString templ = QString ("ktalk-") + username + "-";
    bool announceok = FALSE;
    char buffer [N_CHARS+2];
    buffer [0] = 1;
    strcpy (buffer + 1, announce); // announce is at most N_CHARS long.
    unsigned int announcelen = strlen(buffer);
    unsigned int len;
 
    DIR *dir = opendir (tempDir);
    struct dirent *entry;
    QStrList dirList;
    struct sockaddr_un ktalkAddr;
    ktalkAddr.sun_family = AF_UNIX;
    while ((entry = readdir (dir))) {
        // send announce to each of them
        if (strncmp (templ.data (), entry->d_name, templ.length ()) == 0) {
            QString path = tempDir + "/" + entry->d_name;
            strncpy (ktalkAddr.sun_path, path.data (), sizeof (ktalkAddr.sun_path));
            len = sendto (sock, buffer, announcelen, 0,
                          (struct sockaddr *) &ktalkAddr, sizeof (ktalkAddr));
            if (len == announcelen)
                announceok = TRUE;
        }
    }
    closedir (dir);
    if (!announceok) return FALSE;
    // at least one accepted the packet, wait for response :
    bool result = FALSE;
    fd_set readFDs;
    FD_ZERO (&readFDs);
    FD_SET (sock, &readFDs);
    char answer;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;  // Wait for answer at most 0.5 seconds
    if ( (select (sock + 1, &readFDs, 0, 0, &timeout) > 0) &&
         (recv (sock, &answer, 1, 0) == 1) ) {
        result = ( answer == 42 ); // Answer from ktalk has to be 42.
    }
    close (sock);
    unlink (tempAddr.sun_path);
    message("Announce to ktalk : result = %d",result);
    return result;
}
