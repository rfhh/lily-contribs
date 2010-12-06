
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "TempoMarkNonstandard.h"


static RIFFIOSuccess
cbTempoMarkNonstandardStart(NIFFIOChunkContext *pctxChunk, niffTempoMarkNonstandard *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("TempoMarkNonstandardStart");
}


static RIFFIOSuccess
cbTempoMarkNonstandardEnd(NIFFIOChunkContext *pctxChunk, niffTempoMarkNonstandard *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("TempoMarkNonstandardEnd");
}


void nf2_chunk_TempoMarkNonstandard_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTempoMarkNonstandard(pparser, cbTempoMarkNonstandardStart, cbTempoMarkNonstandardEnd);
}


void nf2_chunk_TempoMarkNonstandard_end(NIFFIOParser *pparser)
{
}

