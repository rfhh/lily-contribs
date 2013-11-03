
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "niff/tag/ID.h"

#include "Chunk.h"

#include "Tuplet.h"




static RIFFIOSuccess
cbTupletStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);

    // UNIMPLEMENTED_CHUNK("TupletStart");
    return RIFFIO_OK;
}


static void
increase_tuplets(int ID)
{
    if (ID < n_tuplet) {
        return;
    }

    n_tuplet = ID + 1;
    global_tuplet = realloc(global_tuplet, n_tuplet * sizeof(*global_tuplet));
}


static void
tuplet_push(int *s, int t)
{
    global_tuplet[t].next = *s;
    *s = t;
}


static RIFFIOSuccess
cbTupletEnd(NIFFIOChunkContext *pctxChunk)
{
    tuplet_p    t;

    if (tuplet_current != NULL) {
        /* We met a TupletDescr tag */
        increase_tuplets(ID_current);
        t = &global_tuplet[ID_current];
        tuplet_current->ID = ID_current;
        *t = *tuplet_current;
        free(tuplet_current);
        tuplet_current = NULL;
    }

    if (note_current != NULL) {
        fprintf(stderr, "Oooppsss, the idea of a global tuplet stack doesn't match tuplets bound to notes. REFACTOR!\n");
        tuplet_push(&note_current->symbol.note.tuplet, ID_current);
    } else {
        assert(stem_current != NULL);
        tuplet_push(&stem_current->tuplet, ID_current);
    }

    cbChunkEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_chunk_Tuplet_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTuplet(pparser, cbTupletStart, cbTupletEnd);
}


void nf2_chunk_Tuplet_end(NIFFIOParser *pparser)
{
}

