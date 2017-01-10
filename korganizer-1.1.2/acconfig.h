/* Define if the C++ compiler supports BOOL */
#undef HAVE_BOOL

#undef VERSION 

#undef PACKAGE

/* defines if you have dlopen and co */
#undef HAVE_DYNAMIC_LOADING

/* defines if having libgif (always 1) */
#undef HAVE_LIBGIF

/* defines if having libjpeg (always 1) */
#undef HAVE_LIBJPEG

/* defines if having libpng (always 1) */
#undef HAVE_LIBPNG

/* defines which to take for ksize_t */
#undef ksize_t

/* define if you have setenv */
#undef HAVE_FUNC_SETENV

/* define if shadow under linux */
#undef HAVE_SHADOW

/* define if you have XPM support */
#undef HAVE_XPM

/* define if you have GL (Mesa, OpenGL, ...)*/
#undef HAVE_GL

/* define if you have PAM (Pluggable Authentication Modules); Redhat-Users! */
#undef HAVE_PAM

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* define if you have the PAM lib. Now, we have two different users, this will change */
#undef HAVE_PAM_LIB

/* define if you have shadow library */
#undef HAVE_SHADOW_LIB

/* define, where to find the X server */
#undef XBINDIR

/* define, where to find the XDM configurations */
#undef XDMDIR

/* Define if you have getdomainname */
#undef HAVE_GETDOMAINNAME  

/* Define if you have gethostname */
#undef HAVE_GETHOSTNAME  

/* Define if you have dlopen and co. */
#undef HAVE_DLFCN

/* Define if you have shl_load and co. */
#undef HAVE_SHLOAD

/* Define if you have usleep */
#undef HAVE_USLEEP

/* Define the file for utmp entries */
#undef UTMP

/* Define, if you want to use utmp entries */
#undef UTMP_SUPPORT

/* Define if you have an SGI like STL implementation */
#undef HAVE_SGI_STL
 
/* Define if you have an HP like STL implementation */
#undef HAVE_HP_STL                                                              

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

/* this is needed for Solaris and others */
#ifndef HAVE_GETDOMAINNAME
#define HAVE_GETDOMAINNAME
#ifdef __cplusplus  
extern "C" 
#endif
int getdomainname (char *Name, int Namelen);
#endif  

#ifndef HAVE_GETHOSTNAME
#define HAVE_GETHOSTNAME
#ifdef __cplusplus  
extern "C" 
#endif
int gethostname (char *Name, int Namelen);
#endif  

