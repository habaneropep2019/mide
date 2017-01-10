SUBDIRS = $(TOPSUBDIRS) 

acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in $(top_srcdir)/jsinclude.m4.in
	@cat $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in $(top_srcdir)/jsinclude.m4.in > acinclude.m4

messages:

AUTOMAKE_OPTIONS = foreign 1.4
