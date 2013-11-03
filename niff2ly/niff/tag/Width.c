
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "Width.h"


static RIFFIOSuccess
cbWidth(NIFFIOTagContext *pctxTag, niffWidth *p)
{
    if (cbTagStart(pctxTag, p, cbWidth)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("Width");
}


void nf2_tag_Width_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagWidth(pparser, NIFFIO_FOURCC_WILDCARD, cbWidth);
}


void nf2_tag_Width_end(NIFFIOParser *pparser)
{
}

