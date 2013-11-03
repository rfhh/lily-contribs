# $Id: Makefile,v 1.1 1996/06/06 05:02:07 tim Exp $

pkg_topdir=../../..

include $(pkg_topdir)/mk/top.mk

EXPORT_PROGRAM = niffdump

SRCS = niffdump.c \
       callback.c

OBJS = $(SRCS:c=o)
DOCS = niffdump.txt

include $(pkg_mkdir)/bin.mk
include $(pkg_mkdir)/doc.mk
