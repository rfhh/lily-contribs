# $Id: Makefile,v 1.1 1996/06/12 06:29:06 tim Exp $

pkg_topdir=../..

include $(pkg_topdir)/mk/top.mk


IMPORTED_DOCS=

DOCS= \
	niffsdk.txt \
	snapshot.txt \
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

