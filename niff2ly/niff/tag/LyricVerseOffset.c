
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "LyricVerseOffset.h"


static RIFFIOSuccess
cbLyricVerseOffset(NIFFIOTagContext *pctxTag, niffLyricVerseOffset *p)
{
    if (cbTagStart(pctxTag, p, cbLyricVerseOffset)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("LyricVerseOffset");
}


void nf2_tag_LyricVerseOffset_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagLyricVerseOffset(pparser, NIFFIO_FOURCC_WILDCARD, cbLyricVerseOffset);
}


void nf2_tag_LyricVerseOffset_end(NIFFIOParser *pparser)
{
}

