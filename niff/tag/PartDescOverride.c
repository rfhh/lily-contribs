
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "PartDescOverride.h"


static RIFFIOSuccess
cbPartDescOverride(NIFFIOTagContext *pctxTag, niffPartDescOverride *p)
{
    if (cbTagStart(pctxTag, p, cbPartDescOverride)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("PartDescOverride");
}


void nf2_tag_PartDescOverride_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagPartDescOverride(pparser, NIFFIO_FOURCC_WILDCARD, cbPartDescOverride);
}


void nf2_tag_PartDescOverride_end(NIFFIOParser *pparser)
{
}

