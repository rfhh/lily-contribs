
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

typedef struct SLUR {
    int		n;
    int		pending;
} slur_t, *slur_p;


static slur_p	slur;
static int	n_slur;


static RIFFIOSuccess
cbSlurStart(NIFFIOChunkContext *pctxChunk)
{
    return cbChunkStart(pctxChunk);
}

static RIFFIOSuccess
cbSlurEnd(NIFFIOChunkContext *pctxChunk)
{
    slur_p	s;

    if (ID_current >= n_slur) {
	int	old_n_slur = n_slur;
	int	i;

	n_slur = ID_current + 1;
	slur = realloc(slur, n_slur * sizeof(*slur));
	for (i = old_n_slur; i < n_slur; i++) {
	    memset(&slur[i], 0, sizeof(*slur));
	}
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

