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

#include "niff/tag/ID.h"

#include "Stem.h"


symbol_p
stem_create(void)
{
    symbol_p    y = symbol_create(t_current);
    stem_p      s = &y->symbol.stem;

    y->type = SYM_STEM;
    mpq_init(s->t);
    mpq_set(s->t, t_current);
    s->tuplet = NO_ID;
    s->beam   = NO_ID;
    s->slur_start = NO_ID;
    s->slur_end = NO_ID;
    s->articulations = NULL;

    return y;
}


static RIFFIOSuccess
cbStemStart(NIFFIOChunkContext *pctxChunk)
{
    symbol_p    s;

    cbChunkStart(pctxChunk);

    s = stem_create();
    stem_current = &s->symbol.stem;
    symbol_current = s;
    note_current = NULL;

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbStemEnd(NIFFIOChunkContext *pctxChunk)
{
    cbChunkEnd(pctxChunk);
    return RIFFIO_OK;
}


void nf2_chunk_Stem_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkStem(pparser, cbStemStart, cbStemEnd);
}


void nf2_chunk_Stem_end(NIFFIOParser *pparser)
{
}

