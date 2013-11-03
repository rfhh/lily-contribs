
#ifndef __NF2_CHUNK_Stem_H__
#define __NF2_CHUNK_Stem_H__

#include "niffio.h"

symbol_p stem_create(void);

void nf2_chunk_Stem_init(NIFFIOParser *pparser);
void nf2_chunk_Stem_end(NIFFIOParser *pparser);

#endif
