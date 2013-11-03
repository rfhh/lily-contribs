
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "ID.h"

int             ID_current;             /* ID tag seen */


static RIFFIOSuccess
cbID(NIFFIOTagContext *pctxTag, niffID *p)
{
    if (cbTagStart(pctxTag, p, cbID)) {
        ID_current = *p;
        VPRINTF(" = %d", *p);
    }
    cbTagEnd(pctxTag);
    return RIFFIO_OK;
}


void nf2_tag_ID_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagID(pparser, NIFFIO_FOURCC_WILDCARD, cbID);
}


void nf2_tag_ID_end(NIFFIOParser *pparser)
{
}

