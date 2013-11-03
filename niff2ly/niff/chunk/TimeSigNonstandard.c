
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "TimeSigNonstandard.h"


static RIFFIOSuccess
cbTimeSigNonstandardStart(NIFFIOChunkContext *pctxChunk, niffTimeSigNonstandard *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("TimeSigNonstandardStart");
}


static RIFFIOSuccess
cbTimeSigNonstandardEnd(NIFFIOChunkContext *pctxChunk, niffTimeSigNonstandard *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("TimeSigNonstandardEnd");
}


void nf2_chunk_TimeSigNonstandard_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTimeSigNonstandard(pparser, cbTimeSigNonstandardStart, cbTimeSigNonstandardEnd);
}


void nf2_chunk_TimeSigNonstandard_end(NIFFIOParser *pparser)
{
}

