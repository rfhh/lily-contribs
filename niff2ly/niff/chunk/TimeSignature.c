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

#include "TimeSignature.h"


static symbol_p
time_sig_create(void)
{
    symbol_p    s = symbol_create(t_current);

    s->type = SYM_TIME_SIGNATURE;
    mpq_init(s->symbol.time_signature.duration);

    return s;
}


static RIFFIOSuccess
cbTimeSignatureStart(NIFFIOChunkContext *pctxChunk, niffTimeSignature *p)
{
    symbol_p    s;
    RATIONAL    r;

    cbChunkStart(pctxChunk);

    s = time_sig_create();
    time_sig_current = &s->symbol.time_signature;
    time_sig_current->top = p->topNumber;
    time_sig_current->bottom = p->bottomNumber;
    if (p->topNumber == -1) {
        r.numerator = 4;
        r.denominator = 4;
    } else if (p->topNumber == -2) {
        r.numerator = 2;
        r.denominator = 2;
    } else {
        r.numerator = p->topNumber;
        r.denominator = p->bottomNumber;
    }
    rat2mpq(time_sig_current->duration, &r);

    q_insert(voice_current, s);

    printSIGNEDBYTE(p->topNumber,    "top number");
    printBYTE      (p->bottomNumber, "bottom number");

    return RIFFIO_OK;

}

static RIFFIOSuccess
cbTimeSignatureEnd(NIFFIOChunkContext *pctxChunk, niffTimeSignature *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_TimeSignature_init(NIFFIOParser *pparser)
{
    symbol_p    s;
    RATIONAL    r;

    NIFFIORegisterChunkTimeSignature(pparser, cbTimeSignatureStart, cbTimeSignatureEnd);

    s = time_sig_create();
    time_sig_current = &s->symbol.time_signature;
    r.numerator = 4;
    r.denominator = 4;
    time_sig_current->top = r.numerator;
    time_sig_current->bottom = r.denominator;
    rat2mpq(time_sig_current->duration, &r);
}


void nf2_chunk_TimeSignature_end(NIFFIOParser *pparser)
{
}

