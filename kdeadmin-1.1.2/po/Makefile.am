 
SUBDIRS = ru de fi es hu pl pt cs it sk hr nl no fr pt_BR zh_CN.GB2312 \
	eo sv ko tr is da br el sl ja

# this is a general list for merging!
APPS = kuser ksysv kdat

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


