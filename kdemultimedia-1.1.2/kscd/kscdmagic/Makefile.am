
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
APPSDIR = $(kde_appsdir)/Multimedia

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

####### This part is very kscdmagic specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	kscdmagic

# Which sources should be compiled for kscdmagic
kscdmagic_SOURCES = core.cpp main.cpp xlibwrap.cpp xlib.c sound.cpp bitmap.cpp

# the library search path. 
kscdmagic_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kscdmagic_LDADD   =       -lm $(LIB_KDEUI)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = bitmap.h    symbol.h    syna.h      xlib.h magicconf.h

messages:
#	$(XGETTEXT) -C -ktranslate -kTRANS -ki18n -x $(includedir)/kde.pot $(kscdmagic_SOURCES)

