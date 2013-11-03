
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "GuitarTabNum.h"


static RIFFIOSuccess
cbGuitarTabNumStart(NIFFIOChunkContext *pctxChunk, niffGuitarTabNum *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("GuitarTabNumStart");
}


static RIFFIOSuccess
cbGuitarTabNumEnd(NIFFIOChunkContext *pctxChunk, niffGuitarTabNum *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("GuitarTabNumEnd");
}


void nf2_chunk_GuitarTabNum_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkGuitarTabNum(pparser, cbGuitarTabNumStart, cbGuitarTabNumEnd);
}


void nf2_chunk_GuitarTabNum_end(NIFFIOParser *pparser)
{
}

