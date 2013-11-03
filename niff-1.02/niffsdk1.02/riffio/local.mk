# $Id: local.mk,v 1.2 1996/06/06 02:06:50 tim Exp $
#
# Package specific make

PKG_CPPFLAGS=-I$(pkg_incdir)
PKG_DEPLIBS=$(pkg_libdir)/stdcriff.a $(pkg_libdir)/riffio.a
