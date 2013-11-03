
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "ChordSymbolsOffset.h"


static RIFFIOSuccess
cbChordSymbolsOffset(NIFFIOTagContext *pctxTag, niffChordSymbolsOffset *p)
{
    if (cbTagStart(pctxTag, p, cbChordSymbolsOffset)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("ChordSymbolsOffset");
}


void nf2_tag_ChordSymbolsOffset_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagChordSymbolsOffset(pparser, NIFFIO_FOURCC_WILDCARD, cbChordSymbolsOffset);
}


void nf2_tag_ChordSymbolsOffset_end(NIFFIOParser *pparser)
{
}

