# $Id: Makefile,v 1.3 1996/06/06 03:34:06 tim Exp $

pkg_topdir=../../..

include $(pkg_topdir)/mk/top.mk

EXPORT_LIBRARY = stdcriff
EXPORT_HEADERS = $(pkg_incdir)/stdcriff.h

SRCS = stdcriff.c
OBJS = $(SRCS:c=o)
DOCS = $(SRCS:c=txt)

include $(pkg_mkdir)/inc.mk
include $(pkg_mkdir)/lib.mk
include $(pkg_mkdir)/doc.mk

