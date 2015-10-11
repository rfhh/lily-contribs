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

#include "music/music.h"

#include "xly_chord.h"


static int     *ties_duplicate;
static int     *slurs_duplicate;


static int
tie_dupl_create(int ID)
{
    int         new_ID = n_ties;

    ties_increase(new_ID);
    ties_duplicate[ID] = new_ID;

    return new_ID;
}


static int
tie_dupl_lookup(int ID)
{
    return ties_duplicate[ID];
}


static int
slur_dupl_create(int ID)
{
    int         new_ID = n_slurs;

    slurs_increase(new_ID);
    slurs_duplicate[ID] = new_ID;

    return new_ID;
}


static int
slur_dupl_lookup(int ID)
{
    return slurs_duplicate[ID];
}


static void
do_staff_chording(staff_p f, int do_chording)
{
    symbol_p    scan;
    symbol_p    chord = NULL;
    symbol_p    next;

    for (scan = f->unvoiced.front; scan != NULL; scan = next) {
        next = scan->next;
        if (scan->type != SYM_NOTE) {
            continue;
        }

        if (scan->prev != NULL && mpq_equal(scan->start, scan->prev->start)) {
            VPRINTF("\nInspect note start ");
            VPRINT_MPQ(scan->start);
            VPRINTF(" step %d length ", scan->symbol.note.value);
            VPRINT_MPQ(scan->symbol.note.duration);
        }
        for (chord = scan->prev;
                chord != NULL && mpq_equal(scan->start, chord->start);
                chord = chord->prev) {
            if (chord->type == scan->type) {
                VPRINTF(" against chord start ");
                VPRINT_MPQ(chord->start);
                VPRINTF(" step %d length ", chord->symbol.note.value);
                VPRINT_MPQ(chord->symbol.note.duration);
            }
            if (! (scan->symbol.note.flags & FLAG_REST) &&
                    chord->type == scan->type &&
                    chord->symbol.note.stem == scan->symbol.note.stem &&
                    mpq_equal(chord->symbol.note.duration,
                              scan->symbol.note.duration)
                ) {
                /* OK, it is a chord continuation, as far as we can tell */
                VPRINTF("\nFound a chord note");
                if (do_chording) {
                    q_remove(&f->unvoiced, scan);
                    scan->symbol.note.chord = chord->symbol.note.chord;
                    chord->symbol.note.chord = &scan->symbol.note;
                } else {
                    // Generate a new stem, duplicate ties and slurs
                    note_p note = &chord->symbol.note;
                    stem_p stem = note->stem;
                    symbol_p stem_symbol = SYMBOL_OF_SYM(stem);

                    note->stem = &symbol_clone(stem_symbol)->symbol.stem;

                    // We need to check both scan and chord if there
                    // are slurs or ties to duplicate
					if (note->flags & FLAG_REST) {
						stem->slur_start = -1;
						stem->slur_end = -1;
					} else {
						if (stem->slur_start != -1) {
							int dupl_slur = slur_dupl_create(stem->slur_start);
							VPRINTF("Duplicate its slur %d -> %d\n",
									stem->slur_start, dupl_slur);
							stem->slur_start = dupl_slur;
						}
						if (stem->slur_end != -1) {
							int dupl_slur = slur_dupl_lookup(stem->slur_end);
							VPRINTF("Finish its duplicate slur %d -> %d\n",
									stem->slur_start, dupl_slur);
							stem->slur_end = dupl_slur;
						}
					}

                    if (0) {
                        if (note->tie_start != -1) {
                            tie_p tie = &ties[note->tie_start];
                            if (tie_dupl_lookup(note->tie_start) == -1) {
                                note->tie_start = tie_dupl_create(note->tie_start);
                                tie = &ties[note->tie_start];
                                tie->occurred = 0;
                                tie->occur = ties[chord->symbol.note.tie_start].occur;
                                tie->notes = calloc(tie->occur, sizeof *tie->notes);
                            }
                            tie->occurred++;
                            tie->notes[tie->occurred - 1] = note;
                        }
                        if (note->tie_end != -1) {
                            note->tie_end = tie_dupl_lookup(note->tie_end);
                            tie_p tie = &ties[note->tie_start];
                            tie->notes[tie->occurred - 1] = note;
                        }
                    }
                }
                break;
            }
        }
    }
}


static void
do_staff_slur_pending(staff_p f)
{
    symbol_p    scan;

    for (scan = f->unvoiced.front; scan != NULL; scan = scan->next) {
        if (scan->type != SYM_NOTE) {
            continue;
        }

        int slur_start = scan->symbol.note.stem->slur_start;
        if (slur_start != -1) {
            symbol_p    slur_end = scan->next;

            for (; slur_end != NULL; slur_end = slur_end->next) {
                if (slur_end->type != SYM_NOTE) {
                    continue;
                }

                if (slur_end->symbol.note.stem->slur_end == slur_start) {
                    break;
                }
            }

            if (slur_end == NULL) {
                fprintf(stderr, "Oooppsss, a pending slur ID %d; remove\n",
                        slur_start);
                scan->symbol.note.stem->slur_start = -1;
            }
        }
    }
}


static void
do_staff_multibar_aggregate(staff_p f)
{
    note_p      multibar_open = NULL;   // defy gcc warnings
    mpq_t       time_sig;
    int         multibar = 0;

    mpq_init(time_sig);

    for (symbol_p scan = f->unvoiced.front; scan != NULL; scan = scan->next) {
        int must_close = 1;
        note_p note;

        switch (scan->type) {
        case SYM_TIME_SIGNATURE:
            mpq_set(time_sig, scan->symbol.time_signature.duration);
            break;

        case SYM_TUPLET:
        case SYM_REPEAT:
            break;

        case SYM_NOTE:
            note = &scan->symbol.note;

            if (! (note->flags & FLAG_REST)) {
                break;
            }
            if (! mpq_equal(time_sig, note->duration)) {
                break;
            }

            // Ah, there it is: a multibar rest
            must_close = 0;
            if (multibar == 0) {
                multibar_open = note;
            } else {
                note->multibar = 0;     // suppress
            }
            multibar++;
            break;

        case SYM_CHORD:
            fprintf(stderr, "Ooopppsssss... a CHORD here?\n");
            break;

        default:
            must_close = 0;
            break;
        }

        if (must_close && multibar > 0) {
            multibar_open->multibar = multibar;
            multibar = 0;
        }
    }
}


void
xly_chord(int do_chording)
{
    int         p;
    int         f;
    int         i;

    ties_duplicate = malloc(n_ties * sizeof *ties_duplicate);
    if (ties_duplicate == NULL) {
        fprintf(stderr, "%s:%d malloc(%zd) fails\n", __FILE__, __LINE__,
                n_ties * sizeof *ties_duplicate);
        return;
    }
    for (i = 0; i < n_ties; i++) {
        ties_duplicate[i] = -1;
    }
    slurs_duplicate = malloc(n_slurs * sizeof *slurs_duplicate);
    if (slurs_duplicate == NULL) {
        fprintf(stderr, "%s:%d malloc(%zd) fails\n", __FILE__, __LINE__,
                n_slurs * sizeof *slurs_duplicate);
        return;
    }
    for (i = 0; i < n_slurs; i++) {
        slurs_duplicate[i] = -1;
    }

    fprintf(stderr, "Chord analysis...\n");
    for (p = 0; p < n_part; p++) {
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(stderr, "      ........ part %d, staff %d\n", p, f);
            do_staff_chording(&part[p].staff[f], do_chording);
        }
    }

    fprintf(stderr, "Pending slur analysis...\n");
    for (p = 0; p < n_part; p++) {
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(stderr, "      ........ part %d, staff %d\n", p, f);
            do_staff_slur_pending(&part[p].staff[f]);
        }
    }

    fprintf(stderr, "Multibar rest aggregation...\n");
    for (p = 0; p < n_part; p++) {
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(stderr, "      ........ part %d, staff %d\n", p, f);
            do_staff_multibar_aggregate(&part[p].staff[f]);
        }
    }

    free(ties_duplicate);
    free(slurs_duplicate);
}


void
xly_chord_init(void)
{
}


void
xly_chord_end(void)
{
}
