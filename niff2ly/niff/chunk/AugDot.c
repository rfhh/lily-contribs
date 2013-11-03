
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "AugDot.h"



static RIFFIOSuccess
cbAugDotStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("AugDotStart");
}

static RIFFIOSuccess
cbAugDotEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("AugDotEnd");
}


void nf2_chunk_AugDot_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkAugDot(pparser, cbAugDotStart, cbAugDotEnd);
}


void nf2_chunk_AugDot_end(NIFFIOParser *pparser)
{
}

