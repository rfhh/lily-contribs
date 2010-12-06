
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Dynamic.h"


static RIFFIOSuccess
cbDynamicStart(NIFFIOChunkContext *pctxChunk, niffDynamic *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("DynamicStart");
}


static RIFFIOSuccess
cbDynamicEnd(NIFFIOChunkContext *pctxChunk, niffDynamic *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("DynamicEnd");
}


void nf2_chunk_Dynamic_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkDynamic(pparser, cbDynamicStart, cbDynamicEnd);
}


void nf2_chunk_Dynamic_end(NIFFIOParser *pparser)
{
}

