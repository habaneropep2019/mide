
lib_LTLIBRARIES = libuulib.la
#
# Programs to compile, Manpages to install and Versions
#
UUVERSION=	5.0
UUPATCH	 =	13
INCLUDES =	-DUUVERSION=\"$(UUVERSION)\" -DUUPATCH=\"$(UUPATCH)\"

libuulib_la_SOURCES =	uulib.c uucheck.c uunconc.c uuutil.c uuencode.c \
			uuscan.c uustring.c fptools.c

include_HEADERS= 	fptools.h uudeview.h uuint.h uustring.h

uuencode.o:	uuencode.c uudeview.h uuint.h uustring.h fptools.h config.h
uulib.o:	uulib.c uudeview.h uuint.h uustring.h fptools.h config.h
uunconc.o:	uunconc.c uudeview.h uuint.h uustring.h fptools.h config.h
uucheck.o:	uucheck.c uudeview.h uuint.h uustring.h fptools.h config.h
uuutil.o:	uuutil.c  uudeview.h uuint.h uustring.h fptools.h config.h
uuscan.o:	uuutil.c  uudeview.h uuint.h uustring.h fptools.h config.h
uustring.o:	uustring.c uudeview.h uuint.h uustring.h config.h
fptools.o:	fptools.c fptools.h config.h

uustring.h:	uustring.c
	awk -f uustring.awk < uustring.c > uustring.h

libuulib_la_LDFLAGS = -version-info 5:13

