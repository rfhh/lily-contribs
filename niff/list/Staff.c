
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "List.h"

#include "Staff.h"

#include "niff/tag/Tag.h"


int		staff_current;
int		staff_partID;		/* PartID for whole staff */


static RIFFIOSuccess
cbStaffStart(NIFFIOChunkContext *pctxChunk)
{
    cbListStart(pctxChunk);

    voice_current = &voice_unparted;
    staff_current++;
    staff_partID = NO_PART_ID;
fprintf(stderr, "%s partID %d\n", __func__, staff_current);

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbStaffEnd(NIFFIOChunkContext *pctxChunk)
{
    tag_activate_reset();

    cbListEnd(pctxChunk);

    return RIFFIO_OK;
}


void nf2_list_Staff_init(NIFFIOParser *pparser)
{
    NIFFIORegisterListStaff(pparser, cbStaffStart, cbStaffEnd);
}


void nf2_list_Staff_end(NIFFIOParser *pparser)
{
}

