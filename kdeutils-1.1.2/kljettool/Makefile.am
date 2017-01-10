

LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD   = $(LIB_KDECORE) -lm
INCLUDES= $(all_includes)

####### Files

bin_PROGRAMS = kljettool
noinst_HEADERS = Paper.h PaperData.h  Fonts.h FontsData.h Printer.h \
	PrinterData.h Misc.h MiscData.h Data.h main.h about.h

kljettool_SOURCES = main.cpp Paper.cpp PaperData.cpp Fonts.cpp \
	FontsData.cpp Printer.cpp PrinterData.cpp Misc.cpp MiscData.cpp \
	Data.cpp about.cpp

METASOURCES =	Paper.moc PaperData.moc Fonts.moc FontsData.moc Printer.moc\
		PrinterData.moc	Misc.moc MiscData.moc Data.moc main.moc \
		about.moc

SUBDIRS = docs support 

pics_DATA = kljetlogo.xpm
picsdir = $(kde_icondir)

apps_DATA = KLJetTool.kdelnk
appsdir = $(kde_appsdir)/Utilities/

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kljettool.xpm $(kde_minidir)/kljettool.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kljettool.xpm $(kde_icondir)/large/kljettool.xpm
	@echo ""
	@echo ""
	@echo "If you own a RedHat system or if you have trouble getting"
	@echo "KLJetTool to work on your system, you must read the README"
	@echo "file in the directory support."
	@echo ""
	@echo "thanks,"
	@echo "Bernd Wuebben"
	@echo "wuebben@kde.org"
	@echo ""
	
uninstall-local:
	rm -f $(kde_minidir)/kljettool.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kljettool_SOURCES) && mv messages.po ../po/kljettool.pot

