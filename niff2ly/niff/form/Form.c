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

#include "Form.h"

#include "Form.h"


static RIFFIOSuccess
cbFormStart(NIFFIOChunkContext *pctxChunk)
{
    char strId[RIFFIO_FOURCC_LIM];
    char strType[RIFFIO_FOURCC_LIM];

    assert(pctxChunk != 0);
    assert(pctxChunk->pnf != 0);
    assert(pctxChunk->pchunk != 0);


    RIFFIOFOURCCToString(pctxChunk->pchunk->fccId, strId);
    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);

    VPRINTF("Start NIFF form %s ( %s size %ld", strId, strType, pctxChunk->pchunk->sizeData);

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbFormEnd(NIFFIOChunkContext *pctxChunk)
{
    assert (pctxChunk != 0);
    assert (pctxChunk->pnf != 0);
    assert (pctxChunk->pchunk != 0);

    VPRINTF("\n end NIFF form\n");

    return RIFFIO_OK;

}


void nf2_form_Form_init(NIFFIOParser *pparser)
{
    NIFFIORegisterForm(pparser, cbFormStart, cbFormEnd);
}


void nf2_form_Form_end(NIFFIOParser *pparser)
{
}

