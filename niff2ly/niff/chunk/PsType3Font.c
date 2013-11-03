
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "PsType3Font.h"


static RIFFIOSuccess
cbPsType3Font(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("PsType3Font");
}


void nf2_chunk_PsType3Font_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPsType3Font(pparser, cbPsType3Font);
}


void nf2_chunk_PsType3Font_end(NIFFIOParser *pparser)
{
}

