## ############################################################################
## This Makefile.am is intended to be generic for standard (small) applications
## contact hadacek@kde.org for comments/suggestions

## ######################
## # Building definitions

bin_PROGRAMS = ksirtet
INCLUDES = $(all_includes)
ksirtet_LDFLAGS = $(all_libraries) $(KDE_RPATH)
ksirtet_LDADD =       $(LIB_KDEUI)

ksirtet_SOURCES= gtetris.cpp tpiece.cpp spiece.cpp gtboard.cpp board.cpp \
				net.cpp dialogs.cpp field.cpp main.cpp dbutton.cpp
METASOURCES = spiece.moc board.moc dialogs.moc field.moc main.moc

noinst_HEADERS = board.h ggboard.h spiece.h dbutton.h gtboard.h tpiece.h \
	defines.h gtetris.h version.h dialogs.h main.h field.h net.h

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(ksirtet_SOURCES) && mv messages.po ../po/ksirtet.pot

SUBDIRS = data
