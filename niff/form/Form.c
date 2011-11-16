
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

