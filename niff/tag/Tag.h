
#ifndef __NF2_TAG_Tag_H__
#define __NF2_TAG_Tag_H__

#include "niffio.h"

#include "niff/frontend.h"


#define UNIMPLEMENTED_TAG(name) \
	do { \
	    VPRINTF(" /unimplemented tag %s;", name); \
	    return RIFFIO_OK; \
	} while (0)

void tag_activate_reset(void);
void tag_activate_start(void);
void tag_activate_stop(void);
void tag_activate_apply(void);

void tag_inactivate_start(void);
void tag_inactivate_stop(void);

void tags_reset(void);

RIFFIOSuccess cbTagStart(NIFFIOTagContext *pctxTag, void *p, void *callback);
RIFFIOSuccess cbTagEnd(NIFFIOTagContext *pctxTag);

void nf2_tag_Tag_init(NIFFIOParser *pparser);
void nf2_tag_Tag_end(NIFFIOParser *pparser);

#endif
