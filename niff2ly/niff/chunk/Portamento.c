
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "Portamento.h"



static RIFFIOSuccess
cbPortamentoStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("PortamentoStart");
}

static RIFFIOSuccess
cbPortamentoEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("PortamentoEnd");
}


void nf2_chunk_Portamento_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPortamento(pparser, cbPortamentoStart, cbPortamentoEnd);
}


void nf2_chunk_Portamento_end(NIFFIOParser *pparser)
{
}

