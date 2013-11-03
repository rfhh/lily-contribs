# Build a library

LIB_FILENAME=$(EXPORT_LIBRARY)$(LIBRARY)$(LIB_SUFFIX)
LIB_TARGET=$(pkg_libdir)/$(LIB_FILENAME)

all::$(LIB_TARGET)

$(LIB_TARGET): $(OBJS)
	: $(RM) $(LIB_TARGET)
	@echo Loading library $(LIB_TARGET)
	$(AR) $(LIB_TARGET) $(OBJS)
	$(RANLIB) $(LIB_TARGET)

clean::
	$(RM) $(LIB_TARGET)
	$(RM) $(OBJS)

mostlyclean::
	$(RM) $(OBJS)

distclean:: clean

maintainer-clean:: clean

install:: all
	@if [ "$(EXPORT_LIBRARY)" ] ; then \
		echo "Installing $(LIB_TARGET) in $(libdir)" ; \
		$(INSTALL_DATA) $(LIB_TARGET) $(libdir) ; \
	fi

uninstall::
	@if [ "$(EXPORT_LIBRARY)" ] ; then \
		echo "UNInstalling $(LIB_FILENAME) from $(libdir)" ; \
		$(RM) $(libdir)/$(LIB_FILENAME) ; \
	fi

depend::
	$(MKDEP) $(ALL_CPPFLAGS) $(ALL_CFLAGS) $(SRCS) > depend

include depend

