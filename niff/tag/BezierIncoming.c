
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "BezierIncoming.h"


static RIFFIOSuccess
cbBezierIncoming(NIFFIOTagContext *pctxTag, niffBezierIncoming *p)
{
    if (cbTagStart(pctxTag, p, cbBezierIncoming)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("BezierIncoming");
}


void nf2_tag_BezierIncoming_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagBezierIncoming(pparser, NIFFIO_FOURCC_WILDCARD, cbBezierIncoming);
}


void nf2_tag_BezierIncoming_end(NIFFIOParser *pparser)
{
}

