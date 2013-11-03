
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "Glissando.h"



static RIFFIOSuccess
cbGlissandoStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("GlissandoStart");
}

static RIFFIOSuccess
cbGlissandoEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("GlissandoEnd");
}


void nf2_chunk_Glissando_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkGlissando(pparser, cbGlissandoStart, cbGlissandoEnd);
}


void nf2_chunk_Glissando_end(NIFFIOParser *pparser)
{
}

