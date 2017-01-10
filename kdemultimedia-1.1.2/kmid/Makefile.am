
INCLUDES = $(all_includes)

LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = player/libplayer.a $(LIB_KFILE)

kmid_SOURCES = kmidframe.cpp kmidclient.cpp kdisptext.cpp \
	 kslidertime.cpp klcdnumber.cpp ktrianglebutton.cpp midicfgdlg.cpp \
         collectdlg.cpp kaskdlg.cpp randomlist.cpp channelview.cpp \
         channel.cpp channel3d.cpp channel4d.cpp channelcfgdlg.cpp \
         instrname.cpp rhythmview.cpp main.cpp 


PLAYEROBJECTS = midiout.o player.o track.o midimapper.o midfile.o \
	dattypes.o midistat.o deviceman.o synthout.o

DISTCLEANFILES = $(SRCMETA) ./.deps/* ./.deps/.P

bin_PROGRAMS = kmid
kmid_METASOURCES = USE_AUTOMOC

SUBDIRS = player toolbarxpm maps examples doc

applnk_DATA = kmid.kdelnk
applnkdir = $(kde_appsdir)/Multimedia

mime_DATA = x-karaoke.kdelnk
mimedir = $(kde_mimedir)/audio

icon_DATA = kmid.xpm
icondir = $(kde_icondir)

fmpatches_DATA = std.sb drums.sb std.o3 drums.o3
fmpatchesdir = $(kde_datadir)/kmid/fm

pics_DATA = keyboard.xpm button1.xpm button2.xpm
picsdir = $(kde_datadir)/kmid/pics

EXTRA_DIST = $(applnk_DATA) $(mime_DATA) $(icon_DATA) $(fmpatches_DATA) mini-kmid.xpm $(pics_DATA)

instrname.i18n: instrname.cpp
	echo "/* This is an automatically generated file" > instrname.i18n
	echo " * DO NOT EDIT !!!" >> instrname.i18n
	echo " * To create this file run : ""make instrname.i18n"" " >> instrname.i18n
	echo " */" >> instrname.i18n
	echo "" >> instrname.i18n
	awk -f instrfilter.awk instrname.cpp >> instrname.i18n


messages: instrname.i18n
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kmid_SOURCES) instrname.i18n && mv messages.po ../po/kmid.pot

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/mini-kmid.xpm $(kde_minidir)/kmid.xpm
	$(INSTALL_DATA) $(srcdir)/large-kmid.xpm $(kde_icondir)/large/kmid.xpm
	@echo "********************************************"
	@echo "Thanks for installing kmid 1.7 !"
	@echo "I have included some example songs that have been installed in"
	@echo $(kde_datadir)/kmid
	@echo ""
	@echo "If you find any bug, have any idea or only want to tell me"
	@echo "how much you like kmid :-), just send me an email to:"
	@echo "larrosa@kde.org"
	@echo ""
	@echo "I hope that you have fun with kmid !"
	@echo "Antonio Larrosa Jimenez (larrosa@kde.org)"
	@echo ""
	@echo "PS: I will not be liable for any flood during the use of kmid ;-)"
	@echo "*********************************************"

uninstall-local:
	-rm -f $(kde_minidir)/kmid.xpm

