
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "DefaultValues.h"


static RIFFIOSuccess
cbDefaultValuesStart(NIFFIOChunkContext *pctxChunk, niffDefaultValues *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("DefaultValuesStart");
}


static RIFFIOSuccess
cbDefaultValuesEnd(NIFFIOChunkContext *pctxChunk, niffDefaultValues *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("DefaultValuesEnd");
}


void nf2_chunk_DefaultValues_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkDefaultValues(pparser, cbDefaultValuesStart, cbDefaultValuesEnd);
}


void nf2_chunk_DefaultValues_end(NIFFIOParser *pparser)
{
}

