
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "RehearsalOffset.h"


static RIFFIOSuccess
cbRehearsalOffset(NIFFIOTagContext *pctxTag, niffRehearsalOffset *p)
{
    if (cbTagStart(pctxTag, p, cbRehearsalOffset)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("RehearsalOffset");
}


void nf2_tag_RehearsalOffset_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagRehearsalOffset(pparser, NIFFIO_FOURCC_WILDCARD, cbRehearsalOffset);
}


void nf2_tag_RehearsalOffset_end(NIFFIOParser *pparser)
{
}

