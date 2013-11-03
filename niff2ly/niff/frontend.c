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

#ifndef lint
static const char rcsid[] =
"$Id: frontend.c,v 1.6 2003/04/01 13:27:06 rutger Exp $";
#endif

/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include <gmp.h>

#include "music/music.h"

#include "frontend.h"

#include "form/Form.h"
#include "list/List.h"
#include "chunk/Chunk.h"
#include "tag/Tag.h"


part_p          part_current;
symbol_p        symbol_current;
symbol_p        note_current;

symbol_q_p      voice_current;
symbol_q_p      voice_previous;
symbol_q_t      voice_unparted;

symbol_q_p      *voice_index;

time_signature_p        time_sig_current;

mpq_t           t_measure_start;        /* Most recent measure start */
mpq_t           t_current;              /* Most recent time slice */
stem_p          stem_current;           /* Remember for chording and voicing */


void
printSHORT(SHORT h, const char *strComment)
{
    VPRINTF(" %s=%d", strComment, h);
}


void
printSTROFFSET(STROFFSET offset, const char *strComment)
{
    VPRINTF(" // %s\n", strComment);
}

void
printRATIONAL(RATIONAL r, const char *strComment)
{
    VPRINTF(" %s=%d/%d", strComment, r.numerator, r.denominator);
}

void
printBYTE(BYTE b, const char *strComment)
{
    VPRINTF(" %s=%u", strComment, b);
}


void
printSIGNEDBYTE(SIGNEDBYTE sb, const char *strComment)
{
    VPRINTF(" %s=%d", strComment, sb);
}



void
printSymbol( const char * (decoder(BYTE)),
             BYTE symbol, const char *strComment)
{

    /* Do we know about this value */
    if (! decoder(symbol))
    {
        VPRINTF(" WARNING: UNKNOWN VALUE for %s:", strComment);
        printBYTE(symbol, strComment);
        VPRINTF("]");
        return;
    }

    VPRINTF(" %s=%s", strComment, decoder(symbol));
}


void
nf2_frontend_init(NIFFIOParser *pparser)
{
    mpq_init(t_measure_start);
    mpq_init(t_current);
    mpq_set_ui(t_current, 0, 1);

    nf2_form_Form_init(pparser);
    nf2_list_List_init(pparser);
    nf2_chunk_Chunk_init(pparser);
    nf2_tag_Tag_init(pparser);
}


void
nf2_frontend_end(NIFFIOParser *pparser)
{
    nf2_form_Form_end(pparser);
    nf2_list_List_end(pparser);
    nf2_chunk_Chunk_end(pparser);
    nf2_tag_Tag_end(pparser);
}
