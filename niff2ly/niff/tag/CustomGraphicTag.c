
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "CustomGraphicTag.h"


static RIFFIOSuccess
cbCustomGraphicTag(NIFFIOTagContext *pctxTag, niffCustomGraphicTag *p)
{
    if (cbTagStart(pctxTag, p, cbCustomGraphicTag)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("CustomGraphicTag");
}


void nf2_tag_CustomGraphicTag_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagCustomGraphicTag(pparser, NIFFIO_FOURCC_WILDCARD, cbCustomGraphicTag);
}


void nf2_tag_CustomGraphicTag_end(NIFFIOParser *pparser)
{
}

