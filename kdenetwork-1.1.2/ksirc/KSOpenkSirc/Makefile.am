
# set the include path for X, qt and KDE
INCLUDES= $(all_includes) -include $(srcdir)/../kplunger/kplunger.h

####### This part is very pws specific
# you can add here more. This one gets installed 
noinst_LIBRARIES = libksopenksirc.a

# Which sources should be compiled for dialog

libksopenksirc_a_SOURCES = \
	open_ksirc.cpp\
	open_ksircData.cpp\
	serverFileParser.cpp\
        enter_combo.cpp

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = open_ksirc.h open_ksircData.h serverDataType.h serverFileParser.h enter_combo.h

# just to make sure, automake makes them 
METASOURCES = open_ksirc.moc open_ksircData.moc enter_combo.moc

misc_DATA = servers.txt servers.ini
miscdir = $(kde_datadir)/ksirc

