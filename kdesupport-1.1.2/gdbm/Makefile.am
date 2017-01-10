
lib_LTLIBRARIES = libgdbm.la

libgdbm_la_SOURCES = dbminit.c delete.c fetch.c store.c seq.c close.c \
	dbmopen.c dbmdelete.c dbmfetch.c dbmstore.c dbmseq.c \
	dbmclose.c dbmdirfno.c dbmpagfno.c dbmrdonly.c \
	gdbmopen.c gdbmdelete.c gdbmfetch.c  gdbmstore.c gdbmclose.c \
	gdbmreorg.c gdbmseq.c gdbmsync.c gdbmerrno.c gdbmexists.c gdbmsetopt.c \
	bucket.c falloc.c findkey.c global.c hash.c update.c \
	version.c $(ALLOCA)

noinst_HEADERS = gdbmdefs.h extern.h gdbmerrno.h systems.h \
	gdbmconst.h proto.h
libgdbm_la_LDFLAGS =  -version-info 8:3:7

include_HEADERS =  gdbm.h dbm.h ndbm.h gdbm.proto gdbm.proto2

gdbm.h:	gdbm.proto gdbmerrno.h gdbm.proto2
	rm -f gdbm.h
	cp $(srcdir)/gdbm.proto gdbm.h
	chmod +w gdbm.h
	grep _ $(srcdir)/gdbmerrno.h >> gdbm.h
	cat $(srcdir)/gdbm.proto2 >> gdbm.h
	chmod -w gdbm.h



