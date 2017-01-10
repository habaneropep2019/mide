
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
appsdir = $(kde_appsdir)/Utilities

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# claim which subdirectories you want to install
SUBDIRS = docs icons

####### This part is very kpm specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	kpm kpmdocked

# Which sources should be compiled for kpm
kpmdocked_SOURCES = docked.C
kpm_SOURCES =	dialogs.C infobar.C pstable.C svec.C \
		wchan.C \
		fieldsel.C prefs.C qps.C ttystr.C \
		details.C htable.C proc.C scheddlg.C uidstr.C \
		moc.C buttongroup.C

# the library search path. 
kpmdocked_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kpm_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kpmdocked_LDADD   =       $(LIB_KDEUI)
kpm_LDADD   =       $(LIB_KDEUI)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = dialogs.h infobar.h pstable.h svec.h wchan.h \
		 fieldsel.h prefs.h qps.h ttystr.h \
		 details.h htable.h proc.h scheddlg.h uidstr.h \
		 buttongroup.h docked.h

# just to make sure, automake makes them 
METASOURCES =	dialogs.moc infobar.moc pstable.moc \
		fieldsel.moc prefs.moc qps.moc \
		details.moc htable.moc scheddlg.moc \
		docked.moc

messages:
	$(XGETTEXT) -C -ktranslate -kTRANS -ki18n -x $(includedir)/kde.pot $(kpm_SOURCES) && mv messages.po ../po/kpm.pot

apps_DATA = kpm.kdelnk

