
INCLUDES= -include $(srcdir)/../kplunger/kplunger.h $(all_includes)

bin_PROGRAMS = 	ksticker
noinst_LIBRARIES = libksticker.a

# Which sources should be compiled for khexdit.
ksticker_SOURCES = main.cpp kspainter.cpp

libksticker_a_SOURCES = ksticker.cpp speeddialog.cpp speeddialogData.cpp

# the library search path
ksticker_LDFLAGS =  $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
ksticker_LDADD   = ../kplunger/libkplunger.a ./libksticker.a $(LIB_KDEUI)     

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = ksticker.h main.h speeddialog.h \
	speeddialogData.h kspainter.h

METASOURCES = ksticker.moc main.moc speeddialog.moc speeddialogData.moc

