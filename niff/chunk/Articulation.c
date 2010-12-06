
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Articulation.h"


static symbol_p
articulation_create(void)
{
    symbol_p	s;

    s = symbol_create(t_current);
    s->type = SYM_ARTICULATION;

    symbol_current = s;

    return s;
}


static RIFFIOSuccess
cbArticulationStart(NIFFIOChunkContext *pctxChunk, niffArticulation *p)
{
    symbol_p	s;

    s = articulation_create();
    s->symbol.articulation.shape = p->shape;

    cbChunkStart(pctxChunk);

    printSHORT(p->shape, "articulation shape");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbArticulationEnd(NIFFIOChunkContext *pctxChunk, niffArticulation *p)
{
    symbol_current->next = stem_current->articulations;
    stem_current->articulations = symbol_current;

    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Articulation_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkArticulation(pparser, cbArticulationStart, cbArticulationEnd);
}


void nf2_chunk_Articulation_end(NIFFIOParser *pparser)
{
}

