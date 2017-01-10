
INCLUDES = $(all_includes)
AM_CPPFLAGS = -DKRN -DCHARSETS

LDFLAGS = $(all_libraries) $(KDE_RPATH)
krn_LDADD =  -lkspell  $(LIB_KHTMLW)  $(LIB_KFILE) \
	 -lmimelib -lgdbm $(LIBCOMPAT) 

kdecode_LDADD =    $(LIB_KFILE) \
	 $(LIBUU) $(LIBCOMPAT) 

newkrn_LDADD =   $(LIB_KHTMLW)  $(LIB_KFILE) \
	 -lmimelib -lgdbm $(LIBCOMPAT) 


MINIDIR =  $(kde_minidir)
# better would be Internet
APPSDIR = $(kde_appsdir)/Internet

METASOURCES =	groupdlg.moc \
		NNTP.moc \
                artdlg.moc\
                asker.moc\
                identDlg.moc\
                NNTPConfigDlg.moc\
                rmbpop.moc\
                decoderDlg.moc\
                kdecode.moc\
                aboutDlg.moc\
                fontsDlg.moc\
                kmcomposewin.moc\
                findArtDlg.moc\
                kmmsgpartdlg.moc\
                kbusyptr.moc\
                kalarmtimer.moc\
                kmreaderwin.moc\
                kpgp.moc\
                krnsender.moc\
                kmsender.moc\
                kmaddrbookdlg.moc\
                ktreeview.moc\
                expireDlg.moc\
                printdlg.moc\
                rulesDlg.moc\
                sortDlg.moc\
		charsetsDlg.moc\
                expirestatusdlg.moc\
                kmtopwidget.moc\
                TooManydlg.moc \
		new_groupdlg.moc

bin_PROGRAMS =	krn kdecode newkrn

newkrn_SOURCES = new_main.cpp\
		 new_groupdlg.cpp\
                 new_globals.cpp\
                 kfileio.cpp

kdecode_SOURCES = kdecodemain.cpp\
                decoderDlg.cpp\
                kdecode.cpp

krn_SOURCES = 	main.cpp\
		NNTP.cpp\
		groupdlg.cpp\
		artdlg.cpp\
                newsgroup.cpp\
                asker.cpp\
                identDlg.cpp\
                NNTPConfigDlg.cpp\
                rmbpop.cpp\
                aboutDlg.cpp\
                ktempfile.cpp\
                kscorer.cpp\
                fontsDlg.cpp\
                kmmessage.cpp\
                kmmsgpart.cpp\
                kmmsginfo.cpp\
                kmcomposewin.cpp\
                kmimemagic.cpp\
                kmsender.cpp\
                findArtDlg.cpp\
                kmmsgbase.cpp\
                kmidentity.cpp\
                kfileio.cpp\
                kmmsgpartdlg.cpp\
                kbusyptr.cpp\
                kalarmtimer.cpp\
                kmreaderwin.cpp\
                kpgp.cpp\
                krnsender.cpp\
                kmaddrbookdlg.cpp\
                kmaddrbook.cpp\
                ktreeview.cpp\
                expireDlg.cpp\
                tlform.cpp\
                tlwidget.cpp\
                typelayout.cpp\
                printdlg.cpp\
                rulesDlg.cpp\
                sortDlg.cpp\
                rules.cpp\
		charsetsDlg.cpp\
                expirestatusdlg.cpp\
                kfontutils.cpp \
                kmtopwidget.cpp\
                kpgpbase.cpp\
		TooManydlg.cpp

noinst_HEADERS = 	NNTP.h\
                kmmessage.h\
		artdlg.h\
                groupdlg.h\
                asker.h\
                identDlg.h\
                NNTPConfigDlg.h\
                decoderDlg.h\
		rmbpop.h\
		kdecode.h\
		aboutDlg.h\
		ktempfile.h\
		kscorer.h\
                fontsDlg.h\
                kmmsgpart.h\
                kmmsginfo.h\
                kmcomposewin.h\
                kmimemagic.h\
                kmsender.h\
                mytree.h\
                findArtDlg.h\
                kmmsgbase.h\
                kmidentity.h\
                kfileio.h\
                kmmsgpartdlg.h\
                kbusyptr.h\
                kalarmtimer.h\
                kmreaderwin.h\
                kpgp.h\
                krnsender.h\
                kmaddrbookdlg.h\
                kmaddrbook.h\
                ktreeview.h\
                expireDlg.h\
                tlform.h\
                tlwidget.h\
                typelayout.h\
                printdlg.h\
                rulesDlg.h\
                sortDlg.h\
                rules.h\
		charsetsDlg.h\
                expirestatusdlg.h\
                kfontutils.h\
                kmtopwidget.h\
		kmiostatusdlg.h\
		kpgpbase.h\
		TooManydlg.h




SUBDIRS = docs pics

EXTRA_DIST = krn.xpm mini-krn.xpm Krn.kdelnk

kde_icon_DATA = krn.xpm

install-data-local:
	$(mkinstalldirs) $(MINIDIR)
	$(INSTALL_DATA) $(srcdir)/mini-krn.xpm $(MINIDIR)/krn.xpm
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/Krn.kdelnk $(APPSDIR)

uninstall-local:
	rm -f $(MINIDIR)/krn.xpm
	rm -f $(APPSDIR)/Krn.kdelnk

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(krn_SOURCES) && mv messages.po ../po/krn.pot
