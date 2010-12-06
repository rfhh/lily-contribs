
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "NumberStyle.h"


static RIFFIOSuccess
cbNumberStyle(NIFFIOTagContext *pctxTag, niffNumberStyle *p)
{
    if (cbTagStart(pctxTag, p, cbNumberStyle)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("NumberStyle");
}


void nf2_tag_NumberStyle_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagNumberStyle(pparser, NIFFIO_FOURCC_WILDCARD, cbNumberStyle);
}


void nf2_tag_NumberStyle_end(NIFFIOParser *pparser)
{
}

