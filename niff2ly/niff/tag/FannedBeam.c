
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "FannedBeam.h"


static RIFFIOSuccess
cbFannedBeam(NIFFIOTagContext *pctxTag, niffFannedBeam *p)
{
    if (cbTagStart(pctxTag, p, cbFannedBeam)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("FannedBeam");
}


void nf2_tag_FannedBeam_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagFannedBeam(pparser, NIFFIO_FOURCC_WILDCARD, cbFannedBeam);
}


void nf2_tag_FannedBeam_end(NIFFIOParser *pparser)
{
}

