# Yow Emacs, this is a -*- makefile -*-

# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# if you use a variable *dir and have *_DATA, it will be installed by
# make install
appsdir = $(kde_appsdir)/Internet
apps_DATA = Kppp.kdelnk

EXTRA_DIST = $(apps_DATA)

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = docs pixmaps icons logview Rules

# This one gets installed 
bin_PROGRAMS = 	kppp

# Which sources should be compiled for kppp.
kppp_SOURCES	= accounts.cpp \
		connect.cpp \
		conwindow.cpp \
		debug.cpp \
		edit.cpp \
		general.cpp \
		iplined.cpp \
		main.cpp \
                modem.cpp \
		modemcmds.cpp \
		pppdargs.cpp \
		pppdata.cpp \
		scriptedit.cpp\
		pwentry.cpp \
		modeminfo.cpp\
		pppstatdlg.cpp\
		pppstats.cpp\
		miniterm.cpp\
		accounting.cpp \
		acctselect.cpp\
		ruleset.cpp \
		docking.cpp \
		runtests.cpp \
		loginterm.cpp \
		ppplog.cpp \
		log.cpp \
		groupbox.cpp \
		newwidget.cpp \
		requester.cpp \
		opener.cpp \
		modemdb.cpp

# the library search path
kppp_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kppp_LDADD   =       -lm $(LIB_KDEUI)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = accounts.h \
		kpppconfig.h \
		connect.h \
		conwindow.h \
		debug.h \
		edit.h \
		general.h \
		iplined.h \
		main.h \
                modem.h \
		modemcmds.h \
		pppdargs.h \
		pppdata.h \
		scriptedit.h\
		pwentry.h \
		modeminfo.h\
		pppstatdlg.h\
		miniterm.h\
		accounting.h \
		acctselect.h\
		ruleset.h \
		docking.h \
		runtests.h \
		loginterm.h \
		auth.h \
		version.h \
		macros.h \
		ppplog.h \
		log.h \
		groupbox.h \
		newwidget.h \
		modemdb.h

METASOURCES = accounting.moc \
	accounts.moc \
	acctselect.moc \
	connect.moc \
	conwindow.moc \
	debug.moc \
	docking.moc \
	edit.moc \
	general.moc \
	groupbox.moc \
	loginterm.moc \
	main.moc \
	miniterm.moc \
	modem.moc \
	modemcmds.moc \
	modemdb.moc \
	modeminfo.moc \
	pppdargs.moc \
	pppstatdlg.moc \
	pwentry.moc \
	scriptedit.moc  

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kppp_SOURCES) && mv messages.po ../po/kppp.pot

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local:
	@echo ""
	@echo "Don't forget to send us a phone tarif rules file for your"
	@echo "telecom company/country/region. Thanks."
	@echo ""
	@echo "We will be maintaining an archive on the kppp page at"
	@echo "http://devel-home.kde.org/kppp/index.html"
	@echo "WARNING: kppp is by default installed with SETUID bit on!"
	@echo  "If you are concerned about the SETUID bit, create a modem"
	@echo "group instead."
	@echo ""
	@echo "Happy Dialing! -- the KPPP team"
	@echo ""

# kppp wants to be installed suid root
install-exec-local:
	@(chown root $(bindir)/kppp && chmod 4755 $(bindir)/kppp) || echo "Was not able to make kppp setuid root"

# remove ALL you have installed in install-data-local or install-exec-local
#uninstall-local:
#	@(cd Rules && $(MAKE) uninstall)

#distclean-local:
#	@(cd Rules && $(MAKE) distclean)

