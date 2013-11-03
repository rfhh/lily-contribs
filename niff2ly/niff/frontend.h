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

#ifndef __NIFF2LY_NIFF_FRONTED_H__
#define __NIFF2LY_NIFF_FRONTED_H__

#include <niffio.h>

#include <gmp.h>

#include "music/music.h"

extern part_p           part_current;
extern symbol_p         symbol_current;
extern symbol_p         note_current;

extern symbol_q_p       voice_current;
extern symbol_q_p       voice_previous;
extern symbol_q_t       voice_unparted;

extern symbol_q_p      *voice_index;

extern time_signature_p time_sig_current;

extern mpq_t            t_measure_start;        /* Most recent measure start */
extern mpq_t            t_current;              /* Most recent time slice */
extern stem_p           stem_current;           /* For chording and voicing */


void printSHORT(SHORT h, const char *strComment);
void printSTROFFSET(STROFFSET offset, const char *strComment);
void printRATIONAL(RATIONAL r, const char *strComment);
void printBYTE(BYTE b, const char *strComment);
void printSIGNEDBYTE(SIGNEDBYTE sb, const char *strComment);
void printSymbol(const char *(decoder(BYTE)), BYTE symbol, const char *strComment);

void nf2_frontend_init(NIFFIOParser *pparser);
void nf2_frontend_end(NIFFIOParser *pparser);

#endif
