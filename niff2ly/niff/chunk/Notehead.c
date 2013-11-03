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

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "niff/tag/ID.h"

#include "Stem.h"
#include "Notehead.h"


static symbol_p
note_create(niffNotehead *p)
{
    symbol_p    s = symbol_create(t_current);
    note_p      n = &s->symbol.note;

    mpq_init(n->duration);
    rat2mpq(n->duration, &p->duration);

    if (stem_current == NULL) {
        fprintf(stderr, "\n ****** Get a NoteHead chunk without stem chunk??");
        stem_current = &stem_create()->symbol.stem;
    }

    s->type = SYM_NOTE;
    n->value = p->staffStep;
    n->stem = stem_current;
    n->flags = stem_current->flags & (FLAG_GRACE | FLAG_SMALL | FLAG_INVISIBLE);
    n->tie_start  = NO_ID;
    n->tie_end    = NO_ID;
    n->tuplet     = stem_current->tuplet;

    return s;
}


static RIFFIOSuccess
cbNoteheadStart(NIFFIOChunkContext *pctxChunk, niffNotehead *p)
{
    symbol_p n;

    n = note_create(p);
    symbol_current = n;
    note_current = n;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolNOTESHAPE, p->shape, "shape");
    printSIGNEDBYTE(p->staffStep,    "staff step");
    printRATIONAL(p->duration, "duration");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbNoteheadEnd(NIFFIOChunkContext *pctxChunk, niffNotehead *p)
{
    q_insert(voice_current, symbol_current);

    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Notehead_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkNotehead(pparser, cbNoteheadStart, cbNoteheadEnd);
}


void nf2_chunk_Notehead_end(NIFFIOParser *pparser)
{
}

