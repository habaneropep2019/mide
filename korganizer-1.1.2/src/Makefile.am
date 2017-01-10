# set the include path for X, qt and KDE, and versit library for vCalendar
INCLUDES= $(all_includes) \
	   -I$(srcdir)/versit 

# this gives better compile-time warnings.
CXXFLAGS += -pedantic

bin_PROGRAMS = 	korganizer alarmd

LDFLAGS = $(all_libraries) $(KDE_RPATH)
korganizer_LDADD   = versit/libversit.a $(LIB_KFILE) -lkab
alarmd_LDADD =  versit/libversit.a $(LIB_MEDIATOOL)

########## FILES

# Which sources should be compiled for korganizer.
korganizer_SOURCES	=  calobject.cpp catdlg.cpp editeventwin.cpp \
	eventwidget.cpp eventwingeneral.cpp eventwindetails.cpp \
	eventwinrecur.cpp kagenda.cpp kdated.cpp kdatenav.cpp \
	kdpdatebutton.cpp kmonthview.cpp kdpevent.cpp kpbutton.cpp \
	kdpquickevt.cpp ksellabel.cpp \
	ktimed.cpp baselistview.cpp listview.cpp main.cpp  \
	searchdialog.cpp topwidget.cpp quickdialog.cpp \
	calprinter.cpp kptbutton.cpp \
	koarchivedlg.cpp optionsdlg.cpp kpropdlg.cpp kpropcheck.cpp \
	kpropcombo.cpp kpropradio.cpp kpropfont.cpp \
	kpropcolor.cpp kpropspin.cpp kproptext.cpp \
	scanholiday.c parseholiday.c aboutdlg.cpp \
	vcaldrag.cpp kpropgroup.cpp komailclient.cpp kotodoview.cpp \
	dateedit.cpp datevalidator.cpp todoeventwin.cpp eventwin.cpp \
        wingeneral.cpp todowingeneral.cpp

alarmd_SOURCES = alarmdaemon.cpp alarmmain.cpp calobject.cpp kdpevent.cpp \
	scanholiday.c parseholiday.c vcaldrag.cpp

METASOURCES =	editeventwin.moc calobject.moc kdpevent.moc \
	kdatenav.moc eventwidget.moc kagenda.moc \
	kpbutton.moc kdpdatebutton.moc \
	eventwingeneral.moc eventwindetails.moc eventwinrecur.moc \
	catdlg.moc ktimed.moc kdated.moc \
	baselistview.moc listview.moc topwidget.moc quickdialog.moc \
	ksellabel.moc kmonthview.moc kdpquickevt.moc optionsdlg.moc \
	searchdialog.moc alarmdaemon.moc \
	calprinter.moc kptbutton.moc \
	koarchivedlg.moc kpropdlg.moc kpropcheck.moc \
	kpropcombo.moc kpropradio.moc kproptext.moc kpropfont.moc \
	kpropcolor.moc kpropspin.moc aboutdlg.moc \
	kpropgroup.moc komailclient.moc kotodoview.moc \
	dateedit.moc datevalidator.moc todoeventwin.moc eventwin.moc \
	wingeneral.moc todowingeneral.moc


noinst_HEADERS = kmonthview.h misc.h calobject.h \
	kpbutton.h optionsdlg.h catdlg.h kagenda.h kptbutton.h \
	quickdialog.h editeventwin.h kdated.h ksellabel.h \
	eventwidget.h kdatenav.h ksmalldaydisp.h \
	topwidget.h eventwindetails.h kdpdatebutton.h ktimed.h \
	version.h eventwingeneral.h kdpevent.h baselistview.h listview.h \
	eventwinrecur.h kdpquickevt.h \
	searchdialog.h calprinter.h kptbutton.h \
	koarchivedlg.h kpropdlg.h kpropcheck.h \
	kpropcombo.h kpropradio.h kproptext.h kpropfont.h \
	kpropcolor.h kpropspin.h parseholiday.h aboutdlg.h \
	vcaldrag.h kpropgroup.h komailclient.h kotodoview.h \
	dateedit.h datevalidator.h todoeventwin.h eventwin.h \
        wingeneral.h todowingeneral.h


SUBDIRS = versit \
	  pixmaps \
	  holidays \
          doc \
	  sounds


datadir = $(kde_datadir)/korganizer
data_SCRIPTS = ical2vcal

kde_conf_DATA = korganizerrc

kdelnkdir = $(kde_appsdir)/Applications
kdelnk_DATA = korganizer.kdelnk

mimedir = $(kde_mimedir)/text
mime_DATA = x-vcalendar.kdelnk

install-data-local:
	$(mkinstalldirs) $(datadir)/sounds

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot \
		$(korganizer_SOURCES) $(alarmd_SOURCES) && \
		mv messages.po ../po/korganizer.pot

$(srcdir)/parseholiday.c: parseholiday.y
	cd $(srcdir) && bison -d -o parseholiday.c parseholiday.y

$(srcdir)/scanholiday.c: scanholiday.lex parseholiday.h
	cd $(srcdir) && flex -B -oscanholiday.c scanholiday.lex

