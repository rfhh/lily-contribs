
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "music/music.h"

#include "Chunk.h"

#include "niff/list/Staff.h"
#include "niff/tag/PartID.h"

#include "KeySignature.h"
#include "StaffHeader.h"


static RIFFIOSuccess
cbStaffHeaderStart(NIFFIOChunkContext *pctxChunk)
{
    symbol_p    key;

    cbChunkStart(pctxChunk);
    key = key_sign_create();
    key->symbol.key_sign.code = KEY_RESET;
    q_insert(voice_current, key);
    UNIMPLEMENTED_CHUNK("StaffHeaderStart");
}


static RIFFIOSuccess
cbStaffHeaderEnd(NIFFIOChunkContext *pctxChunk)
{
    if (partID_current != NO_PART_ID) {
        staff_partID = partID_current;
    }
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("StaffHeaderEnd");
}



void nf2_chunk_StaffHeader_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkStaffHeader(pparser, cbStaffHeaderStart, cbStaffHeaderEnd);
}


void nf2_chunk_StaffHeader_end(NIFFIOParser *pparser)
{
}

