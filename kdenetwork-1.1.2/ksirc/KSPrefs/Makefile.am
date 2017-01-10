
INCLUDES= $(all_includes) -include $(srcdir)/../kplunger/kplunger.h

####### This part is very ksirc specific
# you can add here more. This one gets installed 
noinst_LIBRARIES = libksprefs.a

libksprefs_a_SOURCES = ksprefs.cpp general.cpp generalData.cpp \
	startup.cpp startupdata.cpp serverchannel.cpp \
	serverchanneldata.cpp UserMenuRef.cpp UserMenuRefData.cpp \
	defaultfilters.cpp defaultfiltersdata.cpp 

noinst_HEADERS = ksprefs.h serverchanneldata.h generalData.h \
	startupdata.h serverchannel.h general.h startup.h \
	UserMenuRef.h UserMenuRefData.h defaultfilters.h \
	defaultfiltersdata.h

libksprefs_a_METASOURCES = USE_AUTOMOC

