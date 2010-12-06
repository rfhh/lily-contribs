
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "RestNumeral.h"


static RIFFIOSuccess
cbRestNumeral(NIFFIOTagContext *pctxTag, niffRestNumeral *p)
{
    if (cbTagStart(pctxTag, p, cbRestNumeral)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("RestNumeral");
}


void nf2_tag_RestNumeral_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagRestNumeral(pparser, NIFFIO_FOURCC_WILDCARD, cbRestNumeral);
}


void nf2_tag_RestNumeral_end(NIFFIOParser *pparser)
{
}

