
INCLUDES = $(all_includes)

LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD   = $(LIB_KDEUI) -lm

noinst_HEADERS = game.h trys.h rattler.h board.h\
		level.h ball.h snake.h\
		basket.h startroom.h lcdrange.h score.h\
		pixServer.h progress.h levels.h view.h keys.h version.h \
		bitmaps.h

ksnake_SOURCES = game.cpp trys.cpp rattler.cpp board.cpp\
		level.cpp ball.cpp snake.cpp\
		basket.cpp startroom.cpp lcdrange.cpp\
		score.cpp pixServer.cpp progress.cpp levels.cpp\
		view.cpp keys.cpp

ksnake_METASOURCES = USE_AUTOMOC

bin_PROGRAMS = 	ksnake

######### build rules ################

apps_DATA = ksnake.kdelnk
appsdir = $(kde_appsdir)/Games

kde_icon_DATA = ksnake.xpm

EXTRA_DIST = $(apps_DATA) $(kde_icon_DATA) mini-ksnake.xpm

SUBDIRS = doc data 

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-ksnake.xpm $(kde_minidir)/ksnake.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-ksnake.xpm $(kde_icondir)/large/ksnake.xpm

uninstall-local:
	-rm -f $(kde_minidir)/ksnake.xpm
	-rm -f $(kde_icondir)/large/ksnake.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(ksnake_SOURCES) && mv messages.po ../po/ksnake.pot

