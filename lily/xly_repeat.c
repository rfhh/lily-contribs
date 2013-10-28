
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gmp.h>

#include "music/music.h"

#include "xly_repeat.h"


static int pending_repeats = 0;


static symbol_p
create_repeat(int logic, mpq_t t)
{
    symbol_p    start;

    start = symbol_create(t);
    start->type = SYM_REPEAT;
    start->symbol.repeat.graphic = 10;
    start->symbol.repeat.logic   = logic;

    return start;
}


static void
handle_repeat(symbol_p s, staff_p f)
{
    repeat_p    r = &s->symbol.repeat;

    if (r->graphic != 10) {
        return;
    }

    switch (r->logic) {
    case 0:
        break;
    case 1:
        pending_repeats++;
        break;
    case 2:
        if (pending_repeats == 0) {
            fprintf(stderr, "Warning: insert a repeat start at t = 0\n");
            /* Insert a start_repeat at the beginning */
            symbol_p    repeat;
            mpq_t       nul;

            mpq_init(nul);
            mpq_set_si(nul, 0, 1);

            repeat = create_repeat(1, nul);

            // q_insert(&f->unvoiced, start);
            repeat->next = f->unvoiced.front;
            repeat->prev = NULL;
            if (f->unvoiced.front != NULL) {
                f->unvoiced.front->prev = repeat;
            } else {
                f->unvoiced.tail = repeat;
            }
            f->unvoiced.front = repeat;
        } else {
            pending_repeats--;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    }
}



static void
do_staff_repeat(staff_p f)
{
    symbol_p    scan;
    symbol_p    last = NULL;

    /* The queue may be changed under our hands, but we don't care:
     * what we may insert is a start-repeat, and we do NOT want to see
     * that in the second place.
     */
    for (scan = f->unvoiced.front; scan != NULL; scan = scan->next) {

        switch (scan->type) {

        case SYM_STEM:
        case SYM_ARPEGGIO:
        case SYM_ARTICULATION:
        case SYM_BARLINE:
        case SYM_CHORD:
        case SYM_DYNAMIC:
        case SYM_GLISSANDO:
        case SYM_HAIRPIN:
        case SYM_MEASURE_NUMBERING:
        case SYM_MIDI:
        case SYM_OTTAVA:
        case SYM_ORNAMENT:
        case SYM_PARENTH:
        case SYM_PEDAL:
        case SYM_PORTAMENTO:
        case SYM_REHEARSAL_MARK:
            break;

        case SYM_REPEAT:
            handle_repeat(scan, f);
            break;

        case SYM_TEMPO:
        case SYM_TEXT:
        case SYM_TIE:
        case SYM_TREMOLO:
        case SYM_TUPLET:
        case SYM_NUMBER:
        case SYM_BAR_START:
        case SYM_CLEF:
        case SYM_KEY_SIGN:
        case SYM_TIME_SIGNATURE:
        case SYM_NOTE:
            break;
        }

        last = scan;
    }

    if (last != NULL) {
        while (pending_repeats > 0) {
            /* Insert an omitted end-repeat */
            symbol_p repeat = create_repeat(2, last->start);

            fprintf(stderr, "Warning: insert a missing end-repeat at t = ");
            mpq_out_str(stderr, 10, repeat->start);
            last->next = repeat;
            repeat->prev = last;
            repeat->next = NULL;
            f->unvoiced.tail = repeat;
            pending_repeats--;
        }
    }
}


void
xly_repeat(void)
{
    int         p;
    int         f;

    fprintf(stderr, "Find repeats...\n");
    for (p = 0; p < n_part; p++) {
        fprintf(stderr, "      ........ part %d, ", p);
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(stderr, " staff %d", f);
            do_staff_repeat(&part[p].staff[f]);
        }
        fprintf(stderr, "\n");
    }
}


void
xly_repeat_init(void)
{
}


void
xly_repeat_end(void)
{
}
