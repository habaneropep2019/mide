
INCLUDES = $(all_includes)
LDFLAGS   = $(all_libraries) $(KDE_RPATH)
LDADD =  $(LIB_KFILE)

bin_PROGRAMS = kjots

kjots_SOURCES = main.cpp\
	KJotsMain.cpp\
	SubjList.cpp\
	ReadListConf.cpp\
	cpopmenu.cpp\
	confdialog.cpp

kjots_METASOURCES = USE_AUTOMOC 

SUBDIRS = icons doc

appsdir = $(kde_appsdir)/Utilities
apps_DATA = Kjots.kdelnk

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po ../po/kjots.pot

