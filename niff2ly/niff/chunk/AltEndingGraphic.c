
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "AltEndingGraphic.h"


static RIFFIOSuccess
cbAltEndingGraphicStart(NIFFIOChunkContext *pctxChunk, niffAltEndingGraphic *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("AltEndingGraphicStart");
}


static RIFFIOSuccess
cbAltEndingGraphicEnd(NIFFIOChunkContext *pctxChunk, niffAltEndingGraphic *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("AltEndingGraphicEnd");
}


void nf2_chunk_AltEndingGraphic_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkAltEndingGraphic(pparser, cbAltEndingGraphicStart, cbAltEndingGraphicEnd);
}


void nf2_chunk_AltEndingGraphic_end(NIFFIOParser *pparser)
{
}

