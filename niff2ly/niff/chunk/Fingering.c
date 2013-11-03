
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Fingering.h"


static RIFFIOSuccess
cbFingeringStart(NIFFIOChunkContext *pctxChunk, niffFingering *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("FingeringStart");
}


static RIFFIOSuccess
cbFingeringEnd(NIFFIOChunkContext *pctxChunk, niffFingering *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("FingeringEnd");
}


void nf2_chunk_Fingering_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkFingering(pparser, cbFingeringStart, cbFingeringEnd);
}


void nf2_chunk_Fingering_end(NIFFIOParser *pparser)
{
}

