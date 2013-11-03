
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "TempoMarking.h"


static RIFFIOSuccess
cbTempoMarkingStart(NIFFIOChunkContext *pctxChunk, niffTempoMarking *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("TempoMarkingStart");
}


static RIFFIOSuccess
cbTempoMarkingEnd(NIFFIOChunkContext *pctxChunk, niffTempoMarking *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("TempoMarkingEnd");
}


void nf2_chunk_TempoMarking_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTempoMarking(pparser, cbTempoMarkingStart, cbTempoMarkingEnd);
}


void nf2_chunk_TempoMarking_end(NIFFIOParser *pparser)
{
}

