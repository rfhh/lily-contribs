
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "MeasureNumbering.h"


static RIFFIOSuccess
cbMeasureNumberingStart(NIFFIOChunkContext *pctxChunk, niffMeasureNumbering *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("MeasureNumberingStart");
}


static RIFFIOSuccess
cbMeasureNumberingEnd(NIFFIOChunkContext *pctxChunk, niffMeasureNumbering *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("MeasureNumberingEnd");
}


void nf2_chunk_MeasureNumbering_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkMeasureNumbering(pparser, cbMeasureNumberingStart, cbMeasureNumberingEnd);
}


void nf2_chunk_MeasureNumbering_end(NIFFIOParser *pparser)
{
}

