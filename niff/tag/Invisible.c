
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "Invisible.h"


static RIFFIOSuccess
cbInvisible(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbInvisible)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("Invisible");
}


void nf2_tag_Invisible_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagInvisible(pparser, NIFFIO_FOURCC_WILDCARD, cbInvisible);
}


void nf2_tag_Invisible_end(NIFFIOParser *pparser)
{
}

