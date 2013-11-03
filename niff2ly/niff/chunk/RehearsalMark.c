
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "RehearsalMark.h"


static RIFFIOSuccess
cbRehearsalMarkStart(NIFFIOChunkContext *pctxChunk, niffRehearsalMark *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("RehearsalMarkStart");
}


static RIFFIOSuccess
cbRehearsalMarkEnd(NIFFIOChunkContext *pctxChunk, niffRehearsalMark *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("RehearsalMarkEnd");
}


void nf2_chunk_RehearsalMark_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkRehearsalMark(pparser, cbRehearsalMarkStart, cbRehearsalMarkEnd);
}


void nf2_chunk_RehearsalMark_end(NIFFIOParser *pparser)
{
}

