
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "FontDescription.h"


static RIFFIOSuccess
cbFontDescriptionStart(NIFFIOChunkContext *pctxChunk, niffFontDescription *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("FontDescriptionStart");
}


static RIFFIOSuccess
cbFontDescriptionEnd(NIFFIOChunkContext *pctxChunk, niffFontDescription *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("FontDescriptionEnd");
}


void nf2_chunk_FontDescription_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkFontDescription(pparser, cbFontDescriptionStart, cbFontDescriptionEnd);
}


void nf2_chunk_FontDescription_end(NIFFIOParser *pparser)
{
}

