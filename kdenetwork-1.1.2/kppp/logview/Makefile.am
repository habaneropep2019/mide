
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

apps_DATA = kppplogview.kdelnk

EXTRA_DIST = $(apps_DATA)

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install

INSTALL_PROGRAM=$(INSTALL)

# This one gets installed 
bin_PROGRAMS = 	kppplogview

# Which sources should be compiled for kppp.
kppplogview_SOURCES =	main.cpp loginfo.cpp log.cpp monthly.cpp

# the library search path
kppplogview_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kppplogview_LDADD   =         $(LIB_KFILE)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = loginfo.h log.h monthly.h main.h

# just to make sure, automake makes them 
METASOURCES = monthly.moc main.moc

