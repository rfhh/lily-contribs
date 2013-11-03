
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "FiguredBass.h"


static RIFFIOSuccess
cbFiguredBassStart(NIFFIOChunkContext *pctxChunk, niffFiguredBass *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("FiguredBassStart");
}


static RIFFIOSuccess
cbFiguredBassEnd(NIFFIOChunkContext *pctxChunk, niffFiguredBass *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("FiguredBassEnd");
}


void nf2_chunk_FiguredBass_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkFiguredBass(pparser, cbFiguredBassStart, cbFiguredBassEnd);
}


void nf2_chunk_FiguredBass_end(NIFFIOParser *pparser)
{
}

