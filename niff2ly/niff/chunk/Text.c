
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Text.h"


static RIFFIOSuccess
cbTextStart(NIFFIOChunkContext *pctxChunk, niffText *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("TextStart");
}


static RIFFIOSuccess
cbTextEnd(NIFFIOChunkContext *pctxChunk, niffText *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("TextEnd");
}


void nf2_chunk_Text_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkText(pparser, cbTextStart, cbTextEnd);
}


void nf2_chunk_Text_end(NIFFIOParser *pparser)
{
}

