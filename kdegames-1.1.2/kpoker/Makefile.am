

INCLUDES = $(all_includes) 

LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = -lmediatool       $(LIB_KDEUI) $(LIBCOMPAT)

appsdir = $(kde_appsdir)/Games
apps_DATA = kpoker.kdelnk

kde_icon_DATA = kpoker.xpm

bin_PROGRAMS = kpoker
kpoker_SOURCES = kpoker.cpp kpaint.cpp top.cpp main.cpp

noinst_HEADERS = defines.h global.h kpaint.h kpoker.h top.h version.h

METASOURCES =	kpoker.moc kpaint.moc top.moc

SUBDIRS = docs sounds bitmaps

EXTRA_DIST = $(apps_DATA) $(kde_icon_DATA) mini-kpoker.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kpoker.xpm $(kde_minidir)/kpoker.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kpoker.xpm $(kde_icondir)/large/kpoker.xpm

uninstall-local:
	rm -f $(kde_minidir)/kpoker.xpm
	rm -f $(kde_icondir)/large/kpoker.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kpoker_SOURCES) && mv messages.po ../po/kpoker.pot 

