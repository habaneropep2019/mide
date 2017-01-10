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

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Define if you have an SGI like STL implementation */
#undef HAVE_SGI_STL

/* Define if you have an HP like STL implementation */
#undef HAVE_HP_STL

#undef HAVE_FUNC_ISINF

#define HAVE_MEDIATOOL 1

#undef KDEMAXPATHLEN

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

/*
 * jpeg.h needs HAVE_BOOLEAN, when the system uses boolean in system
 * headers and I'm too lazy to write a configure test as long as only
 * unixware is related
 */
#ifdef _UNIXWARE
#define HAVE_BOOLEAN
#endif

