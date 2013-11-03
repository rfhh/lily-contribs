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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <niffio.h>

#include <gmp.h>

#include "music.h"


// Utility functions from the debugger

int
mpq_printf(FILE *f, const mpq_t t)
{
    mpq_out_str(f, 10, t);
    fprintf(f, "\n");

    return 0;
}


int
mpq_dump(const mpq_t t)
{
    return mpq_printf(stderr, t);
}


const char *
SYMBOL_TYPE_string(symbol_type_t s)
{
    switch (s) {
    case SYM_ARPEGGIO: return "SYM_ARPEGGIO";
    case SYM_ARTICULATION: return "SYM_ARTICULATION";
    case SYM_BARLINE: return "SYM_BARLINE";
    case SYM_BAR_START: return "SYM_BAR_START";
    case SYM_CHORD: return "SYM_CHORD";
    case SYM_CLEF: return "SYM_CLEF";
    case SYM_DYNAMIC: return "SYM_DYNAMIC";
    case SYM_GLISSANDO: return "SYM_GLISSANDO";
    case SYM_HAIRPIN: return "SYM_HAIRPIN";
    case SYM_KEY_SIGN: return "SYM_KEY_SIGN";
    case SYM_MEASURE_NUMBERING: return "SYM_MEASURE_NUMBERING";
    case SYM_MIDI: return "SYM_MIDI";
    case SYM_NOTE: return "SYM_NOTE";
    case SYM_OTTAVA: return "SYM_OTTAVA";
    case SYM_ORNAMENT: return "SYM_ORNAMENT";
    case SYM_PARENTH: return "SYM_PARENTH";
    case SYM_PEDAL: return "SYM_PEDAL";
    case SYM_PORTAMENTO: return "SYM_PORTAMENTO";
    case SYM_REHEARSAL_MARK: return "SYM_REHEARSAL_MARK";
    case SYM_REPEAT: return "SYM_REPEAT";
    case SYM_STEM: return "SYM_STEM";
    case SYM_TEMPO: return "SYM_TEMPO";
    case SYM_TEXT: return "SYM_TEXT";
    case SYM_TIE: return "SYM_TIE";
    case SYM_TIME_SIGNATURE: return "SYM_TIME_SIGNATURE";
    case SYM_TREMOLO: return "SYM_TREMOLO";
    case SYM_TUPLET: return "SYM_TUPLET";
    case SYM_NUMBER: return "SYM_NUMBER";
    }

    return "Unknown symbol";
}


int symbol_dump(const symbol_t *s);
int
symbol_dump(const symbol_t *s)
{
    fprintf(stderr, "%s@", SYMBOL_TYPE_string(s->type));
    return mpq_dump(s->start);
}


int q_dump(const symbol_q_t *q, int off, int n);
int
q_dump(const symbol_q_t *q, int off, int n)
{
     const symbol_t *scan = q->front;
     int        i;

     for (i = 0; i < off && scan != NULL; i++, scan = scan->next) {
         /* traverse */
     }
     for (; i < off + n && scan != NULL; i++, scan = scan->next) {
         fprintf(stderr, "[%d] ", i);
         symbol_dump(scan);
     }

     return 0;
}


void
rat2mpq(mpq_t q, RATIONAL *r)
{
    mpq_set_si(q, r->numerator, r->denominator);
    mpq_canonicalize(q);
}


void
mpq2rat(mpq_t t, int *nu, int *de)
{
    static mpz_t        zde;
    static mpz_t        znu;
    static int          initialized = 0;

    if (! initialized) {
        mpz_init(zde);
        mpz_init(znu);
        initialized = 1;
    }

    mpq_get_num(znu, t);
    mpq_get_den(zde, t);
    *nu = mpz_get_ui(znu);
    *de = mpz_get_ui(zde);
}


int
mpq_zero(mpq_t t)
{
    static mpq_t        z;
    static int  initialized = 0;

    if (! initialized) {
        mpq_init(z);
        mpq_set_ui(z, 0, 1);
    }

    return mpq_equal(t, z);
}


#if VERBOSE

int     xly_verbose = 0;

#include <stdarg.h>

int verbose_printf(char *fmt, ...)
{
    va_list     ap;
    int         r;

    va_start(ap, fmt);
    r = vfprintf(stderr, fmt, ap);
    va_end(ap);

    return r;
}

#endif




/*
 * Yes, a 3-dim array:
 *      part[n_part]
 *      part[p][n_staff]
 *      part[p][f][n_voice]
 */
part_p          part;

int             n_part;

tuplet_p        tuplet_current;

tuplet_p        global_tuplet;
int             n_tuplet;


void
q_insert(symbol_q_p q, symbol_p n)
{
    symbol_p scan;

    if (q->front == NULL) {
        q->front = n;
        q->tail  = n;
        n->next = NULL;
        n->prev = NULL;
        return;
    }

    scan = q->tail;
    while (scan != NULL && mpq_cmp(n->start, scan->start) < 0) {
        scan = scan->prev;
    }
    if (n->type == SYM_NOTE) {
        while (scan != NULL && scan->type == SYM_NOTE &&
                mpq_equal(n->start, scan->start)) {
            if (n->symbol.note.stem->beam <= scan->symbol.note.stem->beam) {
                break;
            }
            scan = scan->prev;
        }
    }

    if (scan == NULL) {
        n->next = q->front;
        n->prev = NULL;
        q->front->prev = n;
        q->front = n;
        return;
    }

    n->prev = scan;
    n->next = scan->next;
    scan->next = n;
    if (n->next == NULL) {
        q->tail = n;
    } else {
        n->next->prev = n;
    }
}


void
q_append(symbol_q_p q, symbol_p n)
{
    if (q->front == NULL) {
        q->front = n;
        q->tail  = n;
        n->next = NULL;
        n->prev = NULL;
    } else {
        n->next = NULL;
        n->prev = q->tail;
        q->tail->next = n;
        q->tail = n;
    }
}


void
q_append_before_simultaneous(symbol_q_p q, symbol_p n)
{
    if (q->front == NULL || mpq_cmp(n->start, q->tail->start) > 0) {
        q_append(q, n);
    } else {
        symbol_p scan = q->tail;
        symbol_p next = NULL;
        while (scan != NULL && mpq_cmp(n->start, scan->start) == 0) {
            next = scan;
            scan = scan->prev;
        }
        n->next = next;
        n->prev = scan;
        if (next == NULL) {
            // at the tail, after all
            q->tail = n;
            scan->next = n;
        } else {
            next->prev = n;
            if (scan == NULL) {
                // at the front
                q->front = n;
            } else {
                scan->next = n;
            }
        }
    }
}


void
q_remove(symbol_q_p q, symbol_p n)
{
    if (n->prev == NULL) {
        assert(n == q->front);
        q->front = n->next;
    } else {
        n->prev->next = n->next;
    }
    if (n->next == NULL) {
        assert(n == q->tail);
        q->tail = n->prev;
    } else {
        n->next->prev = n->prev;
    }
}


void
stack_push(symbol_p *s, symbol_p n)
{
    n->next = *s;
    *s = n;
}


symbol_p
symbol_create(mpq_t t_current)
{
    symbol_p    s = calloc(1, sizeof(*s));

    mpq_init(s->start);
    mpq_set(s->start, t_current);

    return s;
}


symbol_p
symbol_clone(symbol_p s)
{
    symbol_p    c = malloc(sizeof(*c));

    *c = *s;

    mpq_init(c->start);
    mpq_set(c->start, s->start);

    return c;
}


void
symbol_clear(symbol_p s)
{
    mpq_clear(s->start);
    free(s);
}


int     n_ties;
tie_p   ties;


void
ties_increase(int ID)
{
    int         i;
    int         old_n;

    if (ID < n_ties) {
        return;
    }

    old_n = n_ties;
    n_ties = ID + 1;
    ties = realloc(ties, n_ties * sizeof(*ties));
    for (i = old_n; i < n_ties; i++) {
        memset(&ties[i], 0, sizeof(*ties));
    }
}


int     n_slurs;
slur_p  slur;


void
slurs_increase(int ID)
{
    int old_n_slurs = n_slurs;
    int i;

    n_slurs = ID + 1;
    slur = realloc(slur, n_slurs * sizeof(*slur));
    for (i = old_n_slurs; i < n_slurs; i++) {
        memset(&slur[i], 0, sizeof(*slur));
    }
}


void
nf2_music_init(void)
{
}


void
nf2_music_end(void)
{
}
