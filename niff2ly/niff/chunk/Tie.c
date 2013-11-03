
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <niffio.h>

#include "music/music.h"

#include "niff/tag/NumberOfNodes.h"
#include "niff/tag/ID.h"

#include "Chunk.h"

#include "Tie.h"


static RIFFIOSuccess
cbTieStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbTieEnd(NIFFIOChunkContext *pctxChunk)
{
    tie_p       tie;

    ties_increase(ID_current);
    tie = &ties[ID_current];
    tie->occurred++;

    if (symbol_current == NULL) {
        fprintf(stderr, "Your niff file is incorrect. Did you repair all rythmic flaws (blue triangles) in SharpEye?\n");
        exit(33);
    } else {
        assert(symbol_current->type == SYM_NOTE);

        if (tie->occurred == 1) {
            tie->occur = MultiN;
            symbol_current->symbol.note.tie_start = ID_current;
            tie->notes = calloc(tie->occur, sizeof(*tie->notes));
        } else if (tie->occur == tie->occurred) {
            symbol_current->symbol.note.tie_end = ID_current;
        }
        tie->notes[tie->occurred - 1] = &symbol_current->symbol.note;
    }

    cbChunkEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_chunk_Tie_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTie(pparser, cbTieStart, cbTieEnd);
}


void nf2_chunk_Tie_end(NIFFIOParser *pparser)
{
}

