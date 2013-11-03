
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "FontSymbol.h"


static RIFFIOSuccess
cbFontSymbolStart(NIFFIOChunkContext *pctxChunk, niffFontSymbol *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("FontSymbolStart");
}


static RIFFIOSuccess
cbFontSymbolEnd(NIFFIOChunkContext *pctxChunk, niffFontSymbol *pp)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("FontSymbolEnd");
}


void nf2_chunk_FontSymbol_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkFontSymbol(pparser, cbFontSymbolStart, cbFontSymbolEnd);
}


void nf2_chunk_FontSymbol_end(NIFFIOParser *pparser)
{
}

