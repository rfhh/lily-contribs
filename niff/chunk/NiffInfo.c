
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "NiffInfo.h"


static RIFFIOSuccess
cbNiffInfoStart(NIFFIOChunkContext *pctxChunk, niffNiffInfo *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("NiffInfoStart");
}


static RIFFIOSuccess
cbNiffInfoEnd(NIFFIOChunkContext *pctxChunk, niffNiffInfo *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("NiffInfoEnd");
}


void nf2_chunk_NiffInfo_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkNiffInfo(pparser, cbNiffInfoStart, cbNiffInfoEnd);
}


void nf2_chunk_NiffInfo_end(NIFFIOParser *pparser)
{
}

