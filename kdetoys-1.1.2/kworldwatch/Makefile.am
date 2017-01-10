INCLUDES= $(all_includes) 

bin_PROGRAMS = kworldwatch
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KFM) $(LIB_KDEUI) 

####### Files

kworldwatch_SOURCES  = main.cpp toplevel.cpp world.cpp kinputline.cpp \
			 astro.c sunclock.c
METASOURCES  = toplevel.moc world.moc kinputline.moc

SUBDIRS = pics

kde_icon_DATA = kworldwatch.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(mkinstalldirs) $(kde_icondir)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/mini-kworldwatch.xpm $(kde_minidir)/kworldwatch.xpm
	$(INSTALL_DATA) $(srcdir)/large-kworldwatch.xpm $(kde_icondir)/large/kworldwatch.xpm
	$(mkinstalldirs) $(kde_appsdir)/Utilities
	$(INSTALL_DATA) $(srcdir)/KWorldWatch.kdelnk $(kde_appsdir)/Utilities

uninstall-local:
	rm -f $(kde_minidir)/kworldwatch.xpm
	rm -f $(kde_icondir)/large/kworldwatch.xpm
	rm -r $(kde_appsdir)/Utilities

messages:
	$(XGETTEXT) -C -ki18n -ktranslate -x $(includedir)/kde.pot $(kworldwatch_SOURCES) && mv messages.po ../po/kworldwatch.pot

