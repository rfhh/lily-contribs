/*
  Copyright © 2013 Rutger F.H. Hofman. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY [LICENSOR] "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __NF2_TAG_Tag_H__
#define __NF2_TAG_Tag_H__

#include "niffio.h"

#include "niff/frontend.h"


#define UNIMPLEMENTED_TAG(name) \
        do { \
            VPRINTF(" /unimplemented tag %s;", name); \
            return RIFFIO_OK; \
        } while (0)

void tag_activate_reset(void);
void tag_activate_start(void);
void tag_activate_stop(void);
void tag_activate_apply(void);

void tag_inactivate_start(void);
void tag_inactivate_stop(void);

void tags_reset(void);

RIFFIOSuccess cbTagStart(NIFFIOTagContext *pctxTag, void *p, void *callback);
RIFFIOSuccess cbTagEnd(NIFFIOTagContext *pctxTag);

void nf2_tag_Tag_init(NIFFIOParser *pparser);
void nf2_tag_Tag_end(NIFFIOParser *pparser);

#endif
