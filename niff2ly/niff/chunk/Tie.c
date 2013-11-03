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
#include <string.h>

#include <niffio.h>

#include "music/music.h"

#include "niff/tag/NumberOfNodes.h"
#include "niff/tag/ID.h"

#include "Chunk.h"

#include "Tie.h"


static RIFFIOSuccess
cbTieStart(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbTieEnd(NIFFIOChunkContext *pctxChunk)
{
    tie_p       tie;

    ties_increase(ID_current);
    tie = &ties[ID_current];
    tie->occurred++;

    if (symbol_current == NULL) {
        fprintf(stderr, "Your niff file is incorrect. Did you repair all rythmic flaws (blue triangles) in SharpEye?\n");
        exit(33);
    } else {
        assert(symbol_current->type == SYM_NOTE);

        if (tie->occurred == 1) {
            tie->occur = MultiN;
            symbol_current->symbol.note.tie_start = ID_current;
            tie->notes = calloc(tie->occur, sizeof(*tie->notes));
        } else if (tie->occur == tie->occurred) {
            symbol_current->symbol.note.tie_end = ID_current;
        }
        tie->notes[tie->occurred - 1] = &symbol_current->symbol.note;
    }

    cbChunkEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_chunk_Tie_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkTie(pparser, cbTieStart, cbTieEnd);
}


void nf2_chunk_Tie_end(NIFFIOParser *pparser)
{
}

