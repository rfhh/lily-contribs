
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Arpeggio.h"


static RIFFIOSuccess
cbArpeggioStart(NIFFIOChunkContext *pctxChunk, niffArpeggio *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("ArpeggioStart");
}


static RIFFIOSuccess
cbArpeggioEnd(NIFFIOChunkContext *pctxChunk, niffArpeggio *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("ArpeggioEnd");
}


void nf2_chunk_Arpeggio_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkArpeggio(pparser, cbArpeggioStart, cbArpeggioEnd);
}


void nf2_chunk_Arpeggio_end(NIFFIOParser *pparser)
{
}

