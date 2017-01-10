## -*- makefile -*-
# Ktalkd - Makefile.am

SUBDIRS 	=	machines

bin_PROGRAMS	=	ktalkd
EXTRA_HEADERS	=	readcfg++.h
INCLUDES	=	$(all_includes)
noinst_HEADERS	=	defs.h includ.h find_user.h announce.h print.h process.h readconf.h table.h threads.h options.h unixsock.h
ktalkd_DEPS	=	machines/libmach.a

#for extra warnings during compilation :
#AM_CXXFLAGS =	-ansi -pedantic -D_POSIX_SOURCE -D_BSD_SOURCE

###########################################################
if KDE_INSTALLED

# Config file location
TALKD_CONF	=	$(kde_confdir)/ktalkdrc
TALKD_CONF_NAME	=	ktalkdrc
AM_CPPFLAGS 	=      -DHAVE_KDE
ktalkd_SOURCES	=	announce.cpp print.cpp process.cpp readcfg++.cpp table.cpp talkd.cpp find_user.cpp threads.cpp options.cpp unixsock.cpp
LDADD           =       machines/libmach.a $(LIBBSD) $(LIB_KDECORE)

###########################################################
else

# Config file location
TALKD_CONF 	=	/etc/talkd.conf
TALKD_CONF_NAME	=	talkd.conf
AM_CPPFLAGS =	-DTALKD_CONF=\"$(TALKD_CONF)\"
ktalkd_SOURCES	=	announce.cpp print.cpp process.cpp readconf.cpp table.cpp talkd.cpp find_user.cpp threads.cpp options.cpp
LDADD		=	machines/libmach.a $(LIBBSD) $(LIBSOCKET)

endif
###########################################################

LDFLAGS         =       $(all_libraries) $(KDE_RPATH)

EXTRA_DIST	=	.talkdrc talkd.conf ktalkdrc

install-data-local:
if KDE_INSTALLED
	$(mkinstalldirs) $(kde_confdir)
	$(mkinstalldirs) $(kde_sounddir)
	$(INSTALL_DATA) $(srcdir)/ktalkd.wav $(kde_sounddir)
endif
	@echo "**************************************************************************"
	@echo
	@if [ -f $(TALKD_CONF) ]; then \
	  echo "Please check $(TALKD_CONF) to be up-to-date."; \
	else \
	  $(INSTALL_DATA) $(srcdir)/$(TALKD_CONF_NAME) $(TALKD_CONF); \
	fi
