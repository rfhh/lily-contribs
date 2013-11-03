

pkg_topdir=../../..

include $(pkg_topdir)/mk/top.mk

EXPORT_LIBRARY = niffio
EXPORT_HEADERS = $(pkg_incdir)/niffio.h

SRCS = \
clt.c\
inherit.c\
nfile.c\
names.c\
parse.c\
register.c\
rwniff.c\
rwtypes.c\
stbl.c\
store.c\
storenif.c\
tags.c\
tcbtable.c

OBJS = $(SRCS:c=o)
DOCS = clt.txt inherit.txt names.txt nfile.txt parse.txt \
       register.txt rwniff.txt \
       rwtypes.txt stbl.txt store.txt storenif.txt tags.txt

include $(pkg_mkdir)/inc.mk
include $(pkg_mkdir)/lib.mk
include $(pkg_mkdir)/doc.mk

