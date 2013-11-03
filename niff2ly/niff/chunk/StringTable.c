
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "StringTable.h"


static RIFFIOSuccess
cbStringTable(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("StringTable");
}


void nf2_chunk_StringTable_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkStringTable(pparser, cbStringTable);
}


void nf2_chunk_StringTable_end(NIFFIOParser *pparser)
{
}

