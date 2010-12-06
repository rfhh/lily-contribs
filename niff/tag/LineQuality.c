
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "LineQuality.h"


static RIFFIOSuccess
cbLineQuality(NIFFIOTagContext *pctxTag, niffLineQuality *p)
{
    if (cbTagStart(pctxTag, p, cbLineQuality)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("LineQuality");
}


void nf2_tag_LineQuality_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagLineQuality(pparser, NIFFIO_FOURCC_WILDCARD, cbLineQuality);
}


void nf2_tag_LineQuality_end(NIFFIOParser *pparser)
{
}

