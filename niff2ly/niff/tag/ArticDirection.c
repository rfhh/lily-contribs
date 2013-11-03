
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "ArticDirection.h"


static RIFFIOSuccess
cbArticDirection(NIFFIOTagContext *pctxTag, niffArticDirection *p)
{
    if (cbTagStart(pctxTag, p, cbArticDirection)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("ArticDirection");
}


void nf2_tag_ArticDirection_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagArticDirection(pparser, NIFFIO_FOURCC_WILDCARD, cbArticDirection);
}


void nf2_tag_ArticDirection_end(NIFFIOParser *pparser)
{
}

