
APPSDIR = $(kde_appsdir)/Multimedia
INCLUDES= $(all_includes)

####### Files

SUBDIRS = pics docs
bin_PROGRAMS = 	kmedia
kmedia_SOURCES	= kmediaui.cpp mediaactions.cpp uiactions.cpp kmediawin.cpp prefs.cpp
noinst_HEADERS = kmediaui.h kmediawin.h prefs.h
kmedia_LDADD =  -lmediatool $(LIBCOMPAT) $(LIB_KFILE)

kmedia_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = kmediaui.moc kmediawin.moc prefs.moc

EXTRA_DIST = KMedia.kdelnk VERSION

$(srcdir)/version.h: VERSION
	cd $(srcdir); \
	sed "s/1/#define APP_VERSION \"1/" <VERSION > version.h ;\
	echo \" >> version.h

$(srcdir)/uiactions.cpp: version.h

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/KMedia.kdelnk $(APPSDIR)

uninstall-local:
	rm -f $(APPSDIR)/KMedia.kdelnk

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kmedia_SOURCES) && mv messages.po ../po/kmedia.pot

edit:
	xemacs $(kmedia_SOURCES) $(noinst_HEADERS) &
