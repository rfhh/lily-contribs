
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "TagActivate.h"

#include "niff/tag/Tag.h"



static RIFFIOSuccess
cbTagActivateStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    tag_activate_start();
    return RIFFIO_OK;
}


static RIFFIOSuccess
cbTagActivateEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    tag_activate_stop();
    return RIFFIO_OK;
}


void nf2_chunk_TagActivate_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTagActivate(pparser, cbTagActivateStart, cbTagActivateEnd);
}


void nf2_chunk_TagActivate_end(NIFFIOParser *pparser)
{
}

