
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "CustomGraphicChk.h"


static RIFFIOSuccess
cbCustomGraphicChkStart(NIFFIOChunkContext *pctxChunk, niffCustomGraphicChk *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("CustomGraphicChkStart");
}


static RIFFIOSuccess
cbCustomGraphicChkEnd(NIFFIOChunkContext *pctxChunk, niffCustomGraphicChk *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("CustomGraphicChkEnd");
}


void nf2_chunk_CustomGraphicChk_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkCustomGraphicChk(pparser, cbCustomGraphicChkStart, cbCustomGraphicChkEnd);
}


void nf2_chunk_CustomGraphicChk_end(NIFFIOParser *pparser)
{
}

