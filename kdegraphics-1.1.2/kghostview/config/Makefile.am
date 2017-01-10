 
APPSDIR = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)


bin_PROGRAMS = 	kcmkeys
kcmkeys_SOURCES = main.cpp standard.cpp global.cpp \
 kaccel.cpp kkeydialog.cpp kckey.c kglobalaccel.cpp savescm.cpp 
noinst_HEADERS =  standard.h \
kaccel.h \
kkeydialog.h kckey.h kglobalaccel.h global.h savescm.h
kcmkeys_LDADD = $(LIB_KFM) $(LIB_KDEUI)
kcmkeys_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = standard.moc kkeydialog.moc kglobalaccel.moc global.moc

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kcmkeys_SOURCES) && mv messages.po ../po/kghostview.pot
