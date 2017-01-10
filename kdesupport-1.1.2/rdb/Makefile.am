
lib_LTLIBRARIES = librdb.la
librdb_la_LDFLAGS = -version-info 55:0:55

librdb_la_SOURCES = cdb_hash.c cdb_seek.c  hash.c rdb.c

bin_PROGRAMS = cdbmake


rdb_HEADERS = cdb.h cdbmake.h hash.h rdb.h uint32.h 
rdbdir = $(includedir)/rdb

cdbmake_SOURCES = cdbmake.c cdbmake_add.c cdbmake_hash.c cdbmake_pack.c
