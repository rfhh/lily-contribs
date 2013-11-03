
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "SystemSeparation.h"


static RIFFIOSuccess
cbSystemSeparationStart(NIFFIOChunkContext *pctxChunk, niffSystemSeparation *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("SystemSeparationStart");
}


static RIFFIOSuccess
cbSystemSeparationEnd(NIFFIOChunkContext *pctxChunk, niffSystemSeparation *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("SystemSeparationEnd");
}


void nf2_chunk_SystemSeparation_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkSystemSeparation(pparser, cbSystemSeparationStart, cbSystemSeparationEnd);
}


void nf2_chunk_SystemSeparation_end(NIFFIOParser *pparser)
{
}

