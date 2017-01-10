
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

# just set the variable
APPSDIR = $(kde_appsdir)/Internet
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = doc

####### This part is very knu specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	knu

# Which sources should be compiled for knu.
knu_SOURCES	= knu.cpp CommandDlg.cpp OptionsDlg.cpp MtrDlg.cpp\
			PingDlg.cpp TracerouteDlg.cpp HostDlg.cpp FingerDlg.cpp

# the library search path.
knu_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
knu_LDADD   =         $(LIB_KFILE)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = knu.h CommandDlg.h OptionsDlg.h MtrDlg.h\
                        PingDlg.h TracerouteDlg.h HostDlg.h FingerDlg.h

EXTRA_DIST = version.h knu.kdelnk knu.xpm knu.lsm mini-knu.xpm

# just to make sure, automake makes them 
METASOURCES =	knu.moc CommandDlg.moc OptionsDlg.moc MtrDlg.moc\
			PingDlg.moc TracerouteDlg.moc HostDlg.moc FingerDlg.moc

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(knu_SOURCES) && mv messages.po ../po/knu.pot

# just install datas here. Use install-exec-local for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/knu.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/knu.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-knu.xpm $(kde_minidir)/knu.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/knu.kdelnk
	-rm -f $(kde_icondir)/knu.xpm
	-rm -f $(kde_minidir)/knu.xpm

