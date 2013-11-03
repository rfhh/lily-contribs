
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Line.h"


static RIFFIOSuccess
cbLineStart(NIFFIOChunkContext *pctxChunk, niffLine *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("LineStart");
}


static RIFFIOSuccess
cbLineEnd(NIFFIOChunkContext *pctxChunk, niffLine *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("LineEnd");
}


void nf2_chunk_Line_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkLine(pparser, cbLineStart, cbLineEnd);
}


void nf2_chunk_Line_end(NIFFIOParser *pparser)
{
}

