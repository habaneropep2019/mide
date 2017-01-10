VERSION=1.02

INCLUDES = $(all_includes)

LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD =       $(LIB_KDEUI)

KDELNKDIR= $(kde_appsdir)/Games

kabalone_SOURCES = Board.cpp Move.cpp BoardWidget.cpp AbTop.cpp \
	 kabalone.cpp Spy.cpp
METASOURCES =	Board.moc AbTop.moc BoardWidget.moc Spy.moc

noinst_HEADERS = AbTop.h Board.h BoardWidget.h Spy.h Move.h version.h
EXTRA_DIST = kabalone.xpm mini-kabalone.xpm kabalone.kdelnk

bin_PROGRAMS = kabalone

tar: clean
	(rm -f Makefile; cd ..; tar czvf kabalone-$(VERSION).tgz kabalone)

icondir = $(kde_icondir)
icon_DATA = kabalone.xpm

SUBDIRS = html toolbar bitmaps

appsdir = $(kde_appsdir)/Games
apps_DATA = kabalone.kdelnk

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kabalone.xpm $(kde_minidir)/kabalone.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kabalone.xpm $(kde_icondir)/large/kabalone.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kabalone.xpm
	-rm -f $(kde_icondir)/large/kabalone.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kabalone_SOURCES) && mv messages.po ../po/kabalone.pot

# DO NOT DELETE
AbTop.cpp: AbTop.h Board.h Move.h BoardWidget.h Spy.h 
Board.cpp: Board.h Move.h 
BoardWidget.cpp: Board.h Move.h BoardWidget.h bitmaps/Arrow1 bitmaps/Arrow1Mask
BoardWidget.cpp: bitmaps/Arrow2 bitmaps/Arrow2Mask bitmaps/Arrow3
BoardWidget.cpp: bitmaps/Arrow3Mask bitmaps/Arrow4 bitmaps/Arrow4Mask
BoardWidget.cpp: bitmaps/Arrow5 bitmaps/Arrow5Mask bitmaps/Arrow6
BoardWidget.cpp: bitmaps/Arrow6Mask 
Move.cpp: Move.h
Spy.cpp: Spy.h Board.h Move.h BoardWidget.h 
kabalone.cpp: AbTop.h Board.h Move.h BoardWidget.h Spy.h



