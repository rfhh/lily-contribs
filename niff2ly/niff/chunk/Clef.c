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

#include "Clef.h"


static symbol_p
clef_create(niffClef *p)
{
    symbol_p    s = symbol_create(t_current);
    clef_p      c = &s->symbol.clef;

    s->type = SYM_CLEF;
    c->shape = p->shape;
    c->step = p->staffStep;
    c->octave = p->octaveNumber;

    switch (c->shape) {
    case 1:     /* G clef */
        c->offset = c->step + OCTAVE_DIATON;
        break;
    case 2:     /* F clef */
        c->offset = c->step + 5 - 2 * OCTAVE_DIATON;
        break;
    case 3:     /* C clef */
        c->offset = c->step - 1;
        if (c->step == 4) {
            c->offset += 0;                     /* alto */
        } else if (c->step == 6) {
            c->offset += -OCTAVE_DIATON + 3;    /* tenor */
        } else if (c->step == 0) {
            c->offset += OCTAVE_DIATON + 1;     /* soprano */
        } else if (c->step == 2) {
            c->offset += OCTAVE_DIATON;         /* mezzo-soprano */
        } else if (c->step == 8) {
            c->offset += -2 * OCTAVE_DIATON;    /* baritone */
        }
        break;
    case 4:     /* percussion "clef" */
    case 5:     /* Double G clef (C'est quoi????) */
    case 6:     /* TAB for guitar tabulature */
    default:
        break;
    }

    VPRINTF("Insert a clef shape %d offset %d\n", c->shape, c->offset);

    return s;
}


static RIFFIOSuccess
cbClefStart(NIFFIOChunkContext *pctxChunk, niffClef *p)
{
    symbol_p    s;

    cbChunkStart(pctxChunk);

    s = clef_create(p);
    q_insert(voice_current, s);

    printSymbol(NIFFIOSymbolCLEFSHAPE, p->shape, "shape");
    printSIGNEDBYTE(p->staffStep, "staffStep");
    printSymbol(NIFFIOSymbolCLEFOCT, p->octaveNumber, "octaveNumber");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbClefEnd(NIFFIOChunkContext *pctxChunk, niffClef *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_Clef_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkClef(pparser, cbClefStart, cbClefEnd);
}


void nf2_chunk_Clef_end(NIFFIOParser *pparser)
{
}

