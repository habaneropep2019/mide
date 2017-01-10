
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
APPSDIR = $(kde_appsdir)/System
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = doc pics toolbar

####### This part is very ksysv specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	ksysv

# Which sources should be compiled for ksysv.
ksysv_SOURCES	= main.cpp OldView.cpp Content.cpp \
		  DragListCanvas.cpp TopWidget.cpp moving.cpp prefs.cpp error.cpp \
		  DragList.cpp mylist.cpp trash.cpp \
		  ksv_conf.cpp ksv_core.cpp \
		  PropDlg.cpp IOCore.cpp \
		  ViewPort.cpp About.cpp \
		  Clipboard.cpp ReadOnlyDragList.cpp \
		  Data.cpp kurllabel.cpp

# the library search path
ksysv_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
ksysv_LDADD	=      -lstdc++    $(LIB_KFILE) $(LIBCOMPAT)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = moving.h Content.h \
		 TopWidget.h mylist.h DragListCanvas.h DragList.h OldView.h \
		 trash.h error.h ksv_ipc.h prefs.h version.h \
		 ksv_conf.h \
		 ksv_core.h PropDlg.h IOCore.h ViewPort.h \
		 About.h kurllabel.h \
		 Constants.h Clipboard.h \
		 ReadOnlyDragList.h Data.h

# just to make sure, automake makes them 
METASOURCES = TopWidget.moc OldView.moc DragListCanvas.moc \
	       DragList.moc prefs.moc moving.moc trash.moc \
	       ksv_conf.moc ksv_core.moc PropDlg.moc \
	       IOCore.moc ViewPort.moc About.moc Content.moc \
	       Clipboard.moc ReadOnlyDragList.moc \
	       Data.moc kurllabel.moc

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(BUILTSOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(ksysv_SOURCES) && mv messages.po ../po/ksysv.pot

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/ksysv.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/ksysv.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-ksysv.xpm $(kde_minidir)/ksysv.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-ksysv.xpm $(kde_icondir)/large/ksysv.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/ksysv.kdelnk
	-rm -f $(kde_icondir)/ksysv.xpm
	-rm -f $(kde_minidir)/ksysv.xpm
	-rm -f $(kde_icondir)/large/ksysv.xpm

