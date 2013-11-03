
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "FontID.h"


static RIFFIOSuccess
cbFontID(NIFFIOTagContext *pctxTag, niffFontID *p)
{
    if (cbTagStart(pctxTag, p, cbFontID)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("FontID");
}


void nf2_tag_FontID_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagFontID(pparser, NIFFIO_FOURCC_WILDCARD, cbFontID);
}


void nf2_tag_FontID_end(NIFFIOParser *pparser)
{
}

