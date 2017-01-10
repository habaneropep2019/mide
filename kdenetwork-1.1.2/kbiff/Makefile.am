# just set the variable
APPSDIR = $(kde_appsdir)/Internet

# set the include path for X, qt and KDE
INCLUDES= $(all_includes) -I. -I$(srcdir) -I..

# claim, which subdirectories you want to install
SUBDIRS = doc pics

####### This part is very kbiff specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	kbiff

kbiff_METASOURCES =  USE_AUTOMOC

# Which sources should be compiled for kbiff.
kbiff_SOURCES	= setupdlg.cpp kbiff.cpp main.cpp kbiffmonitor.cpp notify.cpp Trace.cpp kbiffurl.cpp status.cpp $(kbiff_METASOURCES)

# the library search path
kbiff_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kbiff_LDADD   =     -lmediatool     $(LIB_KFILE)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = setupdlg.h kbiff.h kbiffmonitor.h notify.h Trace.h kbiffurl.h status.h

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(kbiff_METASOURCES)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po ../po/kbiff.pot

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kbiff.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/kbiff.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kbiff.xpm $(kde_minidir)/kbiff.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/kbiff.kdelnk
	-rm -f $(kde_icondir)/kbiff.xpm
	-rm -f $(kde_minidir)/kbiff.xpm

