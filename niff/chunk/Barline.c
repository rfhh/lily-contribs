
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Barline.h"


static RIFFIOSuccess
cbBarlineStart(NIFFIOChunkContext *pctxChunk, niffBarline *p)
{
    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolBARTYPE, p->type, "type");
    printSymbol(NIFFIOSymbolBAREXT, p->extendsTo, "extends to");
    printSHORT(p->numberOfStaves, "number of staves");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbBarlineEnd(NIFFIOChunkContext *pctxChunk, niffBarline *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Barline_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkBarline(pparser, cbBarlineStart, cbBarlineEnd);
}


void nf2_chunk_Barline_end(NIFFIOParser *pparser)
{
}

