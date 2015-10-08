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

#include <gmp.h>

#include "niff/tag/ID.h"

#include "music/music.h"

#include "xly_voice.h"

void report_symbol(const symbol_p scan, int verbos);

void debugMeAt(mpq_t t);
void debugMe(void);

void debugMe(void)
{
    fprintf(stderr, "Now hit %s\n", __func__);
}


void debugMeAt(mpq_t t)
{
    static int initialized;
    static mpq_t t_debug;

    if (! initialized) {
        mpq_init(t_debug);
        mpq_set_ui(t_debug, 0, 4);
        initialized = 1;
    }

    if (mpq_equal(t_debug, t)) {
        debugMe();
    }
}


static void
voice_increase(staff_p f)
{
    symbol_p    scan;
    voice_p     v;

    f->n_voice++;
    f->voice = realloc(f->voice, f->n_voice * sizeof(*f->voice));
    v = &f->voice[f->n_voice - 1];
    memset(v, 0, sizeof(*f->voice));
    mpq_init(v->t_finish);
    mpq_set_ui(v->t_finish, 0, 1);
    v->key_current = 0;
    v->key_previous_current = KEY_RESET;        /* previous key */
	v->stem_flags = FLAG_REST;					/* no previous stem direction */

    for (scan = f->replicated.front; scan != NULL; scan = scan->next) {
        report_symbol(scan, 1);
    }

    for (scan = f->replicated.front; scan != NULL; scan = scan->next) {
        symbol_p c = symbol_clone(scan);
        q_insert(&v->q, c);
    }
    for (scan = v->q.front; scan != NULL; scan = scan->next) {
        debugMeAt(scan->start);
    }
}


static int
chord_tie_match(note_p tail, note_p note)
{
    while (note != NULL) {
        if (tail->tie_start != -1 && tail->tie_start == note->tie_end) {
            return 1;
        }
        note = note->chord;
    }

    return 0;
}


static int
tie_match(note_p tail, note_p note)
{
    if (note->tie_end == NO_ID) {
        return 0;
    }

    if (tail == NULL) {
        return 1;
    }

    note_p      save_tail = tail;
    while (tail != NULL) {
        if (chord_tie_match(tail, note)) {
            return 1;
        }
        tail = tail->chord;
    }

    return save_tail->tie_start == note->tie_end;
}


static int
beam_match(note_p tail, note_p note)
{
    if (note->flags & FLAG_REST) {
        return 1;
    }

    if (note->stem->beam_left == 0) {
        return 0;
    }

    if (tail == NULL) {
        return note->stem->beam_left == 0;
    }

    if (tail->flags & FLAG_REST) {
        return 1;
    }

    return (tail->stem->beam_right == 0 && note->stem->beam_left == 0) ||
             note->stem->beam == tail->stem->beam;
}


static int
slur_match(voice_p v, note_p note)
{
    int         i;

    if (note->stem->slur_end == NO_ID) {
        return 0;
    }

    for (i = 0; i < v->n_slur; i++) {
        if (note->stem->slur_end == v->slur[i]) {
            return 1;
        }
    }

    return 0;
}


static void
append_note(staff_p s, int voice, symbol_p scan, symbol_p *c_scan, symbol_p *c_next)
{
    voice_p     v = &s->voice[voice];
    note_p      note = &scan->symbol.note;
    int         u;
    static mpq_t        t;
    static int  initialized = 0;

	if (scan == *c_scan) {
		*c_scan = scan->next;
		*c_next = scan->next->next;
	} else if (scan == *c_next) {
		*c_next = scan->next->next;
	}
	q_remove(&s->unvoiced, scan);

    if (! initialized) {
        mpq_init(t);
        initialized = 1;
    }

    q_append(&v->q, scan);
    if (! (note->flags & FLAG_REST)) {
		v->stem_flags = note->stem->flags;
	}
    v->tail = note;

    mpq_set(t, note->duration);
    if (0) {
        for (u = note->stem->tuplet; u != -1; u = global_tuplet[u].next) {
            mpq_mul(t, t, global_tuplet[u].ratio);
        }
    }
    for (u = note->tuplet; u != -1; u = global_tuplet[u].next) {
        mpq_mul(t, t, global_tuplet[u].ratio);
    }
    mpq_add(v->t_finish, scan->start, t);

    if (note->stem->slur_end != NO_ID) {
        int     i;

        for (i = 0; i < v->n_slur; i++) {
            if (note->stem->slur_end == v->slur[i]) {
                v->n_slur--;
                v->slur[i] = v->slur[v->n_slur];
                break;
            }
        }
        s->slur_pending--;
    }
    if (note->stem->slur_start != NO_ID) {
        v->n_slur++;
        if (v->n_slur == MAX_CONCURRENT_SLURS) {
            fprintf(stderr, "Can only support %d concurrent slurs -- see for yourself what rests now\n", MAX_CONCURRENT_SLURS);
        }
        v->slur[v->n_slur - 1] = note->stem->slur_start;
        s->slur_pending++;
    }
}


static int
is_constrained(const note_t *note)
{
    return note->tie_end != -1 ||
                note->stem->slur_end != -1 ||
                note->stem->beam_left != 0;
}


static void
report_note(const symbol_p scan)
{
    const note_p note = &scan->symbol.note;
    int         u;

    VPRINTF("Test %p for contiguous append: t = ", scan);
    VPRINT_MPQ(scan->start);
    if (note->flags & FLAG_REST) {
        VPRINTF(" rest");
    } else if (note->chord != NULL) {
        note_p  chord;
        VPRINTF(" step <");
        VPRINTF("%d ", note->value);
        for (chord = note->chord; chord != NULL; chord = chord->chord) {
            VPRINTF("%d ", chord->value);
        }
        VPRINTF(">");
    } else {
        VPRINTF(" step %d", note->value);
    }
    VPRINTF(" duration ");
    VPRINT_MPQ(note->duration);
    if (0) {
        for (u = note->stem->tuplet; u != -1; u = global_tuplet[u].next) {
            VPRINTF("*");
            VPRINT_MPQ(global_tuplet[u].ratio);
        }
    }
    for (u = note->tuplet; u != -1; u = global_tuplet[u].next) {
        VPRINTF("*");
        VPRINT_MPQ(global_tuplet[u].ratio);
    }
    VPRINTF(" beam %p %d %s [%d,%d]", note->stem, note->stem->beam,
             (note->stem->flags & FLAG_STEM_UP) ? "up" : "down",
             note->stem->beam_left,
             note->stem->beam_right);
    VPRINTF(" tie> %d tie< %d", note->tie_start, note->tie_end);
    VPRINTF(" slur> %d slur< %d",
             note->stem->slur_start, note->stem->slur_end);
}


static void
report_voice_tail(const staff_p f, int i)
{
    voice_p v = &f->voice[i];
    note_p tail = v->tail;

    VPRINTF(" voice[%d].t_finish = ", i);
    VPRINT_MPQ(v->t_finish);
    if (tail == NULL) {
        VPRINTF(" beam <nope>\n");
    } else {
        VPRINTF(" beam %p %d [%d,%d]\n", tail->stem,
                tail->stem->beam, tail->stem->beam_left,
                tail->stem->beam_left);
    }
}


void
report_symbol(const symbol_p scan, int verbos)
{
    if (verbos && scan->type == SYM_NOTE) {
        report_note(scan);
    } else {
        VPRINTF("At t = ");
        VPRINT_MPQ(scan->start);
        VPRINTF(" symbol %p %s: ", scan, SYMBOL_TYPE_string(scan->type));
        VPRINTF("\n");
    }
}


#ifdef UNUSED
static int
n_simultaneous(const staff_p f,
               mpq_t now,
               const symbol_t *front,
               int require_constrained)
{
    const symbol_t *scan;
    int         n = 0;

    for (scan = front;
             scan != NULL && mpq_equal(now, scan->start);
             scan = scan->next) {
        if (scan->type != SYM_NOTE) {
            continue;
        }

        if (require_constrained == is_constrained(&scan->symbol.note)) {
            n++;
        }
    }

    return n;
}
#endif


static int
notes_simultaneous_constrained(staff_p f,
                               mpq_t now,
                               symbol_p *c_scan,
                               symbol_p *c_next)
{
    symbol_p    scan;
    symbol_p    next;
    int         r = 1;

    next = *c_next;
    for (scan = *c_scan;
             scan != NULL && mpq_equal(now, scan->start);
             scan = next) {
        int     i;

        next = scan->next;
        if (scan->type != SYM_NOTE) {
            continue;
        }

        note_p  note = &scan->symbol.note;
        if (! is_constrained(note)) {
            continue;
        }

        report_note(scan);

        for (i = 0; i < f->n_voice; i++) {
            voice_p v = &f->voice[i];
            note_p tail = v->tail;

            report_voice_tail(f, i);

            if (mpq_cmp(scan->start, f->voice[i].t_finish) >= 0) {
                if (tie_match(tail, note)) {
                    VPRINTF("Append note contiguously/tie to voice %d\n", i);
                    break;
                }
                if (beam_match(tail, note)) {
                    VPRINTF("Append note contiguously/beam to voice %d\n", i);
                    break;
                }
                if (slur_match(v, note)) {
                    VPRINTF("Append note contiguously/slur to voice %d\n", i);
                    break;
                }
            }
        }

        if (i != f->n_voice && ! mpq_equal(scan->start, f->voice[i].t_finish)) {
            fprintf(stderr, "Weird, a constrained note with a time gap\n");
        }
        if (i == f->n_voice) {          /* Could not append */
            fprintf(stderr,
                    "Ooooppss, constrained note but cannot connect to voice\n");

            for (i = 0; i < f->n_voice; i++) {
                if (mpq_cmp(scan->start, f->voice[i].t_finish) > 0) {
                    VPRINTF("Append note non-contiguously, t = ");
                    VPRINT_MPQ(scan->start);
                    VPRINTF(" voice[%d].t_finish = ", i);
                    VPRINT_MPQ(f->voice[i].t_finish);
                    VPRINTF(" to voice %d\n", i);
                    break;
                }
            }
        }

        if (i == f->n_voice) {          /* Could not append */
            fprintf(stderr, "Oooppss, meet the combinatorial case...\n");

            VPRINTF("Append note to new voice[%d]\n", f->n_voice);
#if VERBOSE
            {
                int v;

                for (v = 0; v < f->n_voice; v++) {
                    VPRINTF("voice[%d] finish ", v);
                    VPRINT_MPQ(f->voice[v].t_finish);
                    VPRINTF("\n");
                }
            }
#endif
            voice_increase(f);
        }

        if (i != f->n_voice) {
            append_note(f, i, scan, c_scan, c_next);
        }
    }

    // update it now, at the end...
    *c_next = (*c_scan)->next;

    return r;
}


static int
stem_match(staff_p f,
		   int start, int step,
		   symbol_p scan, symbol_p next,
		   symbol_p *c_scan, symbol_p *c_next,
		   const char *label)
{
	for (int i = start; i < f->n_voice; i += step) {
		voice_p v = &f->voice[i];

		if (mpq_equal(scan->start, v->t_finish)) {
			VPRINTF("Voice %d matches\n", i);
			if (! (v->stem_flags & FLAG_REST) &&
				(scan->symbol.note.stem->flags & FLAG_STEM_UP) ==
					(v->stem_flags & FLAG_STEM_UP)) {
				report_note(scan);
				VPRINTF("Append note with %s contiguously to voice %d\n", label, i);
				append_note(f, i, scan, c_scan, c_next);

				return 1;
			}
		}
	}

	return 0;
}


static int
cmp_note_value(const void *vn1, const void *vn2) {
    const note_t *n1 = vn1;
    const note_t *n2 = vn2;

	return n1->value - n2->value;
}


static int
notes_from_chord_directed(staff_p f,
						  mpq_t now,
						  symbol_p *c_scan, symbol_p *c_next,
						  int n,
						  note_flags_t direction)
{
	const note_t **sorted = malloc(n * sizeof *sorted);

	int i = 0;
	for (symbol_p scan = *c_scan;
			 scan != NULL && mpq_equal(now, scan->start);
			 scan = scan->next) {
		if (scan->type != SYM_NOTE) {
			continue;
		}
		sorted[i] = &scan->symbol.note;
		i++;
	}

	qsort(sorted, n, sizeof *sorted, cmp_note_value);

	for (symbol_p scan = *c_scan;
			 scan != NULL && mpq_equal(now, scan->start);
			 scan = scan->next) {
		if (scan->type != SYM_NOTE) {
			continue;
		}

		note_p note = &scan->symbol.note;
		int i;
		for (i = 0; i < n; i++) {
			if (note == sorted[i]) {
				break;
			}
		}
		if (i == n) {
			fprintf(stderr, "OOOOPPPPSSSSS note not found in sorted\n");
			continue;
		}

        while (i >= f->n_voice) {          /* Could not append */
            VPRINTF("Create new voice to append ex-chorded note\n");
            voice_increase(f);
		}
		VPRINTF("Append 'chorded' note contiguously to voice %d\n", i);
		append_note(f, i, scan, c_scan, c_next);
		if ((i % 2) == 0) {
			f->voice[i].stem_flags |= FLAG_STEM_UP;
		} else {
			f->voice[i].stem_flags &= ~FLAG_STEM_UP;
		}
	}

	free(sorted);

	return n;
}


/**
 * The case where a chorded notation has been pulled apart deserves special
 * attention: the stems are all in the same direction. If this is the case,
 * simply matching the first of the 'chorded' notes to a previous stem direction
 * is too simple: need to consider all possible matches and select the 'best' one.
 */
static int
notes_from_chord(staff_p f, mpq_t now, symbol_p *c_scan, symbol_p *c_next)
{
	int n = 0;
	int up = 0;
	for (symbol_t *scan = *c_scan;
			 scan != NULL && mpq_equal(now, scan->start);
			 scan = scan->next) {
		if (scan->type != SYM_NOTE) {
			continue;
		}
		if ((scan->symbol.note.stem->flags & FLAG_STEM_UP) == FLAG_STEM_UP) {
			up++;
		}
		n++;
	}

	if (n > 1 && up == n) {
		// All upstems
		notes_from_chord_directed(f, now, c_scan, c_next, up, FLAG_STEM_UP);
	}

	if (n > 1 && up == 0) {
		// All downstems
		notes_from_chord_directed(f, now, c_scan, c_next, n - up, 0);
	}
	
	return n;
}


static int
notes_simultaneous_unconstrained(staff_p f,
                                 mpq_t now,
                                 symbol_p *c_scan,
                                 symbol_p *c_next)
{
    symbol_p    scan;
    symbol_p    next;
    int         r = 1;
	mpq_t	t_whole;

	mpq_init(t_whole);
	mpq_set_ui(t_whole, 1, 1);

	notes_from_chord(f, now, c_scan, c_next);

	/* We first handle all stem-matching notes */
	next = *c_next;
	for (scan = *c_scan;
			 scan != NULL && mpq_equal(now, scan->start);
			 scan = next) {
		next = scan->next;

		if (scan->type != SYM_NOTE) {
			continue;
		}

		if (is_constrained(&scan->symbol.note)) {
			fprintf(stderr, "OOOOPPPPSSSSS constrained note not assigned\n");
		}

		stem_match(f, 0, 1, scan, next, c_scan, c_next, "matching stem");
	}

	/* We then handle all stem-up notes */
	next = *c_next;
	for (scan = *c_scan;
			 scan != NULL && mpq_equal(now, scan->start);
			 scan = next) {
		next = scan->next;
		if (scan->type != SYM_NOTE) {
			continue;
		}

		if (! (scan->symbol.note.stem->flags & FLAG_STEM_UP)) {
			continue;
		}

		stem_match(f, 0, 2, scan, next, c_scan, c_next, "stem UP");
	}

	/* We then handle all explicit stem-down notes to assign to voice 1, 3, 5, ... */
	next = *c_next;
	for (scan = *c_scan;
			 scan != NULL && mpq_equal(now, scan->start);
			 scan = next) {
		next = scan->next;
		if (scan->type != SYM_NOTE) {
			continue;
		}

		if (scan->symbol.note.stem->flags & FLAG_STEM_UP) {
			continue;
		}
		if (scan->symbol.note.flags & FLAG_REST) {
			continue;
		}
		if (mpq_cmp(scan->symbol.note.duration, t_whole) >= 0) {
			continue;
		}

		stem_match(f, 1, 2, scan, next, c_scan, c_next, "stem DOWN");
	}

    next = *c_next;
    for (scan = *c_scan;
             scan != NULL && mpq_equal(now, scan->start);
             scan = next) {
        int     i;

        next = scan->next;
        if (scan->type != SYM_NOTE) {
            continue;
        }

        report_note(scan);

		for (i = 0; i < f->n_voice; i++) {
			voice_p v = &f->voice[i];

			report_voice_tail(f, i);

			if (mpq_equal(scan->start, v->t_finish)) {
				VPRINTF("Append note contiguously to voice %d\n", i);
				break;
			}
		}

        if (i >= f->n_voice) {          /* Could not append */
            for (i = 0; i < f->n_voice; i++) {
                if (mpq_cmp(scan->start, f->voice[i].t_finish) > 0) {
                    VPRINTF("Append note non-contiguously, t = ");
                    VPRINT_MPQ(scan->start);
                    VPRINTF(" voice[%d].t_finish = ", i);
                    VPRINT_MPQ(f->voice[i].t_finish);
                    VPRINTF(" to voice %d\n", i);
                    break;
                }
            }
        }

        if (i >= f->n_voice) {          /* Could not append */
            VPRINTF("Append note to new voice[%d]\n", f->n_voice);
#if VERBOSE
            {
                int v;

                for (v = 0; v < f->n_voice; v++) {
                    VPRINTF("voice[%d] finish ", v);
                    VPRINT_MPQ(f->voice[v].t_finish);
                    VPRINTF("\n");
                }
            }
#endif
            voice_increase(f);
        }

        if (i < f->n_voice) {
            append_note(f, i, scan, c_scan, c_next);
        }
    }

    // update it now, at the end...
    *c_next = *c_scan;

    return r;
}


static int
do_staff_voicing(staff_p f, symbol_p scan)
{
    symbol_p    next;
    int         i;
    mpq_t       now;
    int         r = 1;

    mpq_init(now);

    /* yes, test, because we might be recursing */
    if (scan != NULL && f->n_voice == 0) {
        voice_increase(f);
    }

    for (; scan != NULL; scan = next) {

        mpq_set(now, scan->start);

        next = scan->next;
        report_symbol(scan, 0);
        VPRINTF("%d scan %p next %p next->next %p\n", __LINE__, scan, next, (next != NULL) ? next->next : NULL);

        if (scan->type != SYM_NOTE) {
            // NOTEs have a special treatment because multiple simultaneous
            // notes are handled; no point in already unlinking scan
			q_remove(&f->unvoiced, scan);
        }

        switch (scan->type) {

        default:
        case SYM_STEM:
            abort();

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
        case SYM_TEMPO:
        case SYM_TEXT:
        case SYM_TIE:
        case SYM_TREMOLO:
        case SYM_TUPLET:
        case SYM_NUMBER:
            q_append_before_simultaneous(&f->voice[0].q, scan);
            break;

        case SYM_REPEAT:
            q_append(&f->voice[0].q, scan);
            break;

        case SYM_BAR_START:
        case SYM_CLEF:
        case SYM_KEY_SIGN:
        case SYM_TIME_SIGNATURE:
            VPRINTF("Symbol %s %p at t = ",
                     SYMBOL_TYPE_string(scan->type), scan);
            VPRINT_MPQ(scan->start);
            VPRINTF("; n_voice = %d\n", f->n_voice);
            for (i = 0; i < f->n_voice; i++) {
                symbol_p c = symbol_clone(scan);
                q_append_before_simultaneous(&f->voice[i].q, c);
            }
            q_append_before_simultaneous(&f->replicated, scan);
            break;

        case SYM_NOTE:
            if (! notes_simultaneous_constrained(f,
                                                 now,
                                                 &scan,
                                                 &next)) {
                r = 0;
                goto exit;
            }
            // VPRINTF("%d scan %p next %p\n", __LINE__, scan, next);
            if (! notes_simultaneous_unconstrained(f,
                                                   now,
                                                   &scan,
                                                   &next)) {
                r = 0;
                goto exit;
            }
            // VPRINTF("%d scan %p next %p\n", __LINE__, scan, next);

        }
    }

	for (scan = f->unvoiced.front; scan != NULL; scan = next) {
		next = scan->next;
		assert(scan->type != SYM_NOTE);
		VPRINTF("Skip this non-note symbol (t_start = ");
		VPRINT_MPQ(scan->start);
		VPRINTF(" on voice 0\n");
		q_remove(&f->unvoiced, scan);
	}

exit:
    f->next_after_backtrack = NULL;
    mpq_clear(now);

    return r;
}


void
xly_voice(void)
{
    int         p;
    int         f;
    int         v;
    int         n_voices;

    fprintf(stderr, "Voice analysis...\n");
    for (p = 0; p < n_part; p++) {
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(stderr, "      ........ part %d, staff %d\n", p, f);
            do_staff_voicing(&part[p].staff[f], part[p].staff[f].unvoiced.front);
        }
    }

    n_voices = 0;
    for (p = 0; p < n_part; p++) {
        for (f = 0; f < part[p].n_staff; f++) {
            for (v = 0; v < part[p].staff[f].n_voice; v++) {
                part[p].staff[f].voice[v].id = n_voices++;
            }
        }
    }
}


void
xly_voice_init(void)
{
}


void
xly_voice_end(void)
{
}
