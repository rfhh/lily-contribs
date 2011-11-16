
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "LogicalPlacement.h"


static RIFFIOSuccess
cbLogicalPlacement(NIFFIOTagContext *pctxTag, niffLogicalPlacement *p)
{
    if (cbTagStart(pctxTag, p, cbLogicalPlacement)) {
	assert(pctxTag != 0);
	assert(pctxTag->pnf != 0);
	assert(pctxTag->pchunkParent != 0);

	if (symbol_current != NULL && symbol_current->type == SYM_STEM &&
	    p->vertical != 0) {
	    symbol_current->symbol.stem.flags |= FLAG_STEM_EXPLICIT;
	    if (p->vertical == 1) {
		symbol_current->symbol.stem.flags |= FLAG_STEM_UP;
	    }
	} else {
	    VPRINTF(" = (%d,%d) (proximity %d)", p->horizontal, p->vertical, p->proximity);
	}
    }
    cbTagEnd(pctxTag);

    return RIFFIO_OK;
}


void nf2_tag_LogicalPlacement_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagLogicalPlacement(pparser, NIFFIO_FOURCC_WILDCARD, cbLogicalPlacement);
}


void nf2_tag_LogicalPlacement_end(NIFFIOParser *pparser)
{
}

