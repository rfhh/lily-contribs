
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "Silent.h"


static RIFFIOSuccess
cbSilent(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbSilent)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("Silent");
}


void nf2_tag_Silent_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagSilent(pparser, NIFFIO_FOURCC_WILDCARD, cbSilent);
}


void nf2_tag_Silent_end(NIFFIOParser *pparser)
{
}

