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
