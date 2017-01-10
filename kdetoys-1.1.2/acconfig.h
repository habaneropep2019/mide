/* Define if the C++ compiler supports BOOL */
#undef HAVE_BOOL

#undef VERSION 

#undef PACKAGE

/* defines which to take for ksize_t */
#undef ksize_t

/* defines whether to use osockaddr or sockaddr */
#undef osockaddr

/* define if you have setenv */
#undef HAVE_FUNC_SETENV

/* define to 1 if /proc exists & you want to use one of the two methods below */
#undef PROC_FIND_USER

/* define to 1 under linux, using utmp (=>not working alone with xdm and kdm) */
#undef UTMP_AND_PROC_FIND_USER 

/* define also this one under linux to find for X processes even without utmp */
#undef ALL_PROCESSES_AND_PROC_FIND_USER

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Define if you have getdomainname */
#undef HAVE_GETDOMAINNAME

/* Define if you have gethostname */
#undef HAVE_GETHOSTNAME

/* Define if you have dl*() */
#undef HAVE_DLFCN

/* Define if you have shload() */
#undef HAVE_SHLOAD

/* Defines if you have OpenGL installed */
#undef HAVE_GL

/* Defines if you have libjpeg installed */
#undef HAVE_LIBJPEG


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
