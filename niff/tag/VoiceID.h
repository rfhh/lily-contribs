
#ifndef __NF2_TAG_VoiceID_H__
#define __NF2_TAG_VoiceID_H__

#include "niffio.h"

#define NO_VOICE_ID	-1

extern int		voiceID_current;	/* VoiceID tag seen */

void nf2_tag_VoiceID_init(NIFFIOParser *pparser);
void nf2_tag_VoiceID_end(NIFFIOParser *pparser);

#endif
