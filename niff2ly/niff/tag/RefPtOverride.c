
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "RefPtOverride.h"


static RIFFIOSuccess
cbRefPtOverride(NIFFIOTagContext *pctxTag, niffRefPtOverride *p)
{
    if (cbTagStart(pctxTag, p, cbRefPtOverride)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("RefPtOverride");
}


void nf2_tag_RefPtOverride_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagRefPtOverride(pparser, NIFFIO_FOURCC_WILDCARD, cbRefPtOverride);
}


void nf2_tag_RefPtOverride_end(NIFFIOParser *pparser)
{
}

