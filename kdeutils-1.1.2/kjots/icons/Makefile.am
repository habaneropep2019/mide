
pics_DATA = OpenBook.xpm filedel.xpm
picsdir = $(kde_datadir)/kjots/pics

icon_DATA = kjots.xpm
icondir = $(kde_icondir)

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kjots.xpm $(kde_minidir)/kjots.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kjots.xpm $(kde_icondir)/large/kjots.xpm
	
uninstall-local:
	rm -f $(kde_minidir)/kjots.xpm
	rm -f $(kde_icondir)/large/kjots.xpm
