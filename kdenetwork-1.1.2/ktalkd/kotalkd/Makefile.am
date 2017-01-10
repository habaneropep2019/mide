## -*- makefile -*-
# Kotalkd - Makefile.am

bin_PROGRAMS	=	kotalkd
INCLUDES	=	$(all_includes)
noinst_HEADERS	=	print.h process.h

kotalkd_SOURCES	=	kotalkd.c print.c process.c
LDADD           =       $(LIBBSD) $(LIBSOCKET)

#for extra warnings during compilation :
#AM_CXXFLAGS =	-ansi -pedantic -D_POSIX_SOURCE -D_BSD_SOURCE
