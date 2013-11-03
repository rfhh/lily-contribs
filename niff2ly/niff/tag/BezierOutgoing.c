
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "BezierOutgoing.h"


static RIFFIOSuccess
cbBezierOutgoing(NIFFIOTagContext *pctxTag, niffBezierOutgoing *p)
{
    if (cbTagStart(pctxTag, p, cbBezierOutgoing)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("BezierOutgoing");
}


void nf2_tag_BezierOutgoing_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagBezierOutgoing(pparser, NIFFIO_FOURCC_WILDCARD, cbBezierOutgoing);
}


void nf2_tag_BezierOutgoing_end(NIFFIOParser *pparser)
{
}

