#	$Id: Makefile.am,v 1.9 1999/01/20 10:16:12 kulow Exp $	

noinst_LIBRARIES = libversit.a

libversit_a_SOURCES = vcc.c vobject.c

$(srcdir)/vcc.c: vcc.y
	bison $(srcdir)/vcc.y -o $(srcdir)/vcc.c
