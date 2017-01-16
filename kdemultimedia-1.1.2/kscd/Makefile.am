# $Id$

MIMEDIR = $(kde_mimedir)/text/
PICSDIR  = $(kde_datadir)/kscd/pics/
APPSDIR = $(kde_appsdir)/Multimedia
CDDBDIR = $(kde_datadir)/kscd/cddb/

LDFLAGS	= $(all_libraries) $(KDE_RPATH)
kscd_LDADD   = $(LIBCDROM) $(LIBCDAUDIO) $(LIBFPE)  $(LIB_KFM) $(LIB_KDEUI)
INCLUDES= $(all_includes)
LINK    = $(CXXLINK)

####### Files

noinst_HEADERS = kscd.h struct.h bwlednum.h ledlamp.h inexact.h \
	CDDialog.h CDDialogData.h CDDBSetup.h CDDBSetupData.h mgconfdlg.h \
	configdlg.h cddb.h version.h docking.h wmcdda.h smtpconfig.h \
	smtp.h

kscd_SOURCES = kscd.cpp configdlg.cpp cddb.cpp ledlamp.cpp CDDialog.cpp \
	CDDialogData.cpp CDDBSetup.cpp CDDBSetupData.cpp docking.cpp \
	inexact.cpp cdinfo.c mgconfdlg.cpp \
	plat_scor5.c plat_sun.c plat_hpux.c plat_linux.c plat_news.c \
	plat_bsd386.c plat_ultrix.c cdrom.c scsi.c drv_toshiba.c drv_sony.c \
	plat_svr4.c plat_freebsd.c plat_osf1.c plat_irix.c bwlednum.cpp \
	smtpconfig.cpp smtp.cpp

cddaslave_SOURCES = plat_sun_audio.c plat_sun_cdda.c cddaslave.c  

SOURCES3=kscdmagic/bitmap.cpp  kscdmagic/core.cpp kscdmagic/main.cpp \
	kscdmagic/sound.cpp kscdmagic/xlibwrap.cpp kscdmagic/xlib.c

OBJECTS3=kscdmagic/bitmap.o  kscdmagic/core.o kscdmagic/main.o \
	kscdmagic/sound.o kscdmagic/xlibwrap.o kscdmagic/xlib.o

METASOURCES = kscd.moc bwlednum.moc configdlg.moc cddb.moc ledlamp.moc \
	 inexact.moc CDDialog.moc CDDialogData.moc CDDBSetup.moc \
	CDDBSetupData.moc docking.moc mgconfdlg.moc smtpconfig.moc \
	smtp.moc

DISTCLEANFILES = config.h

bin_PROGRAMS = kscd cddaslave 
TARGET3 = kscdmagic

config.h: kscd-script 
	$(SHELL) $(srcdir)/kscd-script

SUBDIRS = docs kscdmagic bitmaps sbpcdhack

CDDB_DIRS = rock blues classical jazz newage soundtrack \
	reggae folk country misc data

bin_SCRIPTS = workman2cddb.pl

EXTRA_DIST = kscd.xpm mini-kscd.xpm kscdlogo.xpm xmcd.kdelnk kscd.kdelnk kscd-script $(bin_SCRIPTS) config.h.std

install-data-local:
	for i in $(CDDB_DIRS); do \
	if test ! -d $(CDDBDIR)/$$i; then \
		$(mkinstalldirs) $(CDDBDIR)/$$i ;\
		chmod 0777 $(CDDBDIR)/$$i ;\
	fi ;\
	done 
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/kscd.xpm $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/bitmaps/cd.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/mini-kscd.xpm $(kde_minidir)/kscd.xpm
	$(INSTALL_DATA) $(srcdir)/large-kscd.xpm $(kde_icondir)/large/kscd.xpm
	$(mkinstalldirs) $(PICSDIR)
	$(INSTALL_DATA) $(srcdir)/kscdlogo.xpm $(PICSDIR)
	$(INSTALL_DATA) $(srcdir)/bitmaps/cdsmall.xpm $(PICSDIR)
	$(mkinstalldirs) $(MIMEDIR)
	$(INSTALL_DATA) $(srcdir)/xmcd.kdelnk $(MIMEDIR)/xmcd.kdelnk
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kscd.kdelnk $(APPSDIR)/kscd.kdelnk

uninstall:
	rm -f $(APPSDIR)/kscd.kdelnk
	rm -f $(PICSDIR)/kscdlogo.xpm
	rm -f $(kde_icondir)/kscd.xpm
	rm -f $(kde_minidir)/kscd.xpm
	rm -f $(kde_icondir)/cd.xpm
	rm -f $(MIMEDIR)/xmcd.kdelnk

messages:
	$(XGETTEXT) -C --keyword=translate *.cpp -x../../kdelibs/po/kde.pot && mv messages.po ../po/kscd.pot   

cddaslave.c:config.h
cdrom.c:config.h
plat_bsd386.c:config.h
plat_freebsd.c:config.h
plat_hpux.c:config.h
plat_irix.c:config.h
plat_linux.c:config.h
plat_news.c:config.h
plat_osf1.c:config.h
plat_scor5.c:config.h
plat_sun.c:config.h
plat_sun_audio.c:config.h
plat_sun_cdda.c:config.h
plat_svr4.c:config.h
plat_ultrix.c:config.h     

