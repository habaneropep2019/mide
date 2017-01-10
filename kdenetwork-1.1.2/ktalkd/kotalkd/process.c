/** kotalkd process.c **/

#include "includ.h"
#include <sys/param.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include <utmp.h>
#include <pwd.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#include "print.h"

extern int debug_mode;

/* Local inet address */
static struct  in_addr my_machine_addr;
/* Control (daemon) address. */
static struct sockaddr_in ctl_addr;
/* Port number of the talk daemon */
static int daemon_port;
/* NEW_CTL_MSG used to talk to our ntalk daemon */
static NEW_CTL_MSG new_msg;
/* Ctl socket */
static int ctl_sockt;

/* Opens a connection to the local ntalk daemon */
void open_local_socket(char * hostname)
{
    struct hostent *hp;
    ksize_t length;
    
    /* find the server's port */
    struct servent * sp = getservbyname("ntalk", "udp");
    if (sp == 0) {
        syslog(LOG_ERR, "talk: ntalk/udp: service is not registered.");
        exit(-1);
    }
    daemon_port = sp->s_port; /* already in network byte order */

    /* look up the address of the local host */
    hp = gethostbyname(hostname);
    if (!hp) {
        syslog(LOG_ERR, "GetHostByName failed.");
        exit(-1);
    }
    memcpy(&my_machine_addr, hp->h_addr, hp->h_length);

    /* open the ctl socket */
    ctl_addr.sin_family = AF_INET;
    ctl_addr.sin_addr = my_machine_addr;
    ctl_addr.sin_port = 0;
    ctl_sockt = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctl_sockt <= 0) {
        syslog(LOG_ERR, "Bad socket");
        exit(-1);
    }
    if (bind(ctl_sockt,(struct sockaddr *)&ctl_addr, sizeof(ctl_addr)) != 0) {
        syslog(LOG_ERR, "Couldn't bind to control socket");
        exit(-1);
    }
    length = sizeof(ctl_addr);
    if (getsockname(ctl_sockt, (struct sockaddr *)&ctl_addr, &length) == -1) {
        syslog(LOG_ERR, "Bad address for control socket");
        exit(-1);        
    }

    /* Fill the msg structure (ntalk protocol) */
    new_msg.vers = 0; /* TALK_VERSION for ntalk is 1. Let's assign 0 to otalk. */
}

void close_local_socket()
{
    close(ctl_sockt);
}

#define CTL_WAIT 2	/* time to wait for a response, in seconds */

/** This one is a very simple ctl_transact : we don't wait for an answer from ktalkd.
    If ktalkd didn't get our message, the otalk client will send it again...
    
    We can't send the response ourselves (ctl_addr has to remain the caller's)
    */
void ctl_transact(int type)
{
    int cc;
    struct sockaddr_in daemon_addr;

    daemon_addr.sin_family = AF_INET;
    daemon_addr.sin_addr = my_machine_addr;
    daemon_addr.sin_port = daemon_port;

    cc = sendto(ctl_sockt, (char *)&new_msg, sizeof(new_msg), 0,
                (struct sockaddr *)&daemon_addr,
                sizeof (daemon_addr));
    if (cc != sizeof(new_msg)) {
        if (errno == EINTR)
            return;
        syslog(LOG_ERR,"Error on write to talk daemon");
    }
}

void  process_request(register OLD_CTL_MSG *mp, register OLD_CTL_RESPONSE *rp)
{
	if (debug_mode)
            print_old_request("process_request", mp);

        /* Process mp->ctl_addr (used to respond) */
        mp->ctl_addr.sa_family = ntohs(mp->ctl_addr.sa_family);
        if (mp->ctl_addr.sa_family != AF_INET) {
                syslog(LOG_WARNING, "Bad control address, family %d",
                    mp->ctl_addr.sa_family);
                rp->answer = BADCTLADDR;
                return ;
        }
        
        /* Cut out names (to prevent problems). Max length = 8 + 1 ('\0') */
        mp->l_name[OLD_NAME_SIZE-1] = '\0';
        mp->r_name[OLD_NAME_SIZE-1] = '\0';
        mp->r_tty[TTY_SIZE-1] = '\0';

        /* Now a little trick.
           If user is ktalk then this a ktalk protocol-detection packet.
           Just exit, so that otalk won't be taken into account by ktalk.
        */
        if (strcmp(mp->r_name,"ktalk")==0) {
            message("ktalk detection protocol. Don't answer.");
            close_local_socket();
            exit(0);
        }
            
        /* Create a NEW_CTL_MSG structure from the OLD_CTL_MSG */
        new_msg.type = mp->type;
        new_msg.id_num = mp->id_num;
        new_msg.addr = mp->addr;
        new_msg.ctl_addr = mp->ctl_addr;
        new_msg.ctl_addr.sa_family = htons(mp->ctl_addr.sa_family);
        new_msg.pid = mp->pid;
        strcpy(new_msg.l_name, mp->l_name); /* cut before, no need to strncpy. */
        strcpy(new_msg.r_name, mp->r_name);
        strcpy(new_msg.r_tty, mp->r_tty);
        
        /* Now send request to local ktalkd. It will respond to it. */
        ctl_transact(mp->type);
}
