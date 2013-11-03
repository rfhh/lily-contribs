
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "TagInactivate.h"

#include "niff/tag/Tag.h"



static RIFFIOSuccess
cbTagInactivateStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    tag_inactivate_start();
    UNIMPLEMENTED_CHUNK("TagInactivateStart");
}

static RIFFIOSuccess
cbTagInactivateEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    tag_inactivate_stop();
    UNIMPLEMENTED_CHUNK("TagInactivateEnd");
}


void nf2_chunk_TagInactivate_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTagInactivate(pparser, cbTagInactivateStart, cbTagInactivateEnd);
}


void nf2_chunk_TagInactivate_end(NIFFIOParser *pparser)
{
}

