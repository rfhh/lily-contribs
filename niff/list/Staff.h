
#ifndef __NF2_LIST_Staff_H__
#define __NF2_LIST_Staff_H__

#include "niffio.h"

#define NO_PART_ID      -1

extern int              staff_current;
extern int              staff_partID;           /* PartID for whole staff */

void nf2_list_Staff_init(NIFFIOParser *pparser);
void nf2_list_Staff_end(NIFFIOParser *pparser);

#endif
