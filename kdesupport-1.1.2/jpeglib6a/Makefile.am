INCLUDES=-I.
lib_LTLIBRARIES = libjpeg.la

include_HEADERS =  \
	jchuff.h jdhuff.h jdct.h jerror.h jinclude.h jmemsys.h jmorecfg.h \
        jpegint.h jpeglib.h jversion.h cdjpeg.h cderror.h jconfig.h

libjpeg_la_LDFLAGS = -version-info 6:1 

# source files: JPEG library proper
libjpeg_la_SOURCES = \
	jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c \
        jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c \
        jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c \
        jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c \
        jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c \
        jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c \
        jfdctint.c jidctflt.c jidctfst.c jidctint.c jidctred.c jquant1.c \
        jquant2.c jutils.c jmemmgr.c jmemansi.c

# memmgr back ends: compile only one of these into a working library
# SYSDEPSOURCES= jmemansi.c jmemname.c jmemnobs.c jmemdos.c jmemmac.c

