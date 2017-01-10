
# add here all files
data_DATA = korganizer.html korganizer-1.html korganizer-2.html \
	korganizer-3.html korganizer-4.html korganizer-5.html \
	korganizer-6.html korganizer-7.html korganizer-8.html \
	korganizer-9.html korganizer-10.html \
	alarmd-panel.gif kagenda-appointment.gif datenavigator.gif \
	logotp3.gif 

# this is the directory, where all datas are installed
# you have given the datas in data_DATA
datadir = $(kde_htmldir)/en/korganizer

html: ${data_DATA}
	ksgml2html korganizer.sgml en

install-data-local:
	-rm -f $(datadir)/index.html
	$(LN_S) korganizer.html $(datadir)/index.html

uninstall-local:
	rm $(datadir)/index.html


