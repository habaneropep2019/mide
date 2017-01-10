
doc_DATA = kview.index kview.html kview-1.html kview-2.html kview-3.html \
	kview-4.html kview-5.html logotp3.gif
docdir = $(kde_htmldir)/en/kview

html: kview.sgml
	sgml2html -img -style nice kview.sgml
	sgmlidx kview.sgml
