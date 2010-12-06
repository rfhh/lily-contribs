
all::;		@for i in $(SUBDIRS);\
		do (\
			echo Making $$i ...;\
			cd $$i;\
			$(MAKE) all \
		); done

clean::;		@for i in $(SUBDIRS);\
		do (\
			echo Cleaning $$i ...;\
			cd $$i;\
			$(MAKE) clean\
		); done

mostlyclean::;	@for i in $(SUBDIRS);\
		do (\
			echo MostlyCleaning $$i ...;\
			cd $$i;\
			$(MAKE) mostlyclean\
		); done

distclean::;	@for i in $(SUBDIRS);\
		do (\
			echo DistCleaning $$i ...;\
			cd $$i;\
			$(MAKE) distclean\
		); done

maintainer-clean::;	@for i in $(SUBDIRS);\
		do (\
			echo Maintainer-Cleaning $$i ...;\
			cd $$i;\
			$(MAKE) maintainer-clean\
		); done

depend::;	@for i in $(SUBDIRS);\
		do (\
			echo Creating dependencies for $$i ...;\
			cd $$i;\
			$(MAKE)  depend\
		); done

install::;	@for i in $(SUBDIRS);\
		do (\
			echo Installing $$i ...;\
			cd $$i;\
			$(MAKE) install\
		); done


