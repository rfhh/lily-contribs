
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "OctaveSign.h"


static RIFFIOSuccess
cbOctaveSignStart(NIFFIOChunkContext *pctxChunk, niffOctaveSign *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("OctaveSignStart");
}


static RIFFIOSuccess
cbOctaveSignEnd(NIFFIOChunkContext *pctxChunk, niffOctaveSign *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("OctaveSignEnd");
}


void nf2_chunk_OctaveSign_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkOctaveSign(pparser, cbOctaveSignStart, cbOctaveSignEnd);
}


void nf2_chunk_OctaveSign_end(NIFFIOParser *pparser)
{
}

