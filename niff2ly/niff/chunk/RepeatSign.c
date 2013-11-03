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

#include "RepeatSign.h"


static symbol_p
repeat_create(void)
{
    symbol_p    s;

    s = symbol_create(t_current);
    s->type = SYM_REPEAT;

    return s;
}


static RIFFIOSuccess
cbRepeatSignStart(NIFFIOChunkContext *pctxChunk, niffRepeatSign *p)
{
    symbol_p    s;

    cbChunkStart(pctxChunk);
    VPRINTF(" graphical = %d logical = %d", p->graphicalCode, p->logicalCode);

    s = repeat_create();
    s->symbol.repeat.graphic = p->graphicalCode;
    s->symbol.repeat.logic   = p->logicalCode;
    q_insert(voice_current, s);

    UNIMPLEMENTED_CHUNK("RepeatSignStart");
    return RIFFIO_OK;
}


static RIFFIOSuccess
cbRepeatSignEnd(NIFFIOChunkContext *pctxChunk, niffRepeatSign *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_RepeatSign_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkRepeatSign(pparser, cbRepeatSignStart, cbRepeatSignEnd);
}


void nf2_chunk_RepeatSign_end(NIFFIOParser *pparser)
{
}

