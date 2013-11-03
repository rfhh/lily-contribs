
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "TieDirection.h"


static RIFFIOSuccess
cbTieDirection(NIFFIOTagContext *pctxTag, niffTieDirection *p)
{
    if (cbTagStart(pctxTag, p, cbTieDirection)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("TieDirection");
}


void nf2_tag_TieDirection_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagTieDirection(pparser, NIFFIO_FOURCC_WILDCARD, cbTieDirection);
}


void nf2_tag_TieDirection_end(NIFFIOParser *pparser)
{
}

