
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "List.h"

#include "Staff.h"
#include "System.h"


static RIFFIOSuccess
cbSystemStart(NIFFIOChunkContext *pctxChunk)
{
    cbListStart(pctxChunk);

    staff_current = 0;
    voice_previous = NULL;

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbSystemEnd(NIFFIOChunkContext *pctxChunk)
{
    cbListEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_list_System_init(NIFFIOParser *pparser)
{
    NIFFIORegisterListSystem(pparser, cbSystemStart, cbSystemEnd);
}


void nf2_list_System_end(NIFFIOParser *pparser)
{
}

