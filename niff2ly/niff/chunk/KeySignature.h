
#ifndef __NF2_CHUNK_KeySignature_H__
#define __NF2_CHUNK_KeySignature_H__

#include "niffio.h"

#include "music/music.h"

symbol_p key_sign_create(void);

void nf2_chunk_KeySignature_init(NIFFIOParser *pparser);
void nf2_chunk_KeySignature_end(NIFFIOParser *pparser);

#endif
