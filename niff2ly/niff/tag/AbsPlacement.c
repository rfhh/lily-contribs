
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "AbsPlacement.h"


static RIFFIOSuccess
cbAbsPlacement(NIFFIOTagContext *pctxTag, niffAbsPlacement *p)
{
    if (cbTagStart(pctxTag, p, cbAbsPlacement)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("AbsPlacement");
}


void nf2_tag_AbsPlacement_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagAbsPlacement(pparser, NIFFIO_FOURCC_WILDCARD, cbAbsPlacement);
}


void nf2_tag_AbsPlacement_end(NIFFIOParser *pparser)
{
}

