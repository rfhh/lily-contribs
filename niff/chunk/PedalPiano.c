
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "PedalPiano.h"


static RIFFIOSuccess
cbPedalPianoStart(NIFFIOChunkContext *pctxChunk, niffPedalPiano *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("PedalPianoStart");
}


static RIFFIOSuccess
cbPedalPianoEnd(NIFFIOChunkContext *pctxChunk, niffPedalPiano *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("PedalPianoEnd");
}


void nf2_chunk_PedalPiano_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPedalPiano(pparser, cbPedalPianoStart, cbPedalPianoEnd);
}


void nf2_chunk_PedalPiano_end(NIFFIOParser *pparser)
{
}

