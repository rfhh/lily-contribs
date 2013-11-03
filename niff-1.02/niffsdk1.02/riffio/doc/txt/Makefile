# $Id: Makefile,v 1.2 1996/06/06 06:09:37 tim Exp $

pkg_topdir=../..

include $(pkg_topdir)/mk/top.mk


IMPORTED_DOCS=      \
	riffio.txt  \
	\
	chunks.txt  \
	error.txt   \
	fcc.txt     \
	rfile.txt   \
	rwbytes.txt \
	stack.txt   \
	table.txt   \
	\
	stdcriff.txt\
	\
	riffdump.txt \
	\
	t_fcc.txt \
	t_stack.txt \
	\
	abcde.txt \
	hello.txt

DOCS= \
	changes.txt \
	$(IMPORTED_DOCS)

PODS=$(DOCS:txt=pod)
MANS=$(DOCS:txt=man)
HTMS=$(DOCS:txt=htm)

.txt.pod:
	txt2pod.pl $< >$@

.pod.man:
	pod2man "--section=3" --release=riffio "--center=RIFFIO Documentation"\
	$< >$@

doc:: pod man htm
	if [ ! -d ../pod ]; then mkdir ../pod; fi
	if [ ! -d ../man ]; then mkdir ../man; fi
	if [ ! -d ../htm ]; then mkdir ../htm; fi
	mv $(PODS) ../pod
	mv $(MANS) ../man
	mv $(HTMS) ../htm

pod: $(PODS)

man: $(MANS)

#htm: $(PODS)
#	pod2html -dsubs -ditems -dheaders -dvars $(PODS)

htm: $(PODS)
	pod2html $(PODS)


#	for i in $(HTMS); do (mv $$i  ../htm/`basename $$i html`htm); done


maintainer-clean::
	rm -rf ../htm ../man ../pod
	rm -f $(IMPORTED_DOCS)

