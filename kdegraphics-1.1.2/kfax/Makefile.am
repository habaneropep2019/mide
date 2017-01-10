# $Id: Makefile.am,v 1.15.2.1 1999/07/11 16:22:55 rahn Exp $

LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       ./libtiffax/libtif.a  -lm $(LIB_KFILE) 
INCLUDES=	-I$(srcdir)/libtiffax $(all_includes)

####### Files
noinst_HEADERS = kfax.h faxexpand.h print.h options.h urldlg.h

kfax_SOURCES   = kfax.cpp viewfax.cpp faxexpand.cpp faxinit.cpp faxinput.cpp\
		 fax2tiff.c fax2ps.c print.cpp options.cpp urldlg.cpp

METASOURCES    = kfax.moc print.moc options.moc urldlg.moc

bin_PROGRAMS   = kfax

SUBDIRS = libtiffax docs

appsdir = $(kde_appsdir)/Graphics
apps_DATA = KFax.kdelnk

picsdir = $(kde_datadir)/kfax/pics
pics_DATA = kfax.tif kfaxlogo.xpm

icondir = $(kde_icondir)
icon_DATA = kfax.xpm

EXTRA_DIST = $(apps_DATA) $(pics_DATA) $(icon_DATA) mini-kfax.xpm large-kfax.xpm

install-data-local: mini-kfax.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kfax.xpm $(kde_icondir)/large/kfax.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kfax.xpm $(kde_minidir)/kfax.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kfax_SOURCES) print.h && mv messages.po ../po/kfax.pot

uninstall-local:
	rm -f $(kde_minidir)/kfax.xpm
	rm -f $(kde_icondir)/large/kfax.xpm

####### Explicit dependencies


