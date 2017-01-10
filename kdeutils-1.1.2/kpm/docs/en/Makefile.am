
# add here all files
pkg_DATA = kpm.html kpm-1.html kpm-2.html kpm-3.html kpm-4.html logotp3.gif

# this is the directory, where all datas are installed
# you have given the datas in data_DATA
pkgdir = $(kde_htmldir)/en/kpm

install-data-local:
	-rm -f $(pkgdir)/index.html
	-ln -s kpm.html $(pkgdir)/index.html

