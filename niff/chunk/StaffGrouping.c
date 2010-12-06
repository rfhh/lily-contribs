
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "StaffGrouping.h"


static RIFFIOSuccess
cbStaffGroupingStart(NIFFIOChunkContext *pctxChunk, niffStaffGrouping *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("StaffGroupingStart");
}


static RIFFIOSuccess
cbStaffGroupingEnd(NIFFIOChunkContext *pctxChunk, niffStaffGrouping *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("StaffGroupingEnd");
}


void nf2_chunk_StaffGrouping_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkStaffGrouping(pparser, cbStaffGroupingStart, cbStaffGroupingEnd);
}


void nf2_chunk_StaffGrouping_end(NIFFIOParser *pparser)
{
}

