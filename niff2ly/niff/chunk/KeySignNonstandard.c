
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "KeySignNonstandard.h"


static RIFFIOSuccess
cbKeySignNonstandardStart(NIFFIOChunkContext *pctxChunk, niffKeySignNonstandard *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("KeySignNonstandardStart");
}


static RIFFIOSuccess
cbKeySignNonstandardEnd(NIFFIOChunkContext *pctxChunk, niffKeySignNonstandard *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("KeySignNonstandardEnd");
}


void nf2_chunk_KeySignNonstandard_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkKeySignNonstandard(pparser, cbKeySignNonstandardStart, cbKeySignNonstandardEnd);
}


void nf2_chunk_KeySignNonstandard_end(NIFFIOParser *pparser)
{
}

