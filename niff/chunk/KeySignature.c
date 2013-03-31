
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "KeySignature.h"


symbol_p
key_sign_create(void)
{
    symbol_p    s;

    s = symbol_create(t_current);
    s->type = SYM_KEY_SIGN;

    return s;
}


static RIFFIOSuccess
cbKeySignatureStart(NIFFIOChunkContext *pctxChunk, niffKeySignature *p)
{
    symbol_p    key;

    key = key_sign_create();
    key->symbol.key_sign.code = p->standardCode;
    q_insert(voice_current, key);

    cbChunkStart(pctxChunk);

    printSIGNEDBYTE(p->standardCode, "standardCode");

    return RIFFIO_OK;
}


static RIFFIOSuccess
cbKeySignatureEnd(NIFFIOChunkContext *pctxChunk, niffKeySignature *p)
{
    return cbChunkEnd(pctxChunk);
}


void nf2_chunk_KeySignature_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkKeySignature(pparser, cbKeySignatureStart, cbKeySignatureEnd);
}


void nf2_chunk_KeySignature_end(NIFFIOParser *pparser)
{
}

