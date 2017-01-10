# This has to be set.  The rest should be reusable
POFILES = cs.po da.po de.po es.po fi.po fr.po is.po ja.po \
	  no.po pl.po pt_BR.po

GMOFILES = $(POFILES:.po=.gmo) 
PACKAGE = korganizer

localedir = $(kde_locale)

EXTRA_DIST = $(PACKAGE).pot $(POFILES) 

MAINTAINERCLEANFILES = $(GMOFILES)

messages:

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
	lang=`echo $$cat | sed 's/\.gmo//'`; \
	name=$(PACKAGE).mo \
	dir=$$destdir/$$lang/LC_MESSAGES; \
	$(mkinstalldirs) $$dir; \
	$(INSTALL_DATA) $$cat $$dir/$$name; \
	  echo "installing $$cat as $$dir/$$name"; \
	done

install-no:

uninstall-local:
	@catalogs='$(GMOFILES)'; \
	for cat in $$catalogs; do \
	destdir=$(localedir); \
	lang=`echo $$cat | sed 's/\.gmo//'`; \
	name=$(PACKAGE).mo; \
	dir=$$destdir/$$lang/LC_MESSAGES; \
	rm -f $$cat $$dir/$$name; \
	echo "removing $$dir/$$name" ; \
	done

merge:
	@catalogs='$(POFILES)'; \
	for cat in $$catalogs; do \
	echo $$cat $$name; \
	msgmerge -o $$cat.new $$cat $(PACKAGE).pot ; \
	if test -s $$cat.new; then \
	  grep -v "\"POT-Creation" $$cat.new > $$cat.new.2 ; \
          grep -v "\"POT-Creation" $$cat >> $$cat.new.1; \
	  if diff $$cat.new.1 $$cat.new.2; then \
		rm $$cat.new;  \
	  else  \
		mv $$cat.new $$cat ; \
	fi; \
	rm -f $$cat.new.1 $$cat.new.2 ;\
	fi ;\
	done

dist-hook: $(GMOFILES)
	cp $(GMOFILES) $(distdir)

