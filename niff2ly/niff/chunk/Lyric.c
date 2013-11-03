
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Lyric.h"


static RIFFIOSuccess
cbLyricStart(NIFFIOChunkContext *pctxChunk, niffLyric *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("LyricStart");
}


static RIFFIOSuccess
cbLyricEnd(NIFFIOChunkContext *pctxChunk, niffLyric *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("LyricEnd");
}


void nf2_chunk_Lyric_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkLyric(pparser, cbLyricStart, cbLyricEnd);
}


void nf2_chunk_Lyric_end(NIFFIOParser *pparser)
{
}

