#ifndef __NF2_LIST_List_H__
#define __NF2_LIST_List_H__

#include <niffio.h>

RIFFIOSuccess cbListStart(NIFFIOChunkContext *pctxChunk);
RIFFIOSuccess cbListEnd(NIFFIOChunkContext *pctxChunk);

void nf2_list_List_init(NIFFIOParser *pparser);
void nf2_list_List_end(NIFFIOParser *pparser);

#endif
