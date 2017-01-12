INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD =   -lQwSpriteField -lmediatool     $(LIB_KDEUI) $(LIBCOMPAT)

kasteroids_SOURCES = main.cpp view.cpp toplevel.cpp kfixedtopwidget.cpp
METASOURCES = view.moc toplevel.moc kfixedtopwidget.moc

noinst_HEADERS = kfixedtopwidget.h sprites.h toplevel.h version.h view.h

EXTRA_DIST = mini-kasteroids.xpm kasteroids.xpm  kasteroids.kdelnk bg.ppm

bin_PROGRAMS = kasteroids

SUBDIRS = html sprites

picsdir = $(kde_datadir)/kasteroids/sprites
pics_DATA = bg.ppm

appsdir = $(kde_appsdir)/Games
apps_DATA = kasteroids.kdelnk

kde_icon_DATA = kasteroids.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kasteroids.xpm $(kde_minidir)/kasteroids.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kasteroids.xpm $(kde_icondir)/large/kasteroids.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kasteroids.xpm
	-rm -f $(kde_icondir)/large/kasteroids.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kasteroids_SOURCES) && mv messages.po ../po/kasteroids.pot

