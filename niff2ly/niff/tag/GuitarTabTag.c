
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "GuitarTabTag.h"


static RIFFIOSuccess
cbGuitarTabTag(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbGuitarTabTag)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("GuitarTabTag");
}


void nf2_tag_GuitarTabTag_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagGuitarTabTag(pparser, NIFFIO_FOURCC_WILDCARD, cbGuitarTabTag);
}


void nf2_tag_GuitarTabTag_end(NIFFIOParser *pparser)
{
}

