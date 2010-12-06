
#ifndef __NF2_TAG_PartID_H__
#define __NF2_TAG_PartID_H__

#include "niffio.h"

extern int		partID_current;		/* PartID tag seen */

void nf2_tag_PartID_init(NIFFIOParser *pparser);
void nf2_tag_PartID_end(NIFFIOParser *pparser);

#endif
