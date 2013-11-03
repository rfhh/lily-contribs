
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "niff/list/Staff.h"

#include "SystemHeader.h"



static RIFFIOSuccess
cbSystemHeaderStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    part_current = NULL;
    staff_current = -1;
    voice_previous = NULL;
    UNIMPLEMENTED_CHUNK("SystemHeaderStart");
}

static RIFFIOSuccess
cbSystemHeaderEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("SystemHeaderEnd");
}


void nf2_chunk_SystemHeader_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkSystemHeader(pparser, cbSystemHeaderStart, cbSystemHeaderEnd);
}


void nf2_chunk_SystemHeader_end(NIFFIOParser *pparser)
{
}

