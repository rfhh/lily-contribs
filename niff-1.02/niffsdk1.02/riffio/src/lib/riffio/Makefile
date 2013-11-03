# $Id

pkg_topdir=../../..

include $(pkg_topdir)/mk/top.mk

EXPORT_LIBRARY = riffio
EXPORT_HEADERS = $(pkg_incdir)/riffio.h

SRCS = \
chunks.c\
error.c\
fcc.c\
rfile.c\
rwbytes.c\
stack.c\
table.c

OBJS = $(SRCS:c=o)
DOCS = $(SRCS:c=txt)

include $(pkg_mkdir)/inc.mk
include $(pkg_mkdir)/lib.mk
include $(pkg_mkdir)/doc.mk


#maintainer-clean::
#	rm -f $(DOCS)


