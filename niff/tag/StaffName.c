
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "StaffName.h"


static RIFFIOSuccess
cbStaffName(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbStaffName)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("StaffName");
}


void nf2_tag_StaffName_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagStaffName(pparser, NIFFIO_FOURCC_WILDCARD, cbStaffName);
}


void nf2_tag_StaffName_end(NIFFIOParser *pparser)
{
}

