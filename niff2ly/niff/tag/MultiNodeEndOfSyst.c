
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "MultiNodeEndOfSyst.h"


static RIFFIOSuccess
cbMultiNodeEndOfSyst(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbMultiNodeEndOfSyst)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("MultiNodeEndOfSyst");
}


void nf2_tag_MultiNodeEndOfSyst_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagMultiNodeEndOfSyst(pparser, NIFFIO_FOURCC_WILDCARD, cbMultiNodeEndOfSyst);
}


void nf2_tag_MultiNodeEndOfSyst_end(NIFFIOParser *pparser)
{
}

