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

####### This part is very pws specific
# you can add here more. This one gets installed 
bin_PROGRAMS = mathpod

# Which sources should be compiled for pws.
mathpod_SOURCES	= cgi.c\
                  config.c\
                  core.c\
                  dump.c\
                  imap.c\
                  main.c\
                  request.c 


# the library search path
mathpod_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
mathpod_LDADD  = $(LIBSOCKET)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = mathopd.h

# just to make sure, automake makes them 
BUILTSOURCES =

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(BUILTSOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n $(pws_SOURCES)

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:

# add a dependency for every moc file to be full portable
