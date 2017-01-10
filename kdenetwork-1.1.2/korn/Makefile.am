SUBDIRS = doc

korn_METASOURCES = USE_AUTOMOC

INCLUDES= $(all_includes)
bin_PROGRAMS = korn
korn_SOURCES = kmailbox.cpp kmailbox.h korn.cpp korn.h kornbutt.cpp kornbutt.h kornopt.cpp kornopt.h kornset.cpp kornset.h main.cpp optdata.cpp optdata.h real_from.cpp 
korn_LDFLAGS = $(all_libraries) $(KDE_RPATH)
korn_LDADD =    $(LIB_KFILE)

EXTRA_DIST = KOrn.kdelnk korn.xpm mini-korn.xpm

kde_icon_DATA = korn.xpm

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Internet
	$(mkinstalldirs) $(kde_htmldir)/en/korn
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/KOrn.kdelnk $(kde_appsdir)/Internet
	$(INSTALL_DATA) $(srcdir)/doc/index.html $(kde_htmldir)/en/korn
	$(INSTALL_DATA) $(srcdir)/mini-korn.xpm $(kde_minidir)/korn.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-korn.xpm $(kde_icondir)/large/korn.xpm

uninstall-local:
	rm -f $(kde_minidir)/korn.xpm
	rm -f $(kde_htmldir)/en/korn/index.html
	rm -f $(kde_appsdir)/Internet/KOrn.kdelnk
	-rmdir $(kde_htmldir)/en/korn
	-rmdir $(kde_htmldir)/en/
	-rmdir $(kde_appsdir)/Internet
	rm -f $(kde_icondir)/large/korn.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(korn_SOURCES) && mv messages.po ../po/korn.pot

