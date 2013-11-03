
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Parenthesis.h"


static RIFFIOSuccess
cbParenthesisStart(NIFFIOChunkContext *pctxChunk, niffParenthesis *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("ParenthesisStart");
}


static RIFFIOSuccess
cbParenthesisEnd(NIFFIOChunkContext *pctxChunk, niffParenthesis *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("ParenthesisEnd");
}


void nf2_chunk_Parenthesis_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkParenthesis(pparser, cbParenthesisStart, cbParenthesisEnd);
}


void nf2_chunk_Parenthesis_end(NIFFIOParser *pparser)
{
}

