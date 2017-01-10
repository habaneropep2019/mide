EXTRA_DIST = kppp.xpm mini-kppp.xpm

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile
install-data-local:
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/kppp.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kppp.xpm $(kde_minidir)/kppp.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kppp.xpm $(kde_icondir)/large/kppp.xpm

