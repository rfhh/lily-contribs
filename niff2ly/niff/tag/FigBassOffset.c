
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "FigBassOffset.h"


static RIFFIOSuccess
cbFigBassOffset(NIFFIOTagContext *pctxTag, niffFigBassOffset *p)
{
    if (cbTagStart(pctxTag, p, cbFigBassOffset)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("FigBassOffset");
}


void nf2_tag_FigBassOffset_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagFigBassOffset(pparser, NIFFIO_FOURCC_WILDCARD, cbFigBassOffset);
}


void nf2_tag_FigBassOffset_end(NIFFIOParser *pparser)
{
}

