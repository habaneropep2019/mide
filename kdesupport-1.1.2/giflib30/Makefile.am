
INCLUDES = $(X_INCLUDES) 
lib_LTLIBRARIES = libgif.la
libgif_la_LDFLAGS = $(X_LDFLAGS) -version-info 3:0
libgif_la_LIBADD = $(LIB_X11)
AM_CFLAGS = -D__X11__ 

include_HEADERS = getarg.h gif_hash.h gif_lib.h

libgif_la_SOURCES = dev2gif.c   egif_lib.c  gif_err.c   gif_hash.c \
	 qprintf.c dgif_lib.c  getarg.c    gif_font.c  gifalloc.c  quantize.c

