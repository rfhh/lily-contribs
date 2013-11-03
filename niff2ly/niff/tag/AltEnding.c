
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "AltEnding.h"


static RIFFIOSuccess
cbAltEnding(NIFFIOTagContext *pctxTag, niffAltEnding *p)
{
    if (cbTagStart(pctxTag, p, cbAltEnding)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("AltEnding");
}


void nf2_tag_AltEnding_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagAltEnding(pparser, NIFFIO_FOURCC_WILDCARD, cbAltEnding);
}


void nf2_tag_AltEnding_end(NIFFIOParser *pparser)
{
}

