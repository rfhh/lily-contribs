
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "niff/tag/ID.h"
#include "Beam.h"



static RIFFIOSuccess
cbBeamStart(NIFFIOChunkContext *pctxChunk, niffBeam *p)
{
    cbChunkStart(pctxChunk);

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbBeamEnd(NIFFIOChunkContext *pctxChunk, niffBeam *p)
{
    assert(stem_current != NULL);
    stem_current->beam = ID_current;
    stem_current->beam_left = p->beamPartsToLeft;
    stem_current->beam_right = p->beamPartsToRight;

    cbChunkEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_chunk_Beam_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkBeam(pparser, cbBeamStart, cbBeamEnd);
}


void nf2_chunk_Beam_end(NIFFIOParser *pparser)
{
}

