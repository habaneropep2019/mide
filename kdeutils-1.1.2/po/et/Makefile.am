
# This has to be set. The rest should be reusable
GMOFILES = ark.gmo kab.gmo karm.gmo kcalc.gmo kedit.gmo kfloppy.gmo \
	   khexdit.gmo kjots.gmo kljettool.gmo knotes.gmo kwrite.gmo \
	   klpq.gmo klipper.gmo

POFILES = $(GMOFILES:.gmo=.po)

LANG = et

localedir = $(kde_locale)

GMSGFMT = @GMSGFMT@
EXTRA_DIST = $(POFILES)

all: all-@USE_NLS@
install-data-local: install-@USE_NLS@

SUFFIXES = .po .gmo

.po.gmo:
	@file=`echo $* | sed 's,.*/,,'`.gmo; \
	echo $(GMSGFMT) -o $$file $< ;\
	rm -f $$file && $(GMSGFMT) -o $$file $<

all-yes: $(GMOFILES)
all-no:

install-yes: all
	@catalogs='$(GMOFILES)'; \
	for cat in $$catalogs; do \
	destdir=$(localedir); \
	name=`echo $$cat | sed -e 's/\.gmo/\.mo/'` \
	dir=$$destdir/$(LANG)/LC_MESSAGES; \
	$(mkinstalldirs) $$dir; \
	$(INSTALL_DATA) $$cat $$dir/$$name; \
	  echo "installing $$cat as $$dir/$$name"; \
	done

install-no:

uninstall-local:
	@catalogs='$(GMOFILES)'; \
	for cat in $$catalogs; do \
	destdir=$(localedir); \
	name=`echo $$cat | sed -e 's/\.gmo/\.mo/'` \
	dir=$$destdir/$(LANG)/LC_MESSAGES; \
	rm -f $$cat $$dir/$$name; \
	echo "removing $$dir/$$name" ; \
	done

dist-hook: $(GMOFILES)
	cp $(GMOFILES) $(distdir)

