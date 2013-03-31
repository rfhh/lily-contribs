
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "NumberOfNodes.h"


int MultiN;


static RIFFIOSuccess
cbNumberOfNodes(NIFFIOTagContext *pctxTag, niffNumberOfNodes *p)
{
    if (cbTagStart(pctxTag, p, cbNumberOfNodes)) {
        VPRINTF(" = %d", *p);
        MultiN = *p;
    }
    cbTagEnd(pctxTag);

    return RIFFIO_OK;
}


void nf2_tag_NumberOfNodes_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagNumberOfNodes(pparser, NIFFIO_FOURCC_WILDCARD, cbNumberOfNodes);
}


void nf2_tag_NumberOfNodes_end(NIFFIOParser *pparser)
{
}

