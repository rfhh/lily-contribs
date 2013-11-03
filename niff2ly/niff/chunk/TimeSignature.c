
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "TimeSignature.h"


static symbol_p
time_sig_create(void)
{
    symbol_p    s = symbol_create(t_current);

    s->type = SYM_TIME_SIGNATURE;
    mpq_init(s->symbol.time_signature.duration);

    return s;
}


static RIFFIOSuccess
cbTimeSignatureStart(NIFFIOChunkContext *pctxChunk, niffTimeSignature *p)
{
    symbol_p    s;
    RATIONAL    r;

    cbChunkStart(pctxChunk);

    s = time_sig_create();
    time_sig_current = &s->symbol.time_signature;
    time_sig_current->top = p->topNumber;
    time_sig_current->bottom = p->bottomNumber;
    if (p->topNumber == -1) {
        r.numerator = 4;
        r.denominator = 4;
    } else if (p->topNumber == -2) {
        r.numerator = 2;
        r.denominator = 2;
    } else {
        r.numerator = p->topNumber;
        r.denominator = p->bottomNumber;
    }
    rat2mpq(time_sig_current->duration, &r);

    q_insert(voice_current, s);

    printSIGNEDBYTE(p->topNumber,    "top number");
    printBYTE      (p->bottomNumber, "bottom number");

    return RIFFIO_OK;

}

static RIFFIOSuccess
cbTimeSignatureEnd(NIFFIOChunkContext *pctxChunk, niffTimeSignature *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_TimeSignature_init(NIFFIOParser *pparser)
{
    symbol_p    s;
    RATIONAL    r;

    NIFFIORegisterChunkTimeSignature(pparser, cbTimeSignatureStart, cbTimeSignatureEnd);

    s = time_sig_create();
    time_sig_current = &s->symbol.time_signature;
    r.numerator = 4;
    r.denominator = 4;
    time_sig_current->top = r.numerator;
    time_sig_current->bottom = r.denominator;
    rat2mpq(time_sig_current->duration, &r);
}


void nf2_chunk_TimeSignature_end(NIFFIOParser *pparser)
{
}

