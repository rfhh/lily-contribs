# $Id: Makefile,v 1.5 1996/06/15 02:53:27 tim Exp $

pkg_topdir=../..

include $(pkg_topdir)/mk/top.mk

IMPORT_DOCS =  \
	clt.txt \
	inherit.txt \
	names.txt \
	nfile.txt \
	niffio.txt \
	parse.txt \
	register.txt \
	rwniff.txt \
	rwtypes.txt \
	stbl.txt \
	store.txt \
	storenif.txt \
	tags.txt  \
	\
	niffdump.txt \
	nif001.txt

DOCS= \
	$(IMPORT_DOCS)

PODS=$(DOCS:txt=pod)
MANS=$(DOCS:txt=man)
HTMS=$(DOCS:txt=htm)

.txt.pod:
	txt2pod.pl $< >$@

.pod.man:
	pod2man "--section=3" --release=niffio "--center=NIFFIO Documentation"\
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
	rm -f $(IMPORT_DOCS)
