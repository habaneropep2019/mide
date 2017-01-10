SUBDIRS = layout doc
bin_PROGRAMS= kview

#CORBA_SRC = kmicoapp.h kmicoapp.cpp
CORBA_SRC=

kview_METASOURCES = USE_AUTOMOC

INCLUDES = $(all_includes) -I/usr/local/mico/include -I$(srcdir)/layout
kview_SOURCES = canvas.cpp kview.cpp main.cpp \
	viewer.cpp canvas.h kview.h \
	version.h viewer.h filter.h filter.cpp filtlist.h filtlist.cpp \
	colour.h colour.cpp filtmenu.h filtmenu.cpp ilistdlg.h ilistdlg.cpp \
	numdlg.h numdlg.cpp kcproc.h kcproc.cpp khelpidx.h khelpidx.cpp \
	prefdlg.h prefdlg.cpp kaccelmenuwatch.h kaccelmenuwatch.cpp \
	$(CORBA_SRC) 

lnkdir = $(kde_appsdir)/Graphics
lnk_DATA = kview.kdelnk

EXTRA_DIST = $(lnk_DATA)

kview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kview_LDADD = layout/liblayout.a $(LIB_KFILE) $(LIB_KIMGIO) 

kde_icon_DATA = kview.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kview.xpm $(kde_minidir)/kview.xpm
	$(mkinstalldirs) $(kde_icondir)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kview.xpm $(kde_icondir)/large/kview.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kview.xpm
	-rm -f $(kde_icondir)/large/kview.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kview_SOURCES) && mv messages.po ../po/kview.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/kview kview *.h -lqt -lkdecore -lkdeui -lkfile 

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
