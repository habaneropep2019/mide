 
APPSDIR = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files

SUBDIRS = doc
bin_PROGRAMS = 	kghostview
kghostview_SOURCES = marklist.cpp copyright.cpp interpreter.cpp messages.cpp \
info.cpp print.cpp goto.cpp viewcontrol.cpp main.cpp kghostview.cpp \
kpswidget.cpp ps.c  zoom.cpp
noinst_HEADERS =  marklist.h copyright.h interpreter.h messages.h info.h \
print.h goto.h viewcontrol.h kghostview.h kpswidget.h ps.h  \
zoom.h
kghostview_LDADD =         $(LIB_KFILE)
kghostview_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = marklist.moc copyright.moc interpreter.moc messages.moc \
info.moc print.moc goto.moc viewcontrol.moc kghostview.moc kpswidget.moc \
zoom.moc

EXTRA_DIST = kghostview.kdelnk kghostview.xpm mini-kghostview.xpm large-kghostview.xpm

kde_icon_DATA = kghostview.xpm

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kghostview.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kghostview.xpm $(kde_icondir)/large/kghostview.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kghostview.xpm $(kde_minidir)/kghostview.xpm

uninstall-local:
	rm -f $(APPSDIR)/kghostview.kdelnk
	rm -f $(kde_minidir)/kghostview.xpm
	rm -f $(kde_icondir)/large/kghostview.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kghostview_SOURCES) && mv messages.po ../po/kghostview.pot

