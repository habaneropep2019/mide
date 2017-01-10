/*
 * This is where we drop in the various dependencies for different systems.
 * Someday this might be remotely complete.
 *
 * I kept the name pmachine.h because it was already in all of the files...this
 * barely resembles the pmachine.h that comes with the real Prospero, though.
 *
 * gf 23 Nov 1992 : Added section for defining malloc() & co.
 */

#ifdef u3b2
# define USG
# define NOREGEX
#endif

#ifdef hpux
# ifndef bcopy
#  define FUNCS			/* HP/UX 8.0 has the fns.  */
# endif
# define NOREGEX
# define NEED_STRING_H
#endif

/* These are required for a Sequent running Dynix/PTX, their SysV variant.
   Archie builds fine untouched on a system running their BSD-based OS.  */
#ifdef _SEQUENT_
# define NOREGEX
# define USG
#endif

#if defined(USG) || defined(SYSV)
# define FUNCS
#endif

#ifdef SOLARIS2
#define FUNCS
#define NOREGEX
#define NEED_STRING_H
#endif

#ifdef ISC
# define FUNCS
# define STRSPN
# define NOREGEX
#endif

#ifdef PCNFS
# define FUNCS
# define NEED_STRING_H
#ifndef MSDOS
# define MSDOS
#endif
#endif

#ifdef CUTCP
# define FUNCS
# define NOREGEX
# define NEED_STRING_H
# define SELECTARG int
# ifndef MSDOS
#  define MSDOS
# endif
#endif

#ifdef _AUX_SOURCE
# define AUX
# define NOREGEX
# define NBBY 8	/* Number of bits in a byte.  */
typedef long Fd_mask;
# define NFDBITS (sizeof(Fd_mask) * NBBY)	/* bits per mask */
#endif

#ifdef OS2
# define NOREGEX
# include <pctcp.h>
#endif
#ifdef MSDOS
# define USG
# define NOREGEX
# include <string.h>
# include <stdlib.h>
#endif

#ifdef _AIX
# ifdef u370
#  define FUNCS
# endif /* AIX/370 */
# define _NONSTD_TYPES
# define _BSD_INCLUDES
# define NEED_STRING_H
# define NEED_SELECT_H
# define NEED_TIME_H
#endif

/* General problems.  */

#ifdef FUNCS
# define index		strchr
/* According to mycroft@gnu.ai.mit.edu. */
# ifdef _IBMR2
char *strchr();
# endif
# define rindex		strrchr
# ifndef _AUX_SOURCE
#  define bcopy(a,b,n)	memcpy(b,a,n)
#  define bzero(a,n)	memset(a,0,n)
# ifdef _IBMR2
char *memset();
# endif
# endif
#endif

#if defined(_IBMR2) || defined(_BULL_SOURCE)
# define NEED_SELECT_H
#endif
#if defined(USG) || defined(UTS)
# define NEED_STRING_H
#endif
#if defined(USG) || defined(UTS) || defined(_AUX_SOURCE)
# define NEED_TIME_H
# ifdef UTS
#  define WANT_BOTH_TIME
# endif
#endif

#ifdef VMS
/* Get the system status stuff.  */
# include <ssdef.h>
#endif /* VMS */

/*
 * FD_SET: lib/pfs/dirsend.c, user/vget/ftp.c
 */
#ifndef CUTCP

#define SELECTARG fd_set
#if !defined(FD_SET) && !defined(VMS) && !defined(NEED_SELECT_H)
#define	FD_SETSIZE	32
#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#undef FD_ZERO
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif

#endif /* not CUTCP */
