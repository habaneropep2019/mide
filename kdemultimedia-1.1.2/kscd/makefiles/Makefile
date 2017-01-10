
# begin LINUX +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# Uncomment the following line to have WorkMan send SCSI commands directly to
# the CD-ROM drive.  If you have a SCSI drive you probably want this, but it
# will cause WorkMan to not work on IDE drives.
#
#PASS = -DLINUX_SCSI_PASSTHROUGH
#
# Uncomment the following if you want fast database lookups using the
# Berkeley libdb code.  You can find libdb on ftp.cs.berkeley.edu in the
# /ucb/4bsd directory.  Make sure you adjust the last part of DBCFLAGS
# to match your system type as listed in libdb's PORT directory.
#

# THIS DOENS'T LOOK VERY GOOD CHECK THIS STUFF !!!!!!! BERND

#DBCFLAGS = -DLIBDB -Idb.1.85/PORT/include -Idb.1.85/PORT/linux
#DBLIB = db.1.85/PORT/linux/libdb.a

# end LINUX +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

INC=-I$(QTDIR)/include -I$(KDEDIR)/include

QTLIB=-lqt 
XLIB=-L/usr/X11/lib -lX11
KLIBS=-L$(KDEDIR)/lib -lkdecore

CC=gcc

CFLAGS=-O2  -Dlinux $(PASS)

###### File Section

METASRC=mkscd.h 
SRC=	kscd.cpp  cdinfo.c\
	plat_sun.c plat_hpux.c plat_linux.c plat_news.c \
	plat_bsd386.c plat_ultrix.c cdrom.c scsi.c drv_toshiba.c drv_sony.c \
	plat_svr4.c plat_freebsd.c plat_osf1.c bwlednum/bwlednum.cpp

OBJS=kscd.o  cdinfo.o \
	plat_sun.o plat_hpux.o plat_linux.o plat_news.o \
	plat_bsd386.o plat_ultrix.o cdrom.o scsi.o drv_toshiba.o drv_sony.o \
	plat_svr4.o plat_freebsd.o plat_osf1.o bwlednum/bwlednum.o

METAOBJS=mkscd.o 
HEADERS=kscd.h struct.h bwlednum/bwlednum.h
TARGET=kcdplayer 
STATIC=

###### Rules

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $(CFLAGS) $(INC) $<

###### Compilation

all: $(TARGET)

static: $(STATIC)

$(TARGET): $(OBJS) $(SRC) $(MEATSRC) $(METAOBJS)
#	cd ../qled; make; cd ../kcdplayer
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS)  $(METAOBJS) $(QTLIB) $(XLIB) $(KLIBS)

#$(STATIC): $(OBJS) $(SRC) $(MEATSRC)
#	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) /usr/local/lib/libqt.a $(XLIB)

mkscd.cpp: kscd.h
	moc -o $@ $<

###### Miscellaneous

clean: 
	rm -f *.o *~ core $(METASRC) $(TARGET) $(OBJS) $(METAOBJS)

