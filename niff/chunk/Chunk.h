
#ifndef __NF2_CHUNK_Chunk_H__
#define __NF2_CHUNK_Chunk_H__

#include "niffio.h"

#include "niff/frontend.h"

#define UNIMPLEMENTED_CHUNK(name) \
	do { \
	    VPRINTF("Unimplemented chunk %s", name); \
	    return RIFFIO_OK; \
	} while (0)

RIFFIOSuccess cbChunkStart(NIFFIOChunkContext *pctxChunk);
RIFFIOSuccess cbChunkEnd(NIFFIOChunkContext *pctxChunk);

void nf2_chunk_Chunk_init(NIFFIOParser *pparser);
void nf2_chunk_Chunk_end(NIFFIOParser *pparser);

#endif
