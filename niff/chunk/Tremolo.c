
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Tremolo.h"


static RIFFIOSuccess
cbTremoloStart(NIFFIOChunkContext *pctxChunk, niffTremolo *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("TremoloStart");
}


static RIFFIOSuccess
cbTremoloEnd(NIFFIOChunkContext *pctxChunk, niffTremolo *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("TremoloEnd");
}


void nf2_chunk_Tremolo_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTremolo(pparser, cbTremoloStart, cbTremoloEnd);
}


void nf2_chunk_Tremolo_end(NIFFIOParser *pparser)
{
}

