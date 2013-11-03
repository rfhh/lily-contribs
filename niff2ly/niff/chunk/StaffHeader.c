/*
  Copyright © 2013 Rutger F.H. Hofman. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY [LICENSOR] "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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

