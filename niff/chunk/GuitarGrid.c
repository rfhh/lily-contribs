
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "GuitarGrid.h"


static RIFFIOSuccess
cbGuitarGridStart(NIFFIOChunkContext *pctxChunk, niffGuitarGrid *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("GuitarGridStart");
}


static RIFFIOSuccess
cbGuitarGridEnd(NIFFIOChunkContext *pctxChunk, niffGuitarGrid *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("GuitarGridEnd");
}


void nf2_chunk_GuitarGrid_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkGuitarGrid(pparser, cbGuitarGridStart, cbGuitarGridEnd);
}


void nf2_chunk_GuitarGrid_end(NIFFIOParser *pparser)
{
}

