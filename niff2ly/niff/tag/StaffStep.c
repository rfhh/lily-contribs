
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "StaffStep.h"


static RIFFIOSuccess
cbStaffStep(NIFFIOTagContext *pctxTag, niffStaffStep *p)
{
    if (cbTagStart(pctxTag, p, cbStaffStep)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("StaffStep");
}


void nf2_tag_StaffStep_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagStaffStep(pparser, NIFFIO_FOURCC_WILDCARD, cbStaffStep);
}


void nf2_tag_StaffStep_end(NIFFIOParser *pparser)
{
}

