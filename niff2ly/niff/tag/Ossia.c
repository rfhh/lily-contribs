
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "Ossia.h"


static RIFFIOSuccess
cbOssia(NIFFIOTagContext *pctxTag, niffOssia *p)
{
    if (cbTagStart(pctxTag, p, cbOssia)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("Ossia");
}


void nf2_tag_Ossia_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagOssia(pparser, NIFFIO_FOURCC_WILDCARD, cbOssia);
}


void nf2_tag_Ossia_end(NIFFIOParser *pparser)
{
}

