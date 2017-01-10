
pics_DATA = eject.xbm ff.xbm kmedia.xpm kmedia_mini.xpm lock.xbm \
	logo.xbm nexttrk.xbm playpause.xbm playpause_both.xpm \
	playpause_play.xpm poweroff.xbm prevtrk.xbm rew.xbm stop.xbm

picsdir = $(kde_datadir)/kmedia/pics

EXTRA_DIST = $(pics_DATA) kmedia_mini.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/kmedia_mini.xpm $(kde_minidir)/kmedia.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-kmedia.xpm $(kde_icondir)/large/kmedia.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kmedia.xpm
	-rm -f $(kde_icondir)/large/kmedia.xpm

icon_DATA = kmedia.xpm
icondir = $(kde_icondir)
