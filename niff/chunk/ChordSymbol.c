
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "ChordSymbol.h"


static RIFFIOSuccess
cbChordSymbolStart(NIFFIOChunkContext *pctxChunk, niffChordSymbol *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("ChordSymbolStart");
}


static RIFFIOSuccess
cbChordSymbolEnd(NIFFIOChunkContext *pctxChunk, niffChordSymbol *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("ChordSymbolEnd");
}


void nf2_chunk_ChordSymbol_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkChordSymbol(pparser, cbChordSymbolStart, cbChordSymbolEnd);
}


void nf2_chunk_ChordSymbol_end(NIFFIOParser *pparser)
{
}

