
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "SlashedStem.h"


static RIFFIOSuccess
cbSlashedStem(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbSlashedStem)) {
	VPRINTF((" /slashed stem"));
	stem_current->flags |= FLAG_SLASHED;
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("SlashedStem");
}


void nf2_tag_SlashedStem_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagSlashedStem(pparser, NIFFIO_FOURCC_WILDCARD, cbSlashedStem);
}


void nf2_tag_SlashedStem_end(NIFFIOParser *pparser)
{
}

