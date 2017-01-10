/*	This is only a little Header-file, which helps to remove 
	many warnings of giflib */

#ifndef _GIF_COMPAT_H_
#define _GIF_COMPAT_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#define REMOVE_VERSIONSTRING_WARNING  VersionStr=VersionStr

#endif /* _GIF_COMPAT_H_ */
