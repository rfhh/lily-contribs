
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "EndOfSystem.h"


static RIFFIOSuccess
cbEndOfSystem(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbEndOfSystem)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("EndOfSystem");
}


void nf2_tag_EndOfSystem_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagEndOfSystem(pparser, NIFFIO_FOURCC_WILDCARD, cbEndOfSystem);
}


void nf2_tag_EndOfSystem_end(NIFFIOParser *pparser)
{
}

