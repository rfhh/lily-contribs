# Build header files to export

all::$(EXPORT_HEADERS)

clean::

mostlyclean::

distclean:: clean

maintainer-clean:: clean

install:: all
	@for i in $(EXPORT_HEADERS);\
	do (\
		echo "Installing $$i in $(includedir)" ;\
		$(INSTALL_DATA) $$i $(includedir);\
	   ); done

uninstall::
	@for i in $(EXPORT_HEADERS);\
	do (\
		echo "UNInstalling $$i from $(includedir)" ;\
		$(RM) $(includedir)/$$i ;\
	   ); done

depend::

include depend
