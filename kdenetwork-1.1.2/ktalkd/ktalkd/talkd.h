#ifndef _TALKD_H
#define _TALKD_H

/* talkd.h 
 *
 * defines structures and constants for communication with talk and ntalk
 * daemons
 *
 * This header file was taken from ytalk version 3.2, file socket.h
 * and merged with the header file protocol/talkd.h version 8.1 (Berkeley)
 * Original author is Britt Yenne
 * Thanks very much for "lending" this code :-)
 */

/* ---- talk daemon I/O structures ---- */

#define OLD_NAME_SIZE 9
#define NEW_NAME_SIZE 12
#define TTY_SIZE 16

/* Control Message structure for old talk protocol (earlier than BSD4.2) */

typedef struct {
  char     type;                   /* request type, see below */
  char     l_name [OLD_NAME_SIZE]; /* caller's name */
  char     r_name [OLD_NAME_SIZE]; /* callee's name */
  char     pad;
  int      id_num;                 /* message id */
  int      pid;                    /* caller's process id */
  char     r_tty [TTY_SIZE];       /* callee's tty name */
  struct sockaddr addr;            /* socket address for connection */
  struct sockaddr ctl_addr;        /* control socket address */
} OLD_CTL_MSG;

/* Control Response structure for old talk protocol (earlier than BSD4.2) */

typedef struct {
  char      type;         /* type of request message, see below */
  char      answer;       /* response to request message, see below */
  char      pad [2];
  int       id_num;       /* message id */
  struct sockaddr addr;   /* address for establishing conversation */ 
} OLD_CTL_RESPONSE;

/* Control Message structure for new talk protocol (BSD4.2 and later) */

typedef struct {
  char      vers;                  /* protocol version */
  char      type;                  /* request type, see below */
  char      pad [2];
  int       id_num;                /* message id */
  struct sockaddr addr;            /* socket address for connection */
  struct sockaddr ctl_addr;        /* control socket address */
  int       pid;                   /* caller's process id */
  char	    l_name[NEW_NAME_SIZE]; /* caller's name */
  char	    r_name[NEW_NAME_SIZE]; /* callee's name */
  char	    r_tty[TTY_SIZE];       /* callee's tty name */
} NEW_CTL_MSG;

/* Control Response structure for new talk protocol (BSD4.2 and later) */

typedef struct {
  char     vers;         /* protocol version */
  char     type;         /* type of request message, see below */
  char     answer;       /* respose to request message, see below */
  char     pad;
  int      id_num;       /* message id */
  struct sockaddr addr;  /* address for establishing conversation */
} NEW_CTL_RESPONSE;

#define	TALK_VERSION	1		/* protocol version */

/* Dgram Types.
 *
 * These are the "type" values for xxx_CLT_MSG and xxx_CLT_RESPONSE.  Each acts
 * either on the remote daemon or the local daemon, as marked.
 */

#define LEAVE_INVITE	0	/* leave an invitation (local) */
#define LOOK_UP		1	/* look up an invitation (remote) */
#define DELETE		2	/* delete erroneous invitation (remote) */
#define ANNOUNCE	3	/* ring a user (remote) */
#define DELETE_INVITE	4	/* delete my invitation (local) */
#define AUTO_LOOK_UP	5	/* look up auto-invitation (remote) */
#define AUTO_DELETE	6	/* delete erroneous auto-invitation (remote) */

/* answer values.
 *
 * These are the values that are returned in "answer" of xxx_CTL_RESPONSE.
 */

#define SUCCESS         0       /* operation completed properly */
#define NOT_HERE        1       /* callee not logged in */
#define FAILED          2       /* operation failed for unexplained reason */
#define MACHINE_UNKNOWN 3       /* caller's machine name unknown */
#define PERMISSION_DENIED 4     /* callee's tty doesn't permit announce */
#define UNKNOWN_REQUEST 5       /* request has invalid type value */
#define BADVERSION      6       /* request has invalid protocol version */
#define BADADDR         7       /* request has invalid addr value */
#define BADCTLADDR      8       /* request has invalid ctl_addr value */

/* Operational parameters. */

#define MAX_LIFE        60      /* max time daemon saves invitations */
#define RING_WAIT       30      /* time to wait before resending invitation */
/* RING_WAIT should be 10's of seconds less than MAX_LIFE */

#endif
