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

SUBDIRS = doc

INCLUDES= $(all_includes) 
# $(MS_NANA_DEFINES)
# -fexternal-templates 

bin_PROGRAMS = 	kab kabapi_test
kab_SOURCES	= main.cc \
		AddressBookAbout.cpp \
		AddressBookAboutData.cpp \
		toplevelwidget.cc
kab_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kab_LDADD   = $(LIB_KAB) $(LIB_KFILE) 

kabapi_test_SOURCES= kabapi_test.cc
kabapi_test_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kabapi_test_LDADD= $(LIB_KAB) $(LIB_KFILE)


# I have added all additional files that need to be in the archive here.
# Hopefully this does not harm.
METASOURCES = 	toplevelwidget.moc \
		AddressBookAboutData.moc \
		AddressBookAbout.moc 

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kab_SOURCES) && mv messages.po ../po/kab.pot

kde_icon_DATA = kab.xpm

appsdir = $(kde_appsdir)/Utilities
apps_DATA = kab.kdelnk

pics_DATA = addressbook_logo.jpg
picsdir = $(kde_datadir)/kab/pics

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kab.xpm $(kde_minidir)/kab.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kab.xpm $(kde_icondir)/large/kab.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kde_minidir)/kab.xpm
	-rm -f $(kde_icondir)/large/kab.xpm

