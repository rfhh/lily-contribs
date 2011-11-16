
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "niff/tag/ID.h"

#include "Stem.h"
#include "Notehead.h"


static symbol_p
note_create(niffNotehead *p)
{
    symbol_p	s = symbol_create(t_current);
    note_p	n = &s->symbol.note;

    mpq_init(n->duration);
    rat2mpq(n->duration, &p->duration);

    if (stem_current == NULL) {
	fprintf(stderr, "\n ****** Get a NoteHead chunk without stem chunk??");
	stem_current = &stem_create()->symbol.stem;
    }

    s->type = SYM_NOTE;
    n->value = p->staffStep;
    n->stem = stem_current;
    n->flags = stem_current->flags & (FLAG_GRACE | FLAG_SMALL | FLAG_INVISIBLE);
    n->tie_start  = NO_ID;
    n->tie_end    = NO_ID;
    n->tuplet     = stem_current->tuplet;

    return s;
}


static RIFFIOSuccess
cbNoteheadStart(NIFFIOChunkContext *pctxChunk, niffNotehead *p)
{
    symbol_p n;

    n = note_create(p);
    symbol_current = n;
    note_current = n;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolNOTESHAPE, p->shape, "shape");
    printSIGNEDBYTE(p->staffStep,    "staff step");
    printRATIONAL(p->duration, "duration");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbNoteheadEnd(NIFFIOChunkContext *pctxChunk, niffNotehead *p)
{
    q_insert(voice_current, symbol_current);

    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Notehead_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkNotehead(pparser, cbNoteheadStart, cbNoteheadEnd);
}


void nf2_chunk_Notehead_end(NIFFIOParser *pparser)
{
}

