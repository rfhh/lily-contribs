
#ifndef __NF2_TAG_ID_H__
#define __NF2_TAG_ID_H__

#include "niffio.h"

#define NO_ID           -1

extern int              ID_current;             /* ID tag seen */

void nf2_tag_ID_init(NIFFIOParser *pparser);
void nf2_tag_ID_end(NIFFIOParser *pparser);

#endif
