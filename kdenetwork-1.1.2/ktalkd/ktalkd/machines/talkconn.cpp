/*
 * Copyright (c) 1983 Regents of the University of California, (c) 1998 David Faure
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
 * This file handles haggling with the various talk daemons to
 * get a socket to talk to. sockt is opened and connected in
 * the progress
 */

#include "talkconn.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <syslog.h>
#include <netdb.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif
#else
#include <time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "../print.h"
#include "../defs.h" // for hostname

#ifndef SOMAXCONN
#warning SOMAXCONN not defined in your headers
#define SOMAXCONN 5
#endif

#define CTL_WAIT 2	/* time to wait for a response, in seconds */

struct in_addr TalkConnection::defaultReplyAddr;
short int TalkConnection::talkDaemonPort;          // Port number of talk demon  (517)
short int TalkConnection::ntalkDaemonPort;         // Port number of ntalk demon (518)


TalkConnection::TalkConnection(struct in_addr caller_machine_addr, 
                               char * r_name,
                               char * local_user,
                               ProtocolType _protocol) : 
   protocol(_protocol), his_machine_addr(caller_machine_addr), ctl_sockt(-1), sockt(-1)
{
    my_machine_addr = getReplyAddr(his_machine_addr);

    new_msg.vers = TALK_VERSION;
    new_msg.pid = htonl (getpid ()); // is it necessary ?
    *new_msg.r_tty = '\0';
    old_msg.pid = htonl (getpid ()); // is it necessary ?
    *old_msg.r_tty = '\0';

    strncpy(new_msg.l_name,local_user,NEW_NAME_SIZE);
    strncpy(new_msg.r_name,r_name,NEW_NAME_SIZE);
    strncpy(old_msg.l_name,local_user,OLD_NAME_SIZE);
    strncpy(old_msg.r_name,r_name,OLD_NAME_SIZE);

}

TalkConnection::~TalkConnection()
{
    close_sockets();
}

void TalkConnection::init()
{
    /* look up the address of the local host */
    struct hostent *hp = gethostbyname(Options.hostname);
    if (!hp) {
        syslog(LOG_ERR, "GetHostByName failed for %s.",Options.hostname);
        exit(-1);
    }
    memcpy(&defaultReplyAddr, hp->h_addr, hp->h_length);

    /* find the server's ports */
    struct servent * sp = getservbyname("talk", "udp");
    if (sp == 0)
        syslog(LOG_ERR, "talkconnection: talk/udp: service is not registered.\n");
    talkDaemonPort = sp->s_port; // already in network byte order

    sp = getservbyname("ntalk", "udp");
    if (sp == 0)
        syslog(LOG_ERR, "talkconnection: ntalk/udp: service is not registered.\n");
    ntalkDaemonPort = sp->s_port; // already in network byte order
}

int TalkConnection::open_socket (struct sockaddr_in *addr, int type)
{
    addr->sin_family = AF_INET;
    addr->sin_addr = my_machine_addr;
    addr->sin_port = 0;
    int newSocket = socket (PF_INET, type, 0);
    if (newSocket <= 0)
        p_error ("Unable to open a new socket!");

    ksize_t length = sizeof (*addr);
    if (bind (newSocket, (struct sockaddr *) addr, length) != 0) {
        ::close (newSocket);
        p_error ("Error binding socket!");
    }
    if (getsockname (newSocket, (struct sockaddr *) addr, &length) == -1) {
        ::close (newSocket);
        p_error ("New socket has a bad address!");
    }
    return newSocket;
}

void TalkConnection::open_sockets()
{
    struct sockaddr_in ctl_addr;
    struct sockaddr_in my_addr;

    /* open the ctl socket */
    ctl_sockt = open_socket(&ctl_addr, SOCK_DGRAM);
    /* store its address */
    set_ctl_addr((const struct sockaddr *)&ctl_addr);

    /* open the text socket */
    sockt = open_socket(&my_addr, SOCK_STREAM);
    /* store its address */
    set_addr((const struct sockaddr *)&my_addr);
}

/* Tries to find out the correct IP address that the daemon at host
  "destination" has to respond to - code borrowed from ktalk, thanks Burkhard ! */
struct in_addr TalkConnection::getReplyAddr (struct in_addr destination) {

  in_addr *result;
  unsigned char *help1;
  unsigned char *help2;

 /* disabled caching - I don't have QIntDict ... 
  result = replyAddrList [(long) destination.s_addr];
  if (result) {
    return *result;
  }
 */
  int testsock, i;
  result = new (struct in_addr);
  struct sockaddr_in client, daemon;
  for (i = 0; i < 2; i++) {
    client.sin_family = daemon.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl (INADDR_ANY);
    client.sin_port = htons (0);
    daemon.sin_addr = destination;
    daemon.sin_port = i ? ntalkDaemonPort : talkDaemonPort;
 
    // Connect to the daemon socket address
    // On some UNIXes (such as Linux) this works and sets the IP address queried
    // by getsockname to the local machine address used to reach the daemon.
    // If it doesn't work (e.g. on SunOS and Solaris), the default machine
    // address is used instead.
    ksize_t length = sizeof (daemon);
    if ((testsock = socket (AF_INET, SOCK_DGRAM, 0)) >= 0 &&
        bind (testsock, (struct sockaddr *) &client, sizeof (client)) == 0 &&
        ::connect (testsock, (struct sockaddr *) &daemon,
                             sizeof (daemon)) == 0 &&
        getsockname (testsock, (struct sockaddr *) &client, &length) != -1 &&
        client.sin_addr.s_addr != htonl (INADDR_ANY))
    {
      *result = client.sin_addr;
      message ("Found reply address");
      ::close (testsock);
      break;
    }
    if (testsock >= 0) ::close (testsock);
  }
  if (i == 2) {
    *result = defaultReplyAddr;
    message ("Couldn't find reply address, using default");
  }
  if (Options.debug_mode) {
      help1 = (unsigned char *) &destination;
      help2 = (unsigned char *) result;
      syslog ( LOG_DEBUG,
               "detected reply address for %d.%d.%d.%d: %d.%d.%d.%d",
               help1 [0], help1 [1], help1 [2], help1 [3],
               help2 [0], help2 [1], help2 [2], help2 [3]);
    /*   replyAddrList.insert ((long) destination.s_addr, result); disabled */
  }
  return *result;
}
/* QIntDict <in_addr> TalkConnection::replyAddrList; */

/** Check the remote protocol. Result stored in <protocol>.
 * @return 1 if succeeded to find at least 1 protocol */
void TalkConnection::findProtocol() {

    message("Remote protocol unknown. Trying to find it. findProtocol()");
    /* The existing ctl_sockt will be used for ntalk */
    int new_socket = ctl_sockt;
    /* We need a new SOCK_DGRAM socket for otalk */
    struct sockaddr_in old_ctl_addr;
    int old_socket = open_socket(&old_ctl_addr, SOCK_DGRAM);

    /* Fill the old_msg return-address to match the address of old_socket */
    old_msg.ctl_addr = *(struct sockaddr *)&old_ctl_addr;
    old_msg.ctl_addr.sa_family = htons(AF_INET);

    /* Prepare two LOOK_UP ctl messages */
    old_msg.type = LOOK_UP;
    old_msg.id_num = htonl(0L);
    new_msg.type = LOOK_UP;
    new_msg.id_num = htonl(0L);
    char svg_r_name[NEW_NAME_SIZE]; // Save the real r_name
    strcpy(svg_r_name, new_msg.r_name);
    strcpy(old_msg.r_name, "ktalk");
    strcpy(new_msg.r_name, "ktalk");

    struct sockaddr_in daemon;
    daemon.sin_family = AF_INET;
    daemon.sin_addr = his_machine_addr;

    /* Prepare the variables used for reading on sockets */
    fd_set read_mask, ctl_mask;
    int nready=0, cc;
    struct timeval wait;

    FD_ZERO(&ctl_mask);
    FD_SET(new_socket, &ctl_mask);
    FD_SET(old_socket, &ctl_mask);
    int max_socket = (new_socket > old_socket) ? new_socket : old_socket;

    /* Method : we send the two packets to the two remote daemons.
       We wait for the first one correct answer, and then we stop everything.
       If a wrong answer comes, ignore it (but note that we got it).
       If no answer (or two wrong answers), retry, up to 3 times. */

    for (int retry = 0; (retry < 3) && (protocol==noProtocol); retry ++)
    {
        message("Send packets. Retry = %d",retry);
        /* Send the messages */
        daemon.sin_port = ntalkDaemonPort;
        int len = sendto (new_socket, (char *) &new_msg, sizeof new_msg, 0,
                          (struct sockaddr *) &daemon, sizeof daemon);
        if (len != sizeof new_msg) 
            syslog(LOG_ERR, "findProtocol: sendto() for ntalk failed!");
    
        daemon.sin_port = talkDaemonPort;
        len = sendto (old_socket, (char *) &old_msg, sizeof old_msg, 0,
                      (struct sockaddr *) &daemon, sizeof daemon);
        if (len != sizeof old_msg)
            syslog(LOG_ERR, "findProtocol: sendto() for otalk failed!");

        do {
            /* Wait for response */
            read_mask = ctl_mask;
            wait.tv_sec = CTL_WAIT;
            wait.tv_usec = 0;
            nready = ::select(max_socket+1, &read_mask, 0, 0, &wait);
            if (nready < 0) {
                if (errno == EINTR)
                    continue;
                // Timeout. Let's exit this loop and retry sending.
                break;
            }
            if (nready == 0) message("select returned 0 ! ");

            /* Analyze response */
            if (FD_ISSET(old_socket, &read_mask)) {
                message("Reading on old_socket");
                cc = ::recv(old_socket, (char *)&old_resp, sizeof (old_resp), 0);
                if (cc < 0) {
                    if (errno == EINTR)
                        continue;
                    // Wrong answer (e.g. too short).
                } else
                    if (old_resp.type == LOOK_UP) protocol=talkProtocol; // FOUND
            }
            if (FD_ISSET(new_socket, &read_mask)) {
                message("Reading on new_socket");
                cc = ::recv(new_socket, (char *)&new_resp, sizeof (new_resp), 0);
                if (cc < 0) {
                    if (errno == EINTR)
                        continue;
                    // Wrong answer (e.g. too short). Note ntalk answered
                } else
                    if ((new_resp.type == LOOK_UP) && (new_resp.vers == TALK_VERSION))
                        protocol=ntalkProtocol;
            }
        } while (protocol==noProtocol);
        // wait for a time out, or ok.
    } // for

    /* restore the real r_name */
    strncpy(old_msg.r_name, svg_r_name, OLD_NAME_SIZE);
    strncpy(new_msg.r_name, svg_r_name, NEW_NAME_SIZE);
    /* restore old.ctl_addr */
    old_msg.ctl_addr = new_msg.ctl_addr;
    ::close(old_socket);
    message("Exiting findProtocol");
    if (protocol==ntalkProtocol) message("Exiting findProtocol with protocol = NTALK");
    else if (protocol==talkProtocol) message("Exiting findProtocol with protocol = NTALK");
    else p_error("FATAL : no protocol found.");
}

void TalkConnection::set_addr(const struct sockaddr * addr)
{
    old_msg.addr = *addr;
    old_msg.addr.sa_family = htons(AF_INET);
    new_msg.addr = *addr;
    new_msg.addr.sa_family = htons(AF_INET);
}

void TalkConnection::set_ctl_addr(const struct sockaddr * ctl_addr)
{
    old_msg.ctl_addr = *ctl_addr;
    old_msg.ctl_addr.sa_family = htons(AF_INET);
    new_msg.ctl_addr = *ctl_addr;
    new_msg.ctl_addr.sa_family = htons(AF_INET);
}

void TalkConnection::close_sockets()
{
    if (sockt!=-1) { close(sockt); sockt = -1; }
    if (ctl_sockt!=-1) { close(ctl_sockt); ctl_sockt = -1; }
}

/*
 * SOCKDGRAM is unreliable, so we must repeat messages if we have
 * not received an acknowledgement within a reasonable amount
 * of time
 */
void TalkConnection::ctl_transact(int type, int id_num)
{

    if (protocol == noProtocol)
        /** We've been so far, but we still don't know which protocol to use.
         * Let's check it, the way ktalk does. */
        findProtocol();

    fd_set read_mask, ctl_mask;
    int nready=0, cc, size, ok=0;
    struct timeval wait;
    struct sockaddr_in daemon_addr;
    char * msg;

    if (protocol == talkProtocol) {
        old_msg.type = type;
        old_msg.id_num = htonl(id_num);
        msg = (char *)&old_msg;
        size = sizeof old_msg;
    } else {
        new_msg.type = type;
        new_msg.id_num = htonl(id_num);
        msg = (char *)&new_msg;
        size = sizeof new_msg;
        print_request("ctl_transact: ",&new_msg);
    }

    daemon_addr.sin_family = AF_INET;
    daemon_addr.sin_addr = his_machine_addr;
    daemon_addr.sin_port = (protocol == talkProtocol) ? talkDaemonPort : ntalkDaemonPort;
    FD_ZERO(&ctl_mask);
    FD_SET(ctl_sockt, &ctl_mask);

    /* Keep sending the message until a response of
     * the proper type is obtained.
     */
    do {
        /* resend message until a response is obtained */
        do {
            cc = sendto(ctl_sockt, msg, size, 0,
                        (struct sockaddr *)&daemon_addr,
                        sizeof (daemon_addr));
            if (cc != size) {
                if (errno == EINTR)
                    continue;
                p_error("Error on write to talk daemon");
            }
            read_mask = ctl_mask;
            wait.tv_sec = CTL_WAIT;
            wait.tv_usec = 0;
            nready = ::select(ctl_sockt+1, &read_mask, 0, 0, &wait);
            if (nready < 0) {
                if (errno == EINTR)
                    continue;
                p_error("Error waiting for daemon response");
            }
            if (nready == 0) message("select returned 0 ! ");
        } while (nready == 0);
        /*
         * Keep reading while there are queued messages 
         * (this is not necessary, it just saves extra
         * request/acknowledgements being sent)
         */
        do {
            if (protocol == talkProtocol)
                cc = ::recv(ctl_sockt, (char *)&old_resp, sizeof (old_resp), 0);
            else
                cc = ::recv(ctl_sockt, (char *)&new_resp, sizeof (new_resp), 0);
            if (cc < 0) {
                if (errno == EINTR)
                    continue;
                p_error("Error on read from talk daemon");
            }
            read_mask = ctl_mask;
            /* an immediate poll */
            timerclear(&wait);
            nready = ::select(ctl_sockt+1, &read_mask, 0, 0, &wait);
            if (protocol == talkProtocol) ok = (old_resp.type == type);
            else ok = ((new_resp.type == type) && (new_resp.vers == TALK_VERSION));
        } while (nready > 0 && (!ok));
    } while (!ok);
    if (protocol == talkProtocol) {
        old_resp.id_num = ntohl(old_resp.id_num);
        old_resp.addr.sa_family = ntohs(old_resp.addr.sa_family);
    } else {
        new_resp.id_num = ntohl(new_resp.id_num);
        new_resp.addr.sa_family = ntohs(new_resp.addr.sa_family);
    }
}

/** Look for an invitation on remote machine */
int TalkConnection::look_for_invite(int mandatory)
{
    /* Check for invitation on caller's machine */
    ctl_transact(LOOK_UP, 0);

    char answer;
    int id_num;
    getResponseItems(&answer, &id_num, &lookup_addr);

    if (!mandatory) return 0;

    /* the switch is for later options, such as multiple invitations */
    switch (answer) {

	case SUCCESS:
            new_msg.id_num = htonl(id_num);
            old_msg.id_num = htonl(id_num);
            message("TalkConnection::look_for_invite : got SUCCESS");
            if (lookup_addr.sa_family != AF_INET)
                p_error("Response uses invalid network address");
            return (1);

	default:
            /* there wasn't an invitation waiting for us */
            message("TalkConnection::look_for_invite : didn't get SUCCESS");
            return (0);
    }
}

/** Prepare to accept a connection from another talk client */
void TalkConnection::listen()
{
    if (::listen(sockt, SOMAXCONN) != 0)
        p_error("Error on attempt to listen for caller");
}

/** Accept a connection from another talk client */
int TalkConnection::accept()
{
    int accept_sockt;
    while ((accept_sockt = ::accept(sockt, 0, 0)) < 0) {
        if (errno == EINTR)
            continue;
        p_error("Unable to connect with your party");
    }
    ::close(sockt);
    sockt = accept_sockt;
    return sockt;
}

/** Connect to another talk client. */
int TalkConnection::connect()
{
    message("Waiting to connect");
    do {
        errno = 0;
        if (::connect(sockt, &lookup_addr, sizeof (struct sockaddr)) != -1)
            return 1;
    } while (errno == EINTR);
    if (errno == ECONNREFUSED) {
        /*
         * The caller gave up, but his invitation somehow
         * was not cleared. Clear it and initiate an 
         * invitation. (We know there are no newer invitations,
         * the talkd works LIFO.)
         */
        message("ECONNREFUSED");
        ctl_transact(DELETE, 0);
        close_sockets();
        return 0;
    }
    p_error("Unable to connect with initiator");
    /*NOTREACHED*/
    return 0;
}

/** Trade edit characters with the other talk. By agreement
 * the first three characters each talk transmits after
 * connection are the three edit characters. 
 * A normal talk client uses tcgetattr() to get the chars,
 * but the daemon isn't connected to a terminal, so we can't call it.
 * We just send dummy chars, to disable control chars. */
void TalkConnection::set_edit_chars()
{
	char buf[3];
        int cc;
        buf[0] = buf[1] = buf[2] = (char)0xff;
        /* Write our config to the caller */
	cc = write(sockt, buf, sizeof(buf));
	if (cc != sizeof(buf) )
		p_error("Lost the connection");
        /* Read the caller configuration */
	cc = read(sockt, buf, sizeof(buf));
	if (cc != sizeof(buf) )
		p_error("Lost the connection");
	char_erase = buf[0]; // store it in TalkConnection
}

void TalkConnection::write_banner(char * banner)
{ /* writes the message 'banner', null-terminated */
    int count = strlen(banner);
    int nbsent;
    char * str = banner;
    /*    message_d("Count : %d.",count); */
    while (count>0) {
        /* let's send 16 -bytes-max packets */
         if (count>=16) nbsent = write(sockt,str,16);
                  else nbsent = write(sockt,str,count);
        count -= nbsent;
        str += nbsent;
        fsync(sockt);
    }
    write(sockt,"\n",1);
}

void TalkConnection::getResponseItems(char * answer, int * id_num, struct sockaddr * addr) {
    if (protocol == talkProtocol) {
        if (answer) *answer = old_resp.answer;
        if (id_num) *id_num = old_resp.id_num;
        if (addr) *addr = old_resp.addr;
    } else {
        if (answer) *answer = new_resp.answer;
        if (id_num) *id_num = new_resp.id_num;
        if (addr) *addr = new_resp.addr;
    }
}

/** p_error prints the system error message in the log
 * and then exits. */
void TalkConnection::p_error(const char *str)
{
    syslog(LOG_ERR,str);
    _exit(0);
}
