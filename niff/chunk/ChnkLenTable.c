
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "ChnkLenTable.h"


static RIFFIOSuccess
cbChnkLenTable(NIFFIOChunkContext *pctxChunk)
{
    char strFOURCC[RIFFIO_FOURCC_LIM]; /* Buffer for printing FOURCCs */
    NIFFIOFile *pnf;
    RIFFIOChunk *pchunk;
    niffChklentabEntry entryCLT;

    cbChunkStart(pctxChunk);

    pnf = pctxChunk->pnf;
    pchunk = pctxChunk->pchunk;

    cbChunkStart(pctxChunk);

    while (! NIFFIOChunkDataEnd(pnf, pchunk))
    {
        NIFFIOReadniffChklentabEntry(pnf, & entryCLT);
        RIFFIOFOURCCToString(entryCLT.chunkName, strFOURCC);
        VPRINTF(("'%s' %5ldL\n", strFOURCC, entryCLT.offsetOfFirstTag));
    }

    VPRINTF(("')'\n"));

    VPRINTF(("\n"));

    cbChunkEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_chunk_ChnkLenTable_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkChnkLenTable(pparser, cbChnkLenTable);
}


void nf2_chunk_ChnkLenTable_end(NIFFIOParser *pparser)
{
}

