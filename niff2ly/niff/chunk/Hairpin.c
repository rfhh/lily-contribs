
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Hairpin.h"


static RIFFIOSuccess
cbHairpinStart(NIFFIOChunkContext *pctxChunk, niffHairpin *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("HairpinStart");
}


static RIFFIOSuccess
cbHairpinEnd(NIFFIOChunkContext *pctxChunk, niffHairpin *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("HairpinEnd");
}


void nf2_chunk_Hairpin_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkHairpin(pparser, cbHairpinStart, cbHairpinEnd);
}


void nf2_chunk_Hairpin_end(NIFFIOParser *pparser)
{
}

