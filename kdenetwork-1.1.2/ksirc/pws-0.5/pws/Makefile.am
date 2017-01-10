
APPSDIR = $(kde_appsdir)/Internet

INCLUDES= $(all_includes) -include $(srcdir)/../../kplunger/kplunger.h

SUBDIRS = doc pics

####### This part is very pws specific
# you can add here more. This one gets installed 
noinst_LIBRARIES = libpws.a

# Which sources should be compiled for pws.


libpws_a_SOURCES = \
                  pws.cpp\
                  pwswidget.cpp\
                  typelayout.cpp\
                  tlform.cpp\
                  tlwidget.cpp\
                  pages.cpp\
                  kfileio.cpp\
                  addwizard.cpp\
                  server.cpp

# just to make sure, automake makes them 
METASOURCES = pws.moc pwswidget.moc server.moc pages.moc addwizard.moc

kde_icon_DATA = pws.xpm

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-pws.xpm $(kde_minidir)/pws.xpm

uninstall-local:
	-rm -f $(kde_minidir)/pws.xpm

