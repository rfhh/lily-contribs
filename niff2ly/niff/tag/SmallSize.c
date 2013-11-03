
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "SmallSize.h"


static RIFFIOSuccess
cbSmallSize(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbSmallSize)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("SmallSize");
}


void nf2_tag_SmallSize_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagSmallSize(pparser, NIFFIO_FOURCC_WILDCARD, cbSmallSize);
}


void nf2_tag_SmallSize_end(NIFFIOParser *pparser)
{
}

