
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "LargeSize.h"


static RIFFIOSuccess
cbLargeSize(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbLargeSize)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("LargeSize");
}


void nf2_tag_LargeSize_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagLargeSize(pparser, NIFFIO_FOURCC_WILDCARD, cbLargeSize);
}


void nf2_tag_LargeSize_end(NIFFIOParser *pparser)
{
}

