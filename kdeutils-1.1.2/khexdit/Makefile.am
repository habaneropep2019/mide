
APPSDIR = $(kde_appsdir)/Utilities
INCLUDES= $(all_includes)

####### Files

SUBDIRS = doc
bin_PROGRAMS = 	khexdit
khexdit_SOURCES	= hexdata.cpp hexfile.cpp hexwidget.cpp
noinst_HEADERS = hexdata.h hexfile.h hexwidget.h
khexdit_LDADD = $(LIB_KFILE)
khexdit_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = hexwidget.moc hexfile.moc

appsdir = $(kde_appsdir)/Utilities
apps_DATA = khexdit.kdelnk

kde_icon_DATA = khexdit.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-khexdit.xpm $(kde_minidir)/khexdit.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-khexdit.xpm $(kde_icondir)/large/khexdit.xpm

uninstall-local:
	rm -f $(kde_minidir)/khexdit.xpm
	rm -f $(kde_icondir)/large/khexdit.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(khexdit_SOURCES) && mv messages.po ../po/khexdit.pot

edit:
	xemacs $(khexdit_SOURCES) $(noinst_HEADERS) &
