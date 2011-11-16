
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Clef.h"


static symbol_p
clef_create(niffClef *p)
{
    symbol_p	s = symbol_create(t_current);
    clef_p	c = &s->symbol.clef;

    s->type = SYM_CLEF;
    c->shape = p->shape;
    c->step = p->staffStep;
    c->octave = p->octaveNumber;

    switch (c->shape) {
    case 1:	/* G clef */
	c->offset = c->step + OCTAVE_DIATON;
	break;
    case 2:	/* F clef */
	c->offset = c->step + 5 - 2 * OCTAVE_DIATON;
	break;
    case 3:	/* C clef */
	c->offset = c->step - 1;
	if (c->step == 4) {
	    c->offset += 0;			/* alto */
	} else if (c->step == 6) {
	    c->offset += -OCTAVE_DIATON + 3;	/* tenor */
	} else if (c->step == 0) {
	    c->offset += OCTAVE_DIATON + 1;	/* soprano */
	} else if (c->step == 2) {
	    c->offset += OCTAVE_DIATON;		/* mezzo-soprano */
	} else if (c->step == 8) {
	    c->offset += -2 * OCTAVE_DIATON;	/* baritone */
	}
	break;
    case 4:	/* percussion "clef" */
    case 5:	/* Double G clef (C'est quoi????) */
    case 6:	/* TAB for guitar tabulature */
    default:
        break;
    }

    VPRINTF("Insert a clef shape %d offset %d\n", c->shape, c->offset);

    return s;
}


static RIFFIOSuccess
cbClefStart(NIFFIOChunkContext *pctxChunk, niffClef *p)
{
    symbol_p	s;

    cbChunkStart(pctxChunk);

    s = clef_create(p);
    q_insert(voice_current, s);

    printSymbol(NIFFIOSymbolCLEFSHAPE, p->shape, "shape");
    printSIGNEDBYTE(p->staffStep, "staffStep");
    printSymbol(NIFFIOSymbolCLEFOCT, p->octaveNumber, "octaveNumber");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbClefEnd(NIFFIOChunkContext *pctxChunk, niffClef *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Clef_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkClef(pparser, cbClefStart, cbClefEnd);
}


void nf2_chunk_Clef_end(NIFFIOParser *pparser)
{
}

