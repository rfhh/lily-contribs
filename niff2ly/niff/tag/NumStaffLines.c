
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "NumStaffLines.h"


static RIFFIOSuccess
cbNumStaffLines(NIFFIOTagContext *pctxTag, niffNumStaffLines *p)
{
    if (cbTagStart(pctxTag, p, cbNumStaffLines)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("NumStaffLines");
}


void nf2_tag_NumStaffLines_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagNumStaffLines(pparser, NIFFIO_FOURCC_WILDCARD, cbNumStaffLines);
}


void nf2_tag_NumStaffLines_end(NIFFIOParser *pparser)
{
}

