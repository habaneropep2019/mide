
SUBDIRS = br de da fr cs nl sv es ru hu it pl pt fi hr el ro no \
	zh_TW.Big5 zh_CN.GB2312 sk mk ca pt_BR eo ko tr he et is hs ja sl

# this is a general list for merging!
APPS = kab karm kcalc kedit kfloppy khexdit kjots kljettool knotes kpm ark klpq kwrite klipper ktop

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
