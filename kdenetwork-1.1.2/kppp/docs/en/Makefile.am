# add here all files
pkg_DATA = kppp.html kppp-1.html kppp-2.html kppp-3.html \
	   kppp-4.html kppp-5.html kppp-6.html kppp-7.html \
	   kppp-8.html kppp-9.html kppp-10.html \
	   hayes.html logotp3.gif  

# this is the directory, where all datas are installed
# you have given the datas in data_DATA
pkgdir = $(kde_htmldir)/en/kppp

EXTRA_DIST = $(pkg_DATA)

install-data-local:
	rm -f $(pkgdir)/index.html
	$(LN_S) kppp.html $(pkgdir)/index.html

docs: kppp.sgml
	ksgml2html kppp.sgml
