
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

