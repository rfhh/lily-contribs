TARGETS=\
all              \
dist             \
clean            \
mostlyclean      \
distclean        \
maintainer-clean \
depend           \
install          \
uninstall        \
depend           \
doc

install:: all installdirs

$(TARGETS)::
		@for i in $(MAKEDIRS);\
		do (\
			echo Making $@ in $$i ...;\
			cd $$i;\
			$(MAKE) $@ \
		); done

installdirs::$(pkg_mkdir)/mkinsdir
	$(pkg_mkdir)/mkinsdir $(INSTALLDIRS)

# This should come last because we can't
# run make immediately after distcleaning or maintainer cleaning
distclean maintainer-clean::
	$(RM) Makefile \
	      config.cache config.log config.status\
	      $(pkg_mkdir)/top.mk $(pkg_mkdir)/mkdep

