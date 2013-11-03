
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "niff/tag/ID.h"

#include "Stem.h"


symbol_p
stem_create(void)
{
    symbol_p    y = symbol_create(t_current);
    stem_p      s = &y->symbol.stem;

    y->type = SYM_STEM;
    mpq_init(s->t);
    mpq_set(s->t, t_current);
    s->tuplet = NO_ID;
    s->beam   = NO_ID;
    s->slur_start = NO_ID;
    s->slur_end = NO_ID;
    s->articulations = NULL;

    return y;
}


static RIFFIOSuccess
cbStemStart(NIFFIOChunkContext *pctxChunk)
{
    symbol_p    s;

    cbChunkStart(pctxChunk);

    s = stem_create();
    stem_current = &s->symbol.stem;
    symbol_current = s;
    note_current = NULL;

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbStemEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    return RIFFIO_OK;
}


void nf2_chunk_Stem_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkStem(pparser, cbStemStart, cbStemEnd);
}


void nf2_chunk_Stem_end(NIFFIOParser *pparser)
{
}

