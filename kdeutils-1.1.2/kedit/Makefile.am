#
#                           KEdit Makefile.am
#
#  Copyright 1997 Bernd Johannes Wuebben, wuebben@math.cornell.edu
#
#

INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = -lkspell $(LIB_KFILE) 

bin_PROGRAMS = kedit

kedit_SOURCES =	kedit.cpp  keditcl1.cpp keditcl2.cpp print.cpp\
		filldlg.cpp mail.cpp  urldlg.cpp
noinst_HEADERS = kedit.h  keditcl.h  print.h\
		filldlg.h  version.h mail.h urldlg.h
METASOURCES =	kedit.moc  keditcl.moc  print.moc\
		filldlg.moc mail.moc  urldlg.moc

SUBDIRS = docs pixmaps

apps_DATA = KEdit.kdelnk
appsdir = $(kde_appsdir)/Applications

EXTRA_DIST = $(apps_DATA)

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kedit_SOURCES) && mv messages.po ../po/kedit.pot

