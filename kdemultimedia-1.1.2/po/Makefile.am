
SUBDIRS = br cs da de et fr pl fi es pt ro it zh_TW.Big5 ja \
	zh_CN.GB2312 hr sk nl no pt_BR sv is eo ko tr ca hu el sl

APPS = kmedia kmix kmid kmidi kscd

merge:
	@apps='$(APPS)'; \
	langs='$(SUBDIRS)'; \
	for cat in $$apps; do \
	  for lang in $$langs; do \
	    if test -r $$lang/$$cat.po; then \
	      echo $$cat; \
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

