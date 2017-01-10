
INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD =   $(LIB_KDEUI)

kfract_SOURCES = kfract.C kfractdlgs.C drawview.C fract.C \
                mandel.C julia.C main.C
METASOURCES = kfract.moc kfractdlgs.moc 
bin_PROGRAMS   = kfract

noinst_HEADERS = kfract.h kfractdlgs.h drawview.h fract.h julia.h lambda.h \
				mandel.h version.h

lnk_DATA = kfract.kdelnk
lnkdir = $(kde_appsdir)/Graphics

icon_DATA = kfract.xpm
icondir = $(kde_icondir)

docs_DATA = index.html kfract.gif
docsdir = $(kde_htmldir)/en/kfract

EXTRA_DIST = $(lnk_DATA) $(icon_DATA) $(docs_DATA) mini-kfract.xpm large-kfract.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kfract.xpm $(kde_minidir)/kfract.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kfract.xpm $(kde_icondir)/large/kfract.xpm

uninstall:
	rm -f $(kde_minidir)/kfract.xpm
	rm -f $(kde_icondir)/large/kfract.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kfract_SOURCES) && mv messages.po ../po/kfract.pot

