
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "RepeatSign.h"


static symbol_p
repeat_create(void)
{
    symbol_p	s;

    s = symbol_create(t_current);
    s->type = SYM_REPEAT;

    return s;
}


static RIFFIOSuccess
cbRepeatSignStart(NIFFIOChunkContext *pctxChunk, niffRepeatSign *p)
{
    symbol_p	s;

    cbChunkStart(pctxChunk);
    VPRINTF((" graphical = %d logical = %d", p->graphicalCode, p->logicalCode));

    s = repeat_create();
    s->symbol.repeat.graphic = p->graphicalCode;
    s->symbol.repeat.logic   = p->logicalCode;
    q_insert(voice_current, s);

    UNIMPLEMENTED_CHUNK("RepeatSignStart");
    return RIFFIO_OK;
}


static RIFFIOSuccess
cbRepeatSignEnd(NIFFIOChunkContext *pctxChunk, niffRepeatSign *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_RepeatSign_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkRepeatSign(pparser, cbRepeatSignStart, cbRepeatSignEnd);
}


void nf2_chunk_RepeatSign_end(NIFFIOParser *pparser)
{
}

