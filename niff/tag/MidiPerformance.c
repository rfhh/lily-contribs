
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "MidiPerformance.h"


static RIFFIOSuccess
cbMidiPerformance(NIFFIOTagContext *pctxTag, niffMidiPerformance *p)
{
    if (cbTagStart(pctxTag, p, cbMidiPerformance)) {
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("MidiPerformance");
}


void nf2_tag_MidiPerformance_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagMidiPerformance(pparser, NIFFIO_FOURCC_WILDCARD, cbMidiPerformance);
}


void nf2_tag_MidiPerformance_end(NIFFIOParser *pparser)
{
}

