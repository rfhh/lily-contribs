
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Part.h"


static void
part_start(int p, int nst)
{
    if (p >= n_part) {
	int	old_n_part = n_part;
	int	i;

	n_part = p + 1;
	part = realloc(part, n_part * sizeof(*part));
	for (i = old_n_part; i < n_part; i++) {
	    memset(&part[i], 0, sizeof(part[i]));
	}
	voice_index = realloc(voice_index, n_part * sizeof(*voice_index));
    }

    if (nst != part[p].n_staff) {
	part[p].n_staff = nst;
	part[p].staff   = calloc(nst, sizeof(*part[p].staff));
	voice_index[p] = &part[p].staff[0].unvoiced;
    }
}


static RIFFIOSuccess
cbPartStart(NIFFIOChunkContext *pctxChunk, niffPart *np)
{
    cbChunkStart(pctxChunk);

    part_start(np->partID, np->numberOfStaves);

    printSHORT(     np->partID,         "Part ID");
    printSTROFFSET( np->name,           "name");
    printSTROFFSET( np->abbreviation,   "abbreviation");
    printBYTE(      np->numberOfStaves, "number of staves");
    printSIGNEDBYTE(np->midiChannel,    "MIDI channel");
    printSIGNEDBYTE(np->midiCable,      "MIDI cable");
    printSIGNEDBYTE(np->transpose,      "transpose");
    VPRINTF(("\n"));

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbPartEnd(NIFFIOChunkContext *pctxChunk, niffPart *np)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Part_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkPart(pparser, cbPartStart, cbPartEnd);
    part_start(0, 1);
}


void nf2_chunk_Part_end(NIFFIOParser *pparser)
{
}

