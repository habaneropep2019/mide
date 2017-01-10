## -*- makefile -*-
# Ktalkdlg - Makefile.am

EXTRA_DIST	 =	

if KDE_INSTALLED

bin_PROGRAMS	 =	ktalkdlg
ktalkdlg_SOURCES =	ktalkdlg.cpp

INCLUDES	=	$(all_includes)
LDFLAGS		=	$(all_libraries) $(KDE_RPATH)
LDADD		=	      -lmediatool $(LIBCOMPAT) $(LIB_KDEUI)

endif

#for extra warnings during compilation :
#AM_CXXFLAGS = -ansi -pedantic -D_POSIX_SOURCE -D_BSD_SOURCE

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(ktalkdlg_SOURCES) && mv messages.po ../../po/ktalkd.pot

