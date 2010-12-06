
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "GuitarGridOffset.h"


static RIFFIOSuccess
cbGuitarGridOffset(NIFFIOTagContext *pctxTag, niffGuitarGridOffset *p)
{
    if (cbTagStart(pctxTag, p, cbGuitarGridOffset)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("GuitarGridOffset");
}


void nf2_tag_GuitarGridOffset_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagGuitarGridOffset(pparser, NIFFIO_FOURCC_WILDCARD, cbGuitarGridOffset);
}


void nf2_tag_GuitarGridOffset_end(NIFFIOParser *pparser)
{
}

