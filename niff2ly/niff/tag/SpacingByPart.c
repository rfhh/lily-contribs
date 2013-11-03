
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "SpacingByPart.h"


static RIFFIOSuccess
cbSpacingByPart(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbSpacingByPart)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("SpacingByPart");
}


void nf2_tag_SpacingByPart_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagSpacingByPart(pparser, NIFFIO_FOURCC_WILDCARD, cbSpacingByPart);
}


void nf2_tag_SpacingByPart_end(NIFFIOParser *pparser)
{
}

