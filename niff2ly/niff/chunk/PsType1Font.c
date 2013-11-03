
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "PsType1Font.h"


static RIFFIOSuccess
cbPsType1Font(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("PsType1Font");
}


void nf2_chunk_PsType1Font_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPsType1Font(pparser, cbPsType1Font);
}


void nf2_chunk_PsType1Font_end(NIFFIOParser *pparser)
{
}

