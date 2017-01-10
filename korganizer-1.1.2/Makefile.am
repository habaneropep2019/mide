# This file you have to edit. Change the name here
SUBDIRS = src po

# not a GNU package. You can remove this line, if 
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = no-dependencies

VERSION=$(shell awk '/^Version:/ { print $$2 }' korganizer.spec)
CVSTAG = v$(subst .,-,$(VERSION))

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4 

messages:
	rm -rf po.backup
	mkdir po.backup
	for i in `ls -1 po/*.pot | sed -e "s#po/##"`; do \
	  grep -v "\"POT-Creation" po/$$i > po.backup/$$i ; \
	  cp po/$$i po.backup/backup_$$i ;  \
	done
	files=`ls -1`; \
	dirs=`for i in $$files; do if test -d $$i && test -r $$i/Makefile.am; then echo $$i; fi; done`; \
	for subdir in $$dirs; do \
	  echo "Making messages in $$subdir"; \
	  (cd $$subdir && $(MAKE) -f Makefile.am messages) \
	done
	for  i in `ls -1 po/*.pot | sed -e "s#po/##"`; do \
	  grep -v "POT-Creation-Date:" po/$$i > temp.pot ;\
	  if diff -q temp.pot po.backup/$$i; then \
	     echo "I'm restoring $$i" ;\
	     mv po.backup/backup_$$i po/$$i; \
	   else \
	     echo "will leave $$i"; \
	fi ;\
	done
	rm -f temp.pot
	rm -rf po.backup
	(cd po && $(MAKE) -f Makefile.am merge)

archive:
	cvs tag -F $(CVSTAG) .
	@rm -rf /tmp/korganizer-$(VERSION) /tmp/korganizer
	@cd /tmp; cvs export -r$(CVSTAG) korganizer
	@cd /tmp/korganizer; make -f Makefile.cvs
	@mv /tmp/korganizer /tmp/korganizer-$(VERSION)
	@dir=$$PWD; cd /tmp; tar cvzf $$dir/korganizer-$(VERSION).tar.gz \
	 korganizer-$(VERSION)
	@rm -rf /tmp/korganizer-$(VERSION)
	@echo "The archive is in korganizer-$(VERSION).tar.gz"

distclean-local:
	-find . -name .deps | xargs rm -rf
