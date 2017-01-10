
bin_PROGRAMS = ksame
INCLUDES = $(all_includes)
LDFLAGS  = $(all_libraries) $(KDE_RPATH)
ksame_LDADD = $(LIB_KDEUI) 

ksame_SOURCES= main.cpp StoneWidget.cpp KSameWidget.cpp HighScore.cpp
METASOURCES = StoneWidget.moc KSameWidget.moc HighScore.moc

noinst_HEADERS = StoneWidget.h KSameWidget.h HighScore.h version.h

appsdir = $(kde_appsdir)/Games
picsdir = $(kde_datadir)/ksame/pics

kde_icon_DATA = ksame.xpm

pics_DATA = stones.gif stones_mask.gif
apps_DATA = ksame.kdelnk

SUBDIRS = docs

EXTRA_DIST = $(pics_DATA) $(apps_DATA) ksame.xpm mini-ksame.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-ksame.xpm $(kde_minidir)/ksame.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-ksame.xpm $(kde_icondir)/large/ksame.xpm

uninstall-local:
	rm -f $(kde_minidir)/ksame.xpm
	rm -f $(kde_icondir)/large/ksame.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(ksame_SOURCES) && mv messages.po ../po/ksame.pot

