/*
 * stringdefs.h : Include defs of string functions
 *
 * George Ferguson, ferguson@cs.rochester.edu, 23 Apr 1993.
 *
 * This is based on the example from the Autoconf manual.
 */

#include "config.h"

#if STDC_HEADERS || HAVE_STRING_H
# include <string.h>
/* An ANSI string.h and pre-ANSI memory.h might conflict.  */
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif /* not STDC_HEADERS and HAVE_MEMORY_H */
# ifndef index
#  define index strchr
# endif
# ifndef rindex
#  define rindex strrchr
# endif
# ifndef bcopy
#  define bcopy(s, d, n) memcpy ((d), (s), (n))
#  define bcmp(s1, s2, n) memcmp ((s1), (s2), (n))
#  define bzero(s, n) memset ((s), 0, (n))
# endif
#else /* not STDC_HEADERS and not HAVE_STRING_H */
# include <strings.h>
/* memory.h and strings.h conflict on some systems.  */
#endif /* not STDC_HEADERS and not HAVE_STRING_H */

#ifndef HAVE_STRCASECMP
#if NeedFunctionPrototypes || defined(FUNCPROTO)
#include <sys/types.h>
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
#else
extern int strcasecmp(),strncasecmp();
#endif
#endif
