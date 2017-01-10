/*
 * Warning, this file was automatically created by the TIFF configure script
 * VERSION:	 v3.4beta024
 * DATE:	 Fri Jun 6 13:02:19 EDT 1997
 * TARGET:	 i586-unknown-linux
 * CCOMPILER:	 /usr/bin/gcc-2.7.2
 */
#ifndef _PORT_
#define _PORT_ 1
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define HOST_FILLORDER FILLORDER_LSB2MSB
#define HOST_BIGENDIAN	0
#define HAVE_MMAP 1
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
typedef double dblparam_t;
#ifdef __STRICT_ANSI__
#define	INLINE	__inline__
#else
#ifdef __SUNPRO_C
#define INLINE
#else
#define	INLINE	inline
#endif
#endif
#define GLOBALDATA(TYPE,NAME)	extern TYPE NAME
#ifdef __cplusplus
}
#endif
#endif
