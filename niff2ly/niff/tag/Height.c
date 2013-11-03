
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "Height.h"


static RIFFIOSuccess
cbHeight(NIFFIOTagContext *pctxTag, niffHeight *p)
{
    if (cbTagStart(pctxTag, p, cbHeight)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("Height");
}


void nf2_tag_Height_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagHeight(pparser, NIFFIO_FOURCC_WILDCARD, cbHeight);
}


void nf2_tag_Height_end(NIFFIOParser *pparser)
{
}

