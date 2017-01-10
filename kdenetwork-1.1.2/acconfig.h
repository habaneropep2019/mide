/* Define if the C++ compiler supports BOOL */
#undef HAVE_BOOL

#undef VERSION 

#undef PACKAGE

/* defines if having libgif (always 1) */
#undef HAVE_LIBGIF

/* defines if having libjpeg (always 1) */
#undef HAVE_LIBJPEG

/* defines if having libtiff */
#undef HAVE_LIBTIFF

/* defines if having libpng */
#undef HAVE_LIBPNG

/* defines which to take for ksize_t */
#undef ksize_t

/* define if you have setenv */
#undef HAVE_FUNC_SETENV

/* define to 1 if /proc exists & you want to use one of the two methods below */
#undef PROC_FIND_USER

/* define to 1 under linux, using utmp (=>not working alone with xdm and kdm) */
#undef UTMP_AND_PROC_FIND_USER 

/* Define the file for utmp entries */
#undef UTMP 

/* define also this one under linux to find for X processes even without utmp */
#undef ALL_PROCESSES_AND_PROC_FIND_USER

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Define if you have getdomainname */
#undef HAVE_GETDOMAINNAME

/* Define if you have gethostname */
#undef HAVE_GETHOSTNAME

/* Define if you have usleep */
#undef HAVE_USLEEP

/* Define if you have dl*() */
#undef HAVE_DLFCN

/* Define if you have shload() */
#undef HAVE_SHLOAD

/* Define if you have the <dl.h> header file.  */
#undef HAVE_DL_H

/* Define if you have the <dlfcn.h> header file.  */
#undef HAVE_DLFCN_H
 
#undef KDEMAXPATHLEN


#define HAVE_MEDIATOOL 1

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

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD)
#       undef __BSD__
#       define __BSD__ 1 /* needed so that #elif __BSD__ will work with gcc */
#endif

#ifndef HAVE_FUNC_SETENV
#define HAVE_FUNC_SETENV
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);
#endif

