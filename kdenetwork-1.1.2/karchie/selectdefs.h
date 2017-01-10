/*
 * selectdefs.h : FD_SET and relatives
 *
 * George Ferguson, ferguson@cs.rochester.edu, 23 Apr 1993.
 *
 * I don't really expect this to work if none of the include cases
 * is used. I mean, where's the definition of the fd_set structure,
 * anyway? However, perhaps it's a start. Better would be to add a case
 * to the confgure script to find the things no matter where they are.
 *
 * 13 May 1993: Fail if none of the macros defined.
 */

#include "config.h"

#include <stdio.h>			/* Some folks need this also */

#ifdef KARCHIE
#ifdef HAVE_SYS_TYPES_H
#define FD_SET_IN_SYS_TYPES_H
#else
#ifdef HAVE_SYS_SELECT_H
#define FD_SET_IN_SYS_SELECT_H
#else
#ifdef HAVE_SYS_INET_H
#define FD_SET_IN_SYS_INET_H
#endif /* HAVE_SYS_INET_H */
#endif /* HAVE_SYS_SELECT_H */
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#endif /* KARCHIE */

#ifdef FD_SET_IN_SYS_TYPES_H			/* normal */
# include <sys/types.h>
#else
#ifdef FD_SET_IN_SYS_SELECT_H
# include <sys/select.h>			/* _AIX */
#else
#ifdef FD_SET_IN_SYS_INET_H
# include <sys/inet.h>				/* u3b2 */
#else
#error "One of the FD_SET_IN_* macros must be defined in config.h!";
#endif
#endif
#endif

#ifndef NBBY
# define NBBY		8				/* bits per byte */
#endif
#ifndef NFDBITS
# define NFDBITS	(sizeof (fd_mask) * NBBY)       /* bits per mask */
#endif
#ifndef FD_SETSIZE
# define FD_SETSIZE	32
#endif
#ifndef FD_SET
# define FD_SET(n,p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#endif
#ifndef FD_CLR
# define FD_CLR(n,p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#endif
#ifndef FD_ISSET
# define FD_ISSET(n,p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#endif
#ifndef FD_ZERO
# define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif
