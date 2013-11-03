
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "SplitStem.h"


static RIFFIOSuccess
cbSplitStem(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbSplitStem)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("SplitStem");
}


void nf2_tag_SplitStem_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagSplitStem(pparser, NIFFIO_FOURCC_WILDCARD, cbSplitStem);
}


void nf2_tag_SplitStem_end(NIFFIOParser *pparser)
{
}

