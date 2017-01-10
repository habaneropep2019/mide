# $Id: Makefile.am,v 1.42.2.5 1999/09/03 17:36:05 duncan Exp $

SUBDIRS = br da de pt fr fi hu es hr pl cs it ro sk el no nl \
	pt_BR ca zh_CN.GB2312 eo sv mk ko ru tr he is et hs ja sl

# this is a general list for merging!
APPS = kbiff kcmktalkd kfinger kmail knu kppp kppplogview krn ksirc \
	ktalkd karchie korn

merge:
	@apps='$(APPS)'; \
	langs='$(SUBDIRS)'; \
	for cat in $$apps; do \
	  echo $$cat; \
	  for lang in $$langs; do \
	    echo $$lang/$$cat ; \
	    if test -r $$lang/$$cat.po; then \
	      echo $$lang; \
	      msgmerge -o $$cat.new $$lang/$$cat.po $$cat.pot; \
	      if test -s $$cat.new && diff $$lang/$$cat.po $$cat.new; then \
		rm $$cat.new;  \
	      else  \
		mv $$cat.new $$lang/$$cat.po ; \
	    fi; \
	  fi ;\
	 done ;\
	done

messages:
