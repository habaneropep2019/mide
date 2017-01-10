
SUBDIRS = da en

DISTCLEANFILES = Makefile *~

docs:
	@for subdir in $(SUBDIRS); do \
	  echo "Making docs in $(subdir)"; \
	  (cd $$subdir && $(MAKE) docs); \
	done












