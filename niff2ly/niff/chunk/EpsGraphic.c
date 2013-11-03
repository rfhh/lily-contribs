
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "EpsGraphic.h"


static RIFFIOSuccess
cbEpsGraphic(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("EpsGraphic");
}


void nf2_chunk_EpsGraphic_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkEpsGraphic(pparser, cbEpsGraphic);
}


void nf2_chunk_EpsGraphic_end(NIFFIOParser *pparser)
{
}

