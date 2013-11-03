
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "Thickness.h"


static RIFFIOSuccess
cbThickness(NIFFIOTagContext *pctxTag, niffThickness *p)
{
    if (cbTagStart(pctxTag, p, cbThickness)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("Thickness");
}


void nf2_tag_Thickness_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagThickness(pparser, NIFFIO_FOURCC_WILDCARD, cbThickness);
}


void nf2_tag_Thickness_end(NIFFIOParser *pparser)
{
}

