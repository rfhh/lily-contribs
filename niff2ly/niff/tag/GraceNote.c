
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"

#include "GraceNote.h"


static RIFFIOSuccess
cbGraceNote(NIFFIOTagContext *pctxTag, niffGraceNote *p)
{
    if (cbTagStart(pctxTag, p, cbGraceNote)) {
        if (symbol_current->type == SYM_NOTE) {
            symbol_current->symbol.note.flags |= FLAG_GRACE;
        } else if (symbol_current->type == SYM_STEM) {
            symbol_current->symbol.stem.flags |= FLAG_GRACE;
        }
    }
    cbTagEnd(pctxTag);

    return RIFFIO_OK;
}


void nf2_tag_GraceNote_init(NIFFIOParser *pparser)
{
    NIFFIORegisterTagGraceNote(pparser, NIFFIO_FOURCC_WILDCARD, cbGraceNote);
}


void nf2_tag_GraceNote_end(NIFFIOParser *pparser)
{
}

