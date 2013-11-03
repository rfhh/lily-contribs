
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "MultiNodeStartOfSyst.h"


static RIFFIOSuccess
cbMultiNodeStartOfSyst(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbMultiNodeStartOfSyst)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("MultiNodeStartOfSyst");
}


void nf2_tag_MultiNodeStartOfSyst_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagMultiNodeStartOfSyst(pparser, NIFFIO_FOURCC_WILDCARD, cbMultiNodeStartOfSyst);
}


void nf2_tag_MultiNodeStartOfSyst_end(NIFFIOParser *pparser)
{
}

