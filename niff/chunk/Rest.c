
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"
#include "Stem.h"

#include "niff/tag/ID.h"

#include "Rest.h"


static symbol_p
rest_create(niffRest *p)
{
    symbol_p	s = symbol_create(t_current);
    note_p	n = &s->symbol.note;

    mpq_init(n->duration);
    rat2mpq(n->duration, &p->duration);

    if (mpq_cmp(n->duration, time_sig_current->duration) >= 0 &&
	! mpq_equal(t_current, t_measure_start)) {
	fprintf(stderr, "Meet SharpEye rest(measure) bug. Replace start time ");
	mpq_out_str(stderr, 10, t_current);
	fprintf(stderr, " with measure start time ");
	mpq_out_str(stderr, 10, t_measure_start);
	fprintf(stderr, "\n");
	mpq_set(s->start, t_measure_start);
    }

    s->type = SYM_NOTE;
    n->flags |= FLAG_REST;

    if (stem_current == NULL) {
#if VERBOSE
	VPRINTF(("\n ****** Get a Rest chunk without stem chunk??"));
#else
	fprintf(stderr, "Warning: ****** Get a Rest chunk without stem chunk??\n");
#endif
	stem_current = &stem_create()->symbol.stem;
    }
    n->tie_start = NO_ID;
    n->tie_end   = NO_ID;
    n->stem      = stem_current;

    return s;
}


static RIFFIOSuccess
cbRestStart(NIFFIOChunkContext *pctxChunk, niffRest *p)
{
    symbol_p n;

    n = rest_create(p);
    symbol_current = n;
    q_insert(voice_current, symbol_current);

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolREST, p->shape, "shape");
    printSIGNEDBYTE(p->staffStep,    "staff step");
    printRATIONAL  (p->duration, "duration");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbRestEnd(NIFFIOChunkContext *pctxChunk, niffRest *p)
{
    VPRINTF((" (tuplet ID = %d)", symbol_current->symbol.note.stem->tuplet));
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Rest_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkRest(pparser, cbRestStart, cbRestEnd);
}


void nf2_chunk_Rest_end(NIFFIOParser *pparser)
{
}

