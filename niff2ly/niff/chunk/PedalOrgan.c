
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "PedalOrgan.h"


static RIFFIOSuccess
cbPedalOrganStart(NIFFIOChunkContext *pctxChunk, niffPedalOrgan *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("PedalOrganStart");
}


static RIFFIOSuccess
cbPedalOrganEnd(NIFFIOChunkContext *pctxChunk, niffPedalOrgan *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("PedalOrganEnd");
}


void nf2_chunk_PedalOrgan_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPedalOrgan(pparser, cbPedalOrganStart, cbPedalOrganEnd);
}


void nf2_chunk_PedalOrgan_end(NIFFIOParser *pparser)
{
}

