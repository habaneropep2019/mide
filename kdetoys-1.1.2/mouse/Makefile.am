#Makefile.am for kodo $Id$

# just set the variable
APPSDIR = $(kde_appsdir)/Utilities
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# if you don't have subdirectories, comment it
#SUBDIRS = 

####### This part is very kodo specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	kodo

# Which sources should be compiled for kodo.
kodo_SOURCES = kodo.cpp kimgnum.cpp gridbag.cpp

kodo_METASOURCES = USE_AUTOMOC

# the library search path. 
kodo_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kodo_LDADD   =       $(LIB_KDEUI)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = kodo.h kimgnum.h gridbag.h

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(kodo_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kodo_SOURCES) && mv messages.po ../po/kodo.pot

# just install datas here. Use install-exec-local for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kodo.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_datadir)/kodo
	$(INSTALL_DATA) $(srcdir)/57-2.gif $(kde_datadir)/kodo
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/kodo.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kodo.xpm $(kde_minidir)/kodo.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kodo.xpm $(kde_icondir)/large/kodo.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/kodo.kdelnk
	-rm -f $(kde_icondir)/kodo.xpm
	-rm -f $(kde_minidir)/kodo.xpm
	-rm -f $(kde_icondir)/large/kodo.xpm


doc_DATA = index.html logotp3.gif guide.gif
docdir = $(kde_htmldir)/en/kodo
