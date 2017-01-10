/* Define if the C++ compiler supports BOOL */
#undef HAVE_BOOL

#undef VERSION 

#undef PACKAGE

/* defines if having libgif (always 1) */
#undef HAVE_LIBGIF

/* defines if having libjpeg (always 1) */
#undef HAVE_LIBJPEG

/* defines which to take for ksize_t */
#undef ksize_t

/* define if you have setenv */
#undef HAVE_FUNC_SETENV

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Whatever ALSA is, ... */
#undef ALSA


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
