
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "niff/list/Staff.h"
#include "PartID.h"

int		partID_current;		/* PartID tag seen */


static RIFFIOSuccess
cbPartID(NIFFIOTagContext *pctxTag, niffPartID *p)
{
    staff_p	s;

    if (cbTagStart(pctxTag, p, cbPartID)) {
	VPRINTF(("=%d", *p));

	partID_current = *p;
	part_current  = &part[*p];
	s = part_current->staff;
	if (voice_current == &voice_unparted) {
	    symbol_p	scan;

	    if (staff_current != 0 &&
		voice_previous != &s[staff_current - 1].unvoiced) {
		staff_current = 0;
	    }
	    voice_current = &s[staff_current].unvoiced;
	    voice_previous = voice_current;

	    while ((scan = voice_unparted.front) != NULL) {
		q_remove(&voice_unparted, scan);
		q_insert(voice_current, scan);
	    }
	}
    }
    cbTagEnd(pctxTag);
    return RIFFIO_OK;
}


void nf2_tag_PartID_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagPartID(pparser, NIFFIO_FOURCC_WILDCARD, cbPartID);
}


void nf2_tag_PartID_end(NIFFIOParser *pparser)
{
}

