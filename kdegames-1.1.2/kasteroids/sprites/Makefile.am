
SUBDIRS = bits missile rock1 rock2 rock3 ship

SHELL = /bin/sh
POVRAY=x-povray  # Must be Povray 3.0
LDLIBS=-lpng -lz -lm

#all: pngtoppmpgm

#clean:
#	rm -f *~ pngtoppmpgm

#distclean: clean
#	rm -f Makefile

FILES = `cd $(srcdir) && find . \( -name "*.ppm" -o -name "*.pbm" \) -print`
spritesdir = $(kde_datadir)/kasteroids/sprites

install-data-local:
	$(mkinstalldirs) $(spritesdir)/rock1
	$(mkinstalldirs) $(spritesdir)/rock2
	$(mkinstalldirs) $(spritesdir)/rock3
	$(mkinstalldirs) $(spritesdir)/ship
	$(mkinstalldirs) $(spritesdir)/bits
	$(mkinstalldirs) $(spritesdir)/missile
	@for file in $(FILES); do \
	 echo $(INSTALL_DATA) $(srcdir)/$$file $(spritesdir)/$$file ;\
	 $(INSTALL_DATA) $(srcdir)/$$file $(spritesdir)/$$file ;\
	done 

uninstall-local:
	for file in $(FILES); do \
	 rm -f $(spritesdir)/$$file ;\
	done
