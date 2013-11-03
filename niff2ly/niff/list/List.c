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

#include "List.h"

#include "Staff.h"
#include "System.h"

#include "List.h"


/*
 * cbListStart
 * ===========
 * Print the beginning of a list
 */
RIFFIOSuccess
cbListStart(NIFFIOChunkContext *pctxChunk)
{
    char strId[RIFFIO_FOURCC_LIM];
    char strType[RIFFIO_FOURCC_LIM];

    assert (pctxChunk);
    assert(pctxChunk->pnf);
    assert(pctxChunk->pchunk);


    RIFFIOFOURCCToString(pctxChunk->pchunk->fccId, strId);
    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);


    VPRINTF("\n start LIST %s ",
            NIFFIONameListType(pctxChunk->pchunk->fccType));
    VPRINTF("'%s' ( '%s' sizeData = %ld",
            strId, strType, pctxChunk->pchunk->sizeData);

    return RIFFIO_OK;

}


RIFFIOSuccess
cbListEnd(NIFFIOChunkContext *pctxChunk)
{

    char strType[RIFFIO_FOURCC_LIM];

    assert (pctxChunk);
    assert(pctxChunk->pnf);
    assert(pctxChunk->pchunk);


    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);

    VPRINTF(" ) end LIST %s", NIFFIONameListType(pctxChunk->pchunk->fccType));

    return RIFFIO_OK;

}


void nf2_list_List_init(NIFFIOParser *pparser)
{
    NIFFIORegisterDefaultList(pparser, cbListStart, cbListEnd);

    nf2_list_Staff_init(pparser);
    nf2_list_System_init(pparser);
}


void nf2_list_List_end(NIFFIOParser *pparser)
{
    nf2_list_Staff_end(pparser);
    nf2_list_System_end(pparser);
}

