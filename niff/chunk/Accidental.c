
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Accidental.h"


static RIFFIOSuccess
cbAccidentalStart(NIFFIOChunkContext *pctxChunk, niffAccidental *p)
{
    cbChunkStart(pctxChunk);

    assert(symbol_current->type == SYM_NOTE);

    symbol_current->symbol.note.accidental = p->shape;

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbAccidentalEnd(NIFFIOChunkContext *pctxChunk, niffAccidental *p)
{
    cbChunkEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_chunk_Accidental_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkAccidental(pparser, cbAccidentalStart, cbAccidentalEnd);
}


void nf2_chunk_Accidental_end(NIFFIOParser *pparser)
{
}

