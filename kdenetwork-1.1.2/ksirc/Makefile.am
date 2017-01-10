
# set the include path for X, qt and KDE
INCLUDES= $(all_includes) -include $(srcdir)/kplunger/kplunger.h

# claim, which subdirectories you want to install
SUBDIRS = kplunger KSTicker KSProgress KSCutDialog KSPrefs \
	KSColourPicker KSOpenkSirc pws-0.5 puke img doc

bin_PROGRAMS = ksirc # ${KSIRCSTATIC_PROGRAM}

noinst_PROGRAMS = getdate

EXTRA_PROGRAMS = ksircstatic

# Which sources should be compiled for ksirc.

ksirc_SOURCES = \
	toplevel.cpp \
	ksirc.cpp \
	irclistbox.cpp \
	irclistitem.cpp \
	alistbox.cpp \
	ahistlineedit.cpp \
	iocontroller.cpp \
	open_top.cpp open_topData.cpp \
	messageReceiver.cpp \
	ioBroadcast.cpp \
	ksircprocess.cpp \
	servercontroller.cpp \
	KSircColour.cpp KSircColourData.cpp \
	FilterRuleEditor.cpp FilterRuleEditorData.cpp \
	ioDiscard.cpp \
	ioDCC.cpp dccDialog.cpp dccDialogData.cpp \
	linelistitem.cpp \
	ioLAG.cpp \
	ioNotify.cpp \
	ssfeprompt.cpp ssfepromptdata.cpp \
	usercontrolmenu.cpp \
	kspainter.cpp \
	baserules.cpp \
	estring.cpp \
	chanparser.cpp \
	objFinder.cpp \
	KMDIMgr.cpp KMDIMgrBase.cpp \
	displayMgr.cpp displayMgrMDI.cpp displayMgrSDI.cpp \
	welcomeWin.cpp welcomeWinData.cpp \
	kpopupmenu.cpp

ksircstatic_SOURCES = $(ksirc_SOURCES)

# the library search path
ksirc_LDFLAGS = -export-dynamic $(all_libraries) $(KDE_RPATH)
ksircstatic_LDFLAGS = --nostdlib -static -all-static $(ksirc_LDFLAGS)

all_ksirc_libraries = ./kplunger/libkplunger.a \
	./KSProgress/libksprogress.a \
	./KSTicker/libksticker.a \
	./KSCutDialog/libkscutdialog.a \
	./KSPrefs/libksprefs.a \
	./KSColourPicker/libkscolourpicker.a \
	./KSOpenkSirc/libksopenksirc.a \
	./pws-0.5/pws/libpws.a \
	puke/libpuke.la $(LIB_KFILE) $(LIBDL) 

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
ksirc_LDADD   = $(all_ksirc_libraries)

ksircstatic_LDADD =  $(all_ksirc_libraries)

STATIC_LIBS = $(static_libkde) $(static_libqt) $(static_libstdc) $(static_pthread) -lXext $(LIB_X11) $(LIBDL) -lm -lc

kdoc:
	kdoc -ddoc ksirc *.h

i18n:
	perl conv-i18n-h `find . -name '*Data.h' -print |grep -Ev '.*/m.*'`	
	perl -n -i conv-i18n-cpp `find . -name '*Data.cpp' -print |grep -Ev '.*/m.*'`

getdate_SOURCES = getdate.c

getdate: getdate.c
	$(CC) $(CFLAGS) -o getdate $(srcdir)/getdate.c

cdate.h: getdate
	./getdate > cdate.h

$(srcdir)/ksirc.cpp: cdate.h

# this option you can leave out. Just, if you use "make dist", you need it
#
# DSIRC is on the bottom so make dist includes it!!!!!!!!!
#
noinst_HEADERS = \
FilterRuleEditor.h        control_message.h         linelistitem.h \
FilterRuleEditorData.h    dccDialog.h               messageReceiver.h \
KSircColour.h             dccDialogData.h           \
KSircColourData.h         ioBroadcast.h             \
ioDCC.h                   open_top.h                usercontrolmenu.h \
ioDiscard.h               open_topData.h            kspainter.h \
ahistlineedit.h           iocontroller.h            servercontroller.h \
alistbox.h                irclistbox.h              chanparser.h \
config.h                  irclistitem.h             toplevel.h \
ksircprocess.h            ksirc.kdelnk              cdate.h \
ioLAG.h                   ioNotify.h                ssfeprompt.h \
ssfepromptdata.h          usercontrolmenu.h         objFinder.h \
dsirc                     filters.pl                ksirc.pl \
sirc.help.gz              baserules.h               estring.h \
KMDIMgr.h KMDIMgrBase.h  \
displayMgr.h displayMgrMDI.h displayMgrSDI.h


# just to make sure, automake makes them 
METASOURCES = FilterRuleEditor.moc \
	FilterRuleEditorData.moc \
	KSircColour.moc \
	KSircColourData.moc \
	ahistlineedit.moc \
	alistbox.moc \
	dccDialog.moc \
	dccDialogData.moc \
	ioDCC.moc \
	ioLAG.moc \
	ioNotify.moc \
	iocontroller.moc \
	irclistbox.moc \
	irclistitem.moc \
	ksircprocess.moc \
	objFinder.moc \
	open_top.moc \
	open_topData.moc \
	servercontroller.moc \
	ssfeprompt.moc \
	ssfepromptdata.moc \
	toplevel.moc kpopupmenu.moc \
	welcomeWin.moc welcomeWinData.moc \
	KMDIMgr.moc KMDIMgrBase.moc \
	displayMgr.moc displayMgrMDI.moc displayMgrSDI.moc 

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cpp */*.cpp && mv messages.po ../po/ksirc.pot

bin_SCRIPTS = dsirc

apps_DATA = ksirc.kdelnk
appsdir = $(kde_appsdir)/Internet

misc_DATA = ksirc.pl filters.pl sirc.help.gz relnotes
miscdir = $(kde_datadir)/ksirc
