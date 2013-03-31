
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "TupletDesc.h"


static RIFFIOSuccess
cbTupletDesc(NIFFIOTagContext *pctxTag, niffTupletDesc *p)
{
    static int initialized = 0;
    static mpq_t r1;
    static mpq_t r2;

    if (cbTagStart(pctxTag, p, cbTupletDesc)) {

        if (! initialized) {
            mpq_init(r1);
            mpq_init(r2);
            initialized = 1;
        }

        tuplet_current = malloc(sizeof(*tuplet_current));
        mpq_init(tuplet_current->ratio);
        rat2mpq(r1, &p->transformRatioAB);
        rat2mpq(r2, &p->transformRatioCD);
        mpq_div(tuplet_current->ratio, r2, r1);
        tuplet_current->den = p->transformRatioAB.numerator;
        tuplet_current->num = 1;
        while (tuplet_current->num < tuplet_current->den) {
            tuplet_current->num *= 2;
        }
        tuplet_current->num /= 2;
    }

    cbTagEnd(pctxTag);

    return RIFFIO_OK;
}


void nf2_tag_TupletDesc_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagTupletDesc(pparser, NIFFIO_FOURCC_WILDCARD, cbTupletDesc);
}


void nf2_tag_TupletDesc_end(NIFFIOParser *pparser)
{
}

