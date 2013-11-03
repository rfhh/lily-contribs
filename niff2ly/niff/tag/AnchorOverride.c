
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "AnchorOverride.h"


static RIFFIOSuccess
cbAnchorOverride(NIFFIOTagContext *pctxTag, niffAnchorOverride *p)
{
    if (cbTagStart(pctxTag, p, cbAnchorOverride)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("AnchorOverride");
}


void nf2_tag_AnchorOverride_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagAnchorOverride(pparser, NIFFIO_FOURCC_WILDCARD, cbAnchorOverride);
}


void nf2_tag_AnchorOverride_end(NIFFIOParser *pparser)
{
}

