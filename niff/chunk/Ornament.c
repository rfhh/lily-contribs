
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Ornament.h"

static symbol_p
ornament_create(void)
{
    symbol_p	s;

    s = symbol_create(t_current);
    s->type = SYM_ORNAMENT;

    symbol_current = s;

    return s;
}


static RIFFIOSuccess
cbOrnamentStart(NIFFIOChunkContext *pctxChunk, niffOrnament *p)
{
    symbol_p	s;

    s = ornament_create();
    s->symbol.ornament.shape = p->shape;

    cbChunkStart(pctxChunk);

    printSHORT(p->shape, "ornament shape");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbOrnamentEnd(NIFFIOChunkContext *pctxChunk, niffOrnament *p)
{
    q_insert(voice_current, symbol_current);

    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Ornament_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkOrnament(pparser, cbOrnamentStart, cbOrnamentEnd);
}


void nf2_chunk_Ornament_end(NIFFIOParser *pparser)
{
}

