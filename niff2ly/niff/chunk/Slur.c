
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <niffio.h>

#include "music/music.h"

#include "niff/tag/ID.h"
#include "niff/tag/NumberOfNodes.h"

#include "Chunk.h"

#include "Slur.h"


static RIFFIOSuccess
cbSlurStart(NIFFIOChunkContext *pctxChunk)
{
    return cbChunkStart(pctxChunk);
}

static RIFFIOSuccess
cbSlurEnd(NIFFIOChunkContext *pctxChunk)
{
    slur_p      s;

    if (ID_current >= n_slurs) {
        slurs_increase(ID_current);
    }

    s = &slur[ID_current];

    if (s->n == 0) {
        s->n = MultiN;
        s->pending = MultiN;
    }
    s->pending--;

    if (s->pending == s->n - 1) {
        stem_current->slur_start = ID_current;
    }
    if (s->pending == 0) {
        stem_current->slur_end = ID_current;
    }

    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Slur_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkSlur(pparser, cbSlurStart, cbSlurEnd);
}


void nf2_chunk_Slur_end(NIFFIOParser *pparser)
{
}

