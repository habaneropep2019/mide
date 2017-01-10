
bin_PROGRAMS = kmoon

kmoon_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kmoon_LDADD =   $(LIB_KDEUI) -lm
kmoon_SOURCES = kmoon.cpp phases.cpp
noinst_HEADERS = kmoon.h

INCLUDES = $(all_includes)

METASOURCES = kmoon.moc

kdelnkdir = $(kde_appsdir)/Utilities
kdelnk_DATA = kmoon.kdelnk

kde_icon_DATA = kmoon.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(mkinstalldirs) $(kde_icondir)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/mini-kmoon.xpm $(kde_minidir)/kmoon.xpm
	$(INSTALL_DATA) $(srcdir)/large-kmoon.xpm $(kde_icondir)/large/kmoon.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kmoon.xpm
	-rm -f $(kde_icondir)/large/kmoon.xpm

SUBDIRS = pics

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kmoon_SOURCES) && mv messages.po ../po/kmoon.pot

