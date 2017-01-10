
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
APPSDIR = $(kde_appsdir)/Utilities
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = doc

####### This part is very kdat specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	kdat

# Which sources should be compiled for kdat.
kdat_SOURCES	=			\
	Archive.cpp			\
	ArchiveInfoWidget.cpp		\
	BackupDlg.cpp			\
	BackupOptDlg.cpp		\
	BackupProfile.cpp		\
	BackupProfileInfoWidget.cpp	\
	BackupProfileManager.cpp	\
	BackupProfileWidget.cpp		\
	File.cpp			\
	FileInfoWidget.cpp		\
	ImageCache.cpp			\
	IndexDlg.cpp			\
	InfoShellWidget.cpp		\
	KDat.cpp			\
	LoggerWidget.cpp		\
	Node.cpp			\
	Options.cpp			\
	OptionsDlg.cpp			\
	Range.cpp			\
	Tape.cpp			\
	TapeDrive.cpp			\
	TapeFileInfoWidget.cpp		\
	TapeInfoWidget.cpp		\
	TapeManager.cpp			\
	FormatOptDlg.cpp		\
	TarParser.cpp			\
	Util.cpp			\
	VerifyDlg.cpp			\
	VerifyOptDlg.cpp		\
	ktreeview.cpp			\
	main.cpp


# the library search path
kdat_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kdat_LDADD   = $(LIB_KFILE)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS =			\
	Archive.h			\
	ArchiveInfoWidget.h		\
	BackupDlg.h			\
	BackupOptDlg.h			\
	BackupProfile.h			\
	BackupProfileInfoWidget.h	\
	BackupProfileManager.h		\
	BackupProfileWidget.h		\
	File.h				\
	FileInfoWidget.h		\
	ImageCache.h			\
	IndexDlg.h			\
	InfoShellWidget.h		\
	KDat.h				\
	LoggerWidget.h			\
	Node.h				\
	Options.h			\
	OptionsDlg.h			\
	Range.h				\
	Tape.h				\
	TapeDrive.h			\
	TapeFileInfoWidget.h		\
	TapeInfoWidget.h		\
	TapeManager.h			\
	FormatOptDlg.h			\
	TarParser.h			\
	Util.h				\
	VerifyDlg.h			\
	VerifyOptDlg.h			\
	kdat.h				\
	ktreeview.h

# just to make sure, automake makes them 
METASOURCES =				\
	ArchiveInfoWidget.moc		\
	BackupDlg.moc			\
	BackupOptDlg.moc		\
	BackupProfileInfoWidget.moc	\
	BackupProfileManager.moc	\
	BackupProfileWidget.moc		\
	FileInfoWidget.moc		\
	IndexDlg.moc			\
	InfoShellWidget.moc		\
	KDat.moc			\
	LoggerWidget.moc		\
	Node.moc			\
	Options.moc			\
	OptionsDlg.moc			\
	TapeDrive.moc			\
	TapeFileInfoWidget.moc		\
	TapeInfoWidget.moc		\
	TapeManager.moc			\
	FormatOptDlg.moc		\
	TarParser.moc			\
	VerifyDlg.moc			\
	VerifyOptDlg.moc		\
	ktreeview.moc

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(DEP_FILES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kdat_SOURCES) && mv messages.po ../po/kdat.pot

kde_icon_DATA = kdat.xpm

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kdat.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/kdat.xpm $(kde_icondir) 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kdat.xpm $(kde_minidir)/kdat.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kdat.xpm $(kde_icondir)/large/kdat.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/kdat.kdelnk
	-rm -f $(kde_icondir)/kdat.xpm
	-rm -f $(kde_minidir)/kdat.xpm
	-rm -f $(kde_icondir)/large/kdat.xpm

