
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "VoiceID.h"


int             voiceID_current;        /* VoiceID tag seen */


static RIFFIOSuccess
cbVoiceID(NIFFIOTagContext *pctxTag, niffVoiceID *p)
{
    if (cbTagStart(pctxTag, p, cbVoiceID)) {
        voiceID_current = *p;
    }
    cbTagEnd(pctxTag);
    UNIMPLEMENTED_TAG("VoiceID");
}


void nf2_tag_VoiceID_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagVoiceID(pparser, NIFFIO_FOURCC_WILDCARD, cbVoiceID);
}


void nf2_tag_VoiceID_end(NIFFIOParser *pparser)
{
}

