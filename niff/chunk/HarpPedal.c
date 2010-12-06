
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "HarpPedal.h"


static RIFFIOSuccess
cbHarpPedalStart(NIFFIOChunkContext *pctxChunk, niffHarpPedal *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("HarpPedalStart");
}


static RIFFIOSuccess
cbHarpPedalEnd(NIFFIOChunkContext *pctxChunk, niffHarpPedal *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("HarpPedalEnd");
}


void nf2_chunk_HarpPedal_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkHarpPedal(pparser, cbHarpPedalStart, cbHarpPedalEnd);
}


void nf2_chunk_HarpPedal_end(NIFFIOParser *pparser)
{
}

