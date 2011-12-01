
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
    symbol_p	scan;
    symbol_p	chord = NULL;
    symbol_p	next;

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
    symbol_p	scan;

    for (scan = f->unvoiced.front; scan != NULL; scan = scan->next) {
	if (scan->type != SYM_NOTE) {
	    continue;
	}

        int slur_start = scan->symbol.note.stem->slur_start;
        if (slur_start != -1) {
            symbol_p	slur_end = scan->next;

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


void
xly_chord(int do_chording)
{
    int		p;
    int		f;
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
