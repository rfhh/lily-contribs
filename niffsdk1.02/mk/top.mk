# $Id: top.in,v 1.11 1996/06/12 07:24:53 tim Exp $

SHELL=/bin/sh

pkg_mkdir=$(pkg_topdir)/mk
pkg_srcdir=$(pkg_topdir)/src
pkg_incdir=$(pkg_topdir)/include
pkg_libdir=$(pkg_topdir)/lib
pkg_bindir=$(pkg_topdir)/bin
pkg_docdir=$(pkg_topdir)/doc


all::

clean::

mostlyclean::

distclean:: clean
	rm -f *~

maintainer-clean::

depend::

install::

# Other (sub) packages
PACKAGES=niff riffio niffio

# The directory for installing executables that users run
bindir=${exec_prefix}/bin

# The directory for storing read-only, architecture independant data
datadir=${prefix}/share

# The directory for storing architecture-dependent files
exec_prefix=${prefix}

# The directory for installing C header files
includedir=${prefix}/include

# The directory for storing documentation in Info format
infodir=${prefix}/info

# The directory for installing object code libraries
libdir=${exec_prefix}/lib

# The directory for installing executables that other programs run
libexecdir=${exec_prefix}/libexec

# The directory for storing modifiable single-machine data
localstatedir=${prefix}/var

# The top level directory for installing documentation in man format
mandir=${prefix}/man

# The directory for installing C header files for non-gcc compilers
oldincludedir=/usr/include

# The installation prefix for architecture-independent files
prefix=/usr/local

# The directory for installing executables that system administrators run
sbindir=${exec_prefix}/sbin

# The directory for installing modifiable architecture-independent data
sharedstatedir=${prefix}/com

# The directory for installing read-only single machine data.
sysconfdir=${prefix}/etc

# Commands

AR= ar -r -cvu

CC=@CC@
CFLAGS=
ALL_CFLAGS= $(PKG_CFLAGS) $(MOD_CFLAGS) $(CFLAGS) $(FLV_CFLAGS)

CPPFLAGS=
ALL_CPPFLAGS=$(PKG_CPPFLAGS) $(MOD_CPPFLAGS) $(CPPFLAGS) $(FLV_CPPFLAGS)

CXX=@CXX@
CXXFLAGS=
ALL_CXXFLAGS=$(PKG_CXXFLAGS) $(MOD_CXXFLAGS) $(CXXFLAGS) $(FLV_CXXFLAGS)

INSTALL=@INSTALL@
INSTALL_PROGRAM=@INSTALL_PROGRAM@
INSTALL_DATA=@INSTALL_DATA@

LEX=@LEX@
LEXLIB=@LEXLIB@

PERL=@PERL@

RANLIB=@RANLIB@
RM= rm -f
YACC=@YACC@

MKDEP=$(SHELL) $(pkg_mkdir)/mkdep

DEFS= 
LDFLAGS=
ALL_LDFLAGS=$(PKG_LDFLAGS) $(MOD_LDFLAGS) $(LDFLAGS) $(FLV_LDFLAGS)

LIBS=

DEPLIBS=$(PKG_DEPLIBS) $(MOD_DEPLIBS)

ALL_LIBS=$(DEPLIBS) $(LIBS) $(FLV_LIBS)


LIB_SUFFIX=.a
BIN_SUFFIX=

#OBJ_SUFFIX=.o

.c.o:
	$(CC) -c $(ALL_CPPFLAGS) $(ALL_CFLAGS) $<

.SUFFIXES: .txt .pod .htm .man

.c.txt:
	unravel.pl $< >$@

.h.txt:
	unravel.pl $< >$@


include $(pkg_topdir)/local.mk
