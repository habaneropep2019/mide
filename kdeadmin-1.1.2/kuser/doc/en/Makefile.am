
# add here all files
data_DATA = logotp3.gif index.html index-1.html index-2.html index-3.html \
	index-4.html index-5.html

# this is the directory, where all datas are installed
# you have given the datas in data_DATA
datadir = $(kde_htmldir)/en/kuser

EXTRA_DIST = $(data_DATA)

doc: index.sgml
	ksgml2html index.sgml en
