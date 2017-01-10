
INCLUDES= $(all_includes) -include $(srcdir)/../kplunger/kplunger.h

ksirclibdir = $(libdir)/ksirc

lib_LTLIBRARIES = libpuke.la

ksirclib_LTLIBRARIES = libpframe.la libplined.la libpbutton.la libppushbt.la \
                  libpprogress.la libptablevw.la libplistbox.la libplabel.la \
                  libpmenudta.la libppopmenu.la libpalistbox.la libptabdialog.la \
                  libksirclistbox.la libpkfiledialog.la libpobjfinder.la

libpuke_la_SOURCES = controller.cpp pwidget.cpp pobject.cpp \
	playout.cpp kdynlib.cpp
libpuke_la_LDFLAGS = -version-info 0:1:0 -export-dynamic

libpframe_la_SOURCES = pframe.cpp 
libpframe_la_LDFLAGS =  -export-dynamic  -nostdlib

libplined_la_SOURCES = plined.cpp
libplined_la_LDFLAGS = -export-dynamic  -nostdlib

libpbutton_la_SOURCES = pbutton.cpp 
libpbutton_la_LDFLAGS = -export-dynamic  -nostdlib

libppushbt_la_SOURCES = ppushbt.cpp 
libppushbt_la_LDFLAGS = -export-dynamic  -nostdlib

libpprogress_la_SOURCES = pprogress.cpp
libpprogress_la_LDFLAGS = -export-dynamic  -nostdlib

libptablevw_la_SOURCES = ptablevw.cpp
libptablevw_la_LDFLAGS = -export-dynamic -nostdlib

libplistbox_la_SOURCES = plistbox.cpp 
libplistbox_la_LDFLAGS = -export-dynamic  -nostdlib

libplabel_la_SOURCES = plabel.cpp
libplabel_la_LDFLAGS = -export-dynamic  -nostdlib

libpmenudta_la_SOURCES = pmenudta.cpp
libpmenudta_la_LDFLAGS = -export-dynamic  -nostdlib

libppopmenu_la_SOURCES = ppopmenu.cpp
libppopmenu_la_LDFLAGS = -export-dynamic  -nostdlib

libpalistbox_la_SOURCES = palistbox.cpp
libpalistbox_la_LDFLAGS = -export-dynamic -nostdlib

libptabdialog_la_SOURCES = ptabdialog.cpp 
libptabdialog_la_LDFLAGS = -export-dynamic  -nostdlib

libksirclistbox_la_SOURCES = ksirclistbox.cpp
libksirclistbox_la_LDFLAGS = -export-dynamic

libpkfiledialog_la_SOURCES = pkfiledialog.cpp
libpkfiledialog_la_LDFLAGS = -export-dynamic

libpobjfinder_la_SOURCES = pobjfinder.cpp
libpobjfinder_la_LDFLAGS = -export-dynamic

$(srcdir)/HOWTO-PUKE.txt: HOWTO-PUKE.pod
	pod2text $(srcdir)/HOWTO-PUKE.pod > HOWTO-PUKE.txt

# no install headers

noinst_HEADERS = \
	commands.h plined.h pwidget.h widgethdlr.h \
	controller.h pframe.h pmessage.h HOWTO-PUKE.pod \
	commands-handler.pl  pboxlayout.pm pwidget.pm \
	commands-perl.pl pframe.pm test.pl convert_commands.pl \
	plined.pm tester.pl pbase.pm puke.pl ppushbt.h \
	pbutton.h pprogress.h ptablevw.h plistbox.h \
	plabel.h pobject.h playout.h pmenudta.h ppopmenu.h \
	kdynlib.h palistbox.h ptabdialog.h ksirclistbox.h

# just to make sure, automake makes them 
METASOURCES = controller.moc ksirclistbox.moc palistbox.moc pbutton.moc \
	pframe.moc plabel.moc playout.moc plined.moc plistbox.moc \
	pmenudta.moc pobject.moc ppopmenu.moc pprogress.moc ppushbt.moc \
	ptabdialog.moc ptablevw.moc pwidget.moc pkfiledialog.moc

# Datafiles to install / uninstall : *.pl *.pm
data_DATA = commands-handler.pl commands-perl.pl convert_commands.pl \
	puke.pl small.pl test.pl tester.pl \
	dcc_progress.pm dcc_status.pm ksirclistbox.pm load_all.pm \
	palistbox.pm pbase.pm pboxlayout.pm pbutton.pm pframe.pm \
	pkfiledialog.pm plabel.pm plined.pm plistbox.pm pmenudta.pm \
	ppopmenu.pm pprogress.pm ppushbt.pm ptabdialog.pm ptablevw.pm \
	pwidget.pm pkfiledialog.pm pobjfinder.pm

# Where to install them
datadir = $(kde_datadir)/ksirc

