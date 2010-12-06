
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "NumberOfFlags.h"


static RIFFIOSuccess
cbNumberOfFlags(NIFFIOTagContext *pctxTag, niffNumberOfFlags *p)
{
    assert(pctxTag != 0);
    assert(pctxTag->pnf != 0);
    assert(pctxTag->pchunkParent != 0);

    if (cbTagStart(pctxTag, p, cbNumberOfFlags)) {
	VPRINTF((" = %d", *p));
    }
    cbTagEnd(pctxTag);

    return RIFFIO_OK;
}


void nf2_tag_NumberOfFlags_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagNumberOfFlags(pparser, NIFFIO_FOURCC_WILDCARD, cbNumberOfFlags);
}


void nf2_tag_NumberOfFlags_end(NIFFIOParser *pparser)
{
}

