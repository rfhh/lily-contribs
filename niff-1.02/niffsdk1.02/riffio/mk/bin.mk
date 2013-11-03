# Build an executable

BIN_TARGET=$(pkg_bindir)/$(EXPORT_PROGRAM)$(PROGRAM)$(BIN_SUFFIX)

all::$(BIN_TARGET)

$(BIN_TARGET): $(OBJS) $(DEPLIBS)
	: $(RM) $(BIN_TARGET)
	@echo Linking program $(BIN_TARGET)
	$(CC) -o $(BIN_TARGET) $(OBJS) $(ALL_LDFLAGS) $(ALL_LIBS)

clean::
	$(RM) $(BIN_TARGET)
	$(RM) $(OBJS)

mostlyclean::
	$(RM) $(OBJS)

distclean:: clean

maintainer-clean:: clean

install:: all
	@if [ "$(EXPORT_PROGRAM)" ] ; then \
		echo "Installing $(BIN_TARGET) in $(bindir)" ; \
		$(INSTALL_PROGRAM) $(BIN_TARGET) $(bindir) ; \
	fi

uninstall::
	@if [ "$(EXPORT_PROGRAM)" ] ; then \
	    	echo \
		"UNInstalling $(EXPORT_PROGRAM)$(BIN_SUFFIX) from $(bindir)";\
		$(RM) $(bindir)/$(EXPORT_PROGRAM)$(BIN_SUFFIX) ; \
	fi

depend::
	$(MKDEP) $(ALL_CPPFLAGS) $(ALL_CFLAGS) $(SRCS) > depend

include depend

