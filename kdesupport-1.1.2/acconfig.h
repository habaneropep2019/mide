/* Define if the C++ compiler supports BOOL */
#undef HAVE_BOOL

#undef VERSION
#undef PACKAGE

/*
 * If your system has stdin/stdout/stderr
 */
#undef HAVE_STDIO


/*
 * Replacement functions.
 * #define strerror _FP_strerror
 * #define tempnam  _FP_tempnam
 * if you don't have these functions
 */
#undef strerror
#undef tempnam

/*
 * define if your compiler supports function prototypes
 */
#undef PROTOTYPES

/* Define if you have gethostname */
#undef HAVE_GETHOSTNAME

/* this is just to make autoheader silent. It's not used */
#undef ksize_t

/**
 * The below is taken out of the js distribution by
 * Markku Rossi <mtr@ngs.fi> 
 */

/* Canonical host name and its parts. */
#undef CANONICAL_HOST
#undef CANONICAL_HOST_CPU
#undef CANONICAL_HOST_VENDOR
#undef CANONICAL_HOST_OS

/* Do we want to build all instruction dispatchers? */
#undef ALL_DISPATCHERS

/* Do we want to profile byte-code operands. */
#undef PROFILING

/* Does the struct stat has st_blocks member? */
#undef HAVE_STAT_ST_ST_BLOCKS  

/*
 * Posix threads features.
 */    

/* Does the asctime_r() function take three arguments. */
#undef ASCTIME_R_WITH_THREE_ARGS

/* Does the drand48_r() work with DRAND48D data. */
#undef DRAND48_R_WITH_DRAND48D


/*
 * Extensions.
 */

/* JS */
#undef WITH_JS

/* Curses. */
#undef WITH_CURSES
#undef HAVE_CURSES_H
#undef HAVE_NCURSES_H

/* MD5 */
#undef WITH_MD5  


#ifndef HAVE_BOOL
#define HAVE_BOOL
typedef int bool;
#ifdef __cplusplus
const bool false = 0;
const bool true = 1;
#else
#define false (bool)0;
#define true (bool)1;
#endif
#endif

#ifndef HAVE_GETHOSTNAME
#define HAVE_GETHOSTNAME
#ifdef __cplusplus
extern "C"
#endif
int gethostname (char *Name, int Namelen);
#endif

