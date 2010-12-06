
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "PageHeader.h"



static RIFFIOSuccess
cbPageHeaderStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("PageHeaderStart");
}

static RIFFIOSuccess
cbPageHeaderEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("PageHeaderEnd");
}


void nf2_chunk_PageHeader_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPageHeader(pparser, cbPageHeaderStart, cbPageHeaderEnd);
}


void nf2_chunk_PageHeader_end(NIFFIOParser *pparser)
{
}

