
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "TimeSlice.h"


static symbol_p
bar_start_create(void)
{
    symbol_p	s = symbol_create(t_current);

    s->type = SYM_BAR_START;

    return s;
}


static RIFFIOSuccess
cbTimeSliceStart(NIFFIOChunkContext *pctxChunk, niffTimeSlice *p)
{
    cbChunkStart(pctxChunk);

    if (p->type == 1) {
	symbol_p s = bar_start_create();
	rat2mpq(t_measure_start, &p->startTime);
	q_insert(voice_current, s);
    } else {
	assert(p->type == 2);
	rat2mpq(t_current, &p->startTime);
	mpq_add(t_current, t_current, t_measure_start);
    }
    stem_current = NULL;	/* Reset it, something new has begun */
    symbol_current = NULL;	/* Reset it, something new has begun */

    printSymbol(NIFFIOSymbolTS, p->type, "type");
    printRATIONAL(p->startTime, "start time");

    return RIFFIO_OK;

}


static RIFFIOSuccess
cbTimeSliceEnd(NIFFIOChunkContext *pctxChunk, niffTimeSlice *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_TimeSlice_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTimeSlice(pparser, cbTimeSliceStart, cbTimeSliceEnd);
}


void nf2_chunk_TimeSlice_end(NIFFIOParser *pparser)
{
}

