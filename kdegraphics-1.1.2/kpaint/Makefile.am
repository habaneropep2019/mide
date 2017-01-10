# $Id: Makefile.am,v 1.15.2.1 1999/07/11 16:23:00 rahn Exp $
##

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD	=       $(LIB_KIMGIO) -lgif -lm $(LIB_KFILE)

appsdir = $(kde_appsdir)/Graphics
apps_DATA = kpaint.kdelnk

icondir = $(kde_icondir)
icon_DATA = kpaint.xpm

kpaint_SOURCES = main.cpp app.cpp canvas.cpp kpaint.cpp manager.cpp \
	formats.cpp infodialog.cpp palette.cpp palettedialog.cpp \
	formatdialog.cpp canvassize.cpp properties.cpp gif.cpp \
	QwViewport.cpp tool.cpp ellipse.cpp circle.cpp line.cpp \
	spraycan.cpp rectangle.cpp pen.cpp areaselect.cpp depthdialog.cpp \
	roundangle.cpp navwidget.cpp sidebar.cpp view.cpp colorbar.cpp \
	mainview.cpp text.cpp

noinst_HEADERS = QwViewport.h areaselect.h canvas.h canvassize.h circle.h \
	depthdialog.h ellipse.h formatdialog.h formats.h gif.h infodialog.h \
	kpaint.h kundo.h line.h main.h manager.h navwidget.h palette.h \
	palettedialog.h pen.h properties.h rectangle.h roundangle.h sidebar.h \
	spraycan.h tool.h version.h view.h colorbar.h mainview.h text.h

METASOURCES = canvas.moc app.moc kpaint.moc manager.moc infodialog.moc \
	depthdialog.moc properties.moc formatdialog.moc palette.moc \
	palettedialog.moc QwViewport.moc tool.moc spraycan.moc circle.moc \
	navwidget.moc sidebar.moc view.moc rectangle.moc roundangle.moc \
	pen.moc ellipse.moc colorbar.moc mainview.moc areaselect.moc text.moc \
	line.moc

bin_PROGRAMS = kpaint

SUBDIRS = doc toolpics images

EXTRA_DIST = $(apps_DATA) $(icon_DATA) mini-kpaint.xpm large-kpaint.xpm

install-data-local: mini-kpaint.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kpaint.xpm $(kde_minidir)/kpaint.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kpaint.xpm $(kde_icondir)/large/kpaint.xpm


uninstall-local:
	-rm -f $(kde_minidir)/kpaint.xpm
	-rm -f $(kde_icondir)/large/kpaint.xpm


messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po ../po/kpaint.pot

