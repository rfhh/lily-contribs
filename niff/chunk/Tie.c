
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

int	n_ties;
tie_p	ties;


static void
increase_ties(int ID)
{
    int		i;
    int		old_n;

    if (ID < n_ties) {
	return;
    }

    old_n = n_ties;
    n_ties = ID + 1;
    ties = realloc(ties, n_ties * sizeof(*ties));
    for (i = old_n; i < n_ties; i++) {
	memset(&ties[i], 0, sizeof(*ties));
    }
}


static RIFFIOSuccess
cbTieStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbTieEnd(NIFFIOChunkContext *pctxChunk)
{
    tie_p	tie;

    increase_ties(ID_current);
    tie = &ties[ID_current];
    tie->occurred++;

    assert(symbol_current->type == SYM_NOTE);

    if (tie->occurred == 1) {
	tie->occur = MultiN;
	symbol_current->symbol.note.tie_start = ID_current;
	tie->notes = calloc(tie->occur, sizeof(*tie->notes));
    } else if (tie->occur == tie->occurred) {
	symbol_current->symbol.note.tie_end = ID_current;
    }
    tie->notes[tie->occurred - 1] = &symbol_current->symbol.note;

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

