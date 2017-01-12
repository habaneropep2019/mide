#
#                           KMidi Makefile
#
#  Copyright 1997 Bernd Johannes Wuebben, wuebben@math.cornell.edu
#
#  $Id$
#

VERSION = 0.2i

INCLUDES = $(all_includes)
AM_CFLAGS = -DDEFAULT_PATH=\"$(TIMID_DIR)\" -DKMIDI
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KFM) $(LIB_KDEUI) $(LIBALIB) $(LIBAUDIO) $(LIBMME) 

# Where to install the patches, config files, and MIDI files.
# If you change this, it's a good idea to recompile the binary,
# or you'll need to invoke timidity with the -L option.
TIMID_DIR = $(kde_datadir)/kmidi/config


bin_PROGRAMS = kmidi

kmidi_SOURCES = kmidi.cpp  playlist.cpp bwlednum.cpp configdlg.cpp log.cpp\
	timidity.c common.c readmidi.c playmidi.c resample.c mix.c instrum.c \
	tables.c controls.c output.c filter.c wave_a.c raw_a.c dumb_c.c \
	sndfont.c readsbk.c linux_a.c ctl.c dec_a.c sun_a.c hpux_a.c \
	b_out.c fffload.c

noinst_HEADERS = kmidi.h  playlist.h bwlednum.h configdlg.h\
	config.h common.h readmidi.h playmidi.h resample.h mix.h instrum.h \
	tables.h controls.h output.h filter.h log.h version.h filepic.h \
	cduppic.h constants.h folderpic.h sbk.h sflayer.h

METASOURCES = kmidi.moc  playlist.moc bwlednum.moc configdlg.moc log.moc

SUBDIRS = docs config bitmaps patch

appsdir = $(kde_appsdir)/Multimedia
apps_DATA = KMidi.kdelnk

kde_icon_DATA = kmidi.xpm

picsdir = $(kde_datadir)/kmidi/pics
pics_DATA = kmidilogo.xpm

EXTRA_DIST = $(apps_DATA) $(kde_icon_DATA) $(pics_DATA) kmidi-mini.xpm


install-data-local:
	$(mkinstalldirs) $(TIMID_DIR)/patch
	if [ ! -f $(TIMID_DIR)/patch/acpiano.pat ]; then $(INSTALL_DATA) $(srcdir)/patch/acpiano.pat $(TIMID_DIR)/patch ; fi
	if [ ! -f $(TIMID_DIR)/patch/nylongt2.pat ]; then $(INSTALL_DATA) $(srcdir)/patch/nylongt2.pat $(TIMID_DIR)/patch ;fi

	$(mkinstalldirs) $(kde_minidir)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/kmidi-mini.xpm $(kde_minidir)/kmidi.xpm
	$(INSTALL_DATA) $(srcdir)/large-kmidi.xpm $(kde_icondir)/large/kmidi.xpm

	@echo ""
	@echo "Important:"
	@echo "=========="	
	@echo "Midis:"
	@echo "I have included a couple of nice midis in the kmidi/midis directory."
	@echo ""
	@echo "Sound Patches:"
	@echo "Only a simple  piano and guitar sound patch come with this distribution."
	@echo "This  means that you can only play guitar or piano midis such as the"
	@echo "included cavatina.mid, malaguena.mid until you get yourself a full set" 
	@echo "of general midi patches. Read the html docs for more info on how to obtain "
	@echo "a full set of General Midi (GM)  patches."
	@echo ""
	@echo "ONCE YOU HAVE A FULL SET YOU WON'T BELIEVE YOUR EARS -- PROMISE!"
	@echo ""
	@echo "Have fun with kmidi -- Bernd Wuebben <wuebben@kde.org>"



uninstall:
	rm -f $(TIMID_DIR)/patch/acpiano.pat
	rm -f $(TIMID_DIR)/patch/nylongt2.pat


messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kmidi_SOURCES) && mv messages.po ../po/kmidi.pot

