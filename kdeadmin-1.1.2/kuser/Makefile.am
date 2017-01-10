INCLUDES	=	$(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
AM_CXXFLAGS = 	$(KU_NIS) $(KU_SHADOW) $(KU_QUOTA) $(QUOTACTL) -D_KU_DEBUG

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	$(LIB_KDEUI) $(LIBCRYPT) $(PAMLIBS) $(LIBPWDB) $(LIBGEN)

####### Files

kuser_SOURCES	=	\
	mainWidget.cpp editGroup.cpp editGroupData.cpp kuservw.cpp \
	kheader.cpp krowtable.cpp kusertbl.cpp kgrouptbl.cpp kgroupvw.cpp \
	kdatectl.cpp kuser.cpp kgroup.cpp main.cpp maindlg.cpp mnt.cpp \
	propdlg.cpp quota.cpp misc.cpp usernamedlg.cpp pwddlg.cpp \
	grpnamedlg.cpp kerror.cpp addUser.cpp editDefaults.cpp \
	userDefaultsPage.cpp userDefaultsPageData.cpp

METASOURCES =	\
	mainWidget.moc editGroup.moc editGroupData.moc kuservw.moc kheader.moc \
	krowtable.moc kusertbl.moc kgrouptbl.moc kgroupvw.moc kdatectl.moc \
	maindlg.moc propdlg.moc usernamedlg.moc pwddlg.moc grpnamedlg.moc \
	addUser.moc userDefaultsPageData.moc userDefaultsPage.moc \
	editDefaults.moc

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=	\
	mainWidget.h editGroup.h editGroupData.h kuservw.h kheader.h \
	krowtable.h kusertbl.h kdatectl.h kuser.h maindlg.h kgroup.h \
	kgrouptbl.h kgroupvw.h propdlg.h mnt.h quota.h globals.h misc.h \
	usernamedlg.h pwddlg.h grpnamedlg.h kerror.h addUser.h \
	userDefaultsPageData.h userDefaultsPage.h editDefaults.h

EDITABLE	=	$(DISTFILES) $(noinst_HEADERS) $(kuser_SOURCES) Makefile.am

EXTRA_DIST = README AUTHORS THANKS kuser.kdelnk

# the programs get installed in $(prefix)/bin
bin_PROGRAMS = kuser

# the subdirs we have. Better would be to include pic here too
SUBDIRS = doc pic toolbar icon mini-icon lsm

# only install in your own directories
install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/System
	$(INSTALL_DATA) $(srcdir)/kuser.kdelnk $(kde_appsdir)/System/
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kuser.xpm $(kde_icondir)/large/kuser.xpm

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kuser_SOURCES) && mv messages.po ../po/kuser.pot

pot::
	/usr/local/bin/xgettext -ki18n -o kuser.pot -p ../po -d kuser -C -n *.cpp
	/usr/local/bin/msgmerge --strict -v -o ../po/ru/kuser.po.new ../po/ru/kuser.po ../po/kuser.pot
	mv ../po/ru/kuser.po.new ../po/ru/kuser.po

