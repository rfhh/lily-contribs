
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "CustomFontChar.h"


static RIFFIOSuccess
cbCustomFontChar(NIFFIOTagContext *pctxTag, niffCustomFontChar *p)
{
    if (cbTagStart(pctxTag, p, cbCustomFontChar)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("CustomFontChar");
}


void nf2_tag_CustomFontChar_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagCustomFontChar(pparser, NIFFIO_FOURCC_WILDCARD, cbCustomFontChar);
}


void nf2_tag_CustomFontChar_end(NIFFIOParser *pparser)
{
}

