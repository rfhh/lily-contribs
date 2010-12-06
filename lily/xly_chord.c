
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gmp.h>

#include "music/music.h"

#include "xly_chord.h"


static void
do_staff_chording(staff_p f)
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
	    VPRINTF(("\nInspect note start "));
	    VPRINT_MPQ(scan->start);
	    VPRINTF((" step %d length ", scan->symbol.note.value));
	    VPRINT_MPQ(scan->symbol.note.duration);
	}
	for (chord = scan->prev;
		chord != NULL && mpq_equal(scan->start, chord->start);
		chord = chord->prev) {
	    if (chord->type == scan->type) {
		VPRINTF((" against chord start "));
		VPRINT_MPQ(chord->start);
		VPRINTF((" step %d length ", chord->symbol.note.value));
		VPRINT_MPQ(chord->symbol.note.duration);
	    }
	    if (chord->type == scan->type &&
		chord->symbol.note.stem == scan->symbol.note.stem &&
		(chord->symbol.note.flags & FLAG_STEM_UP) ==
		     (scan->symbol.note.flags & FLAG_STEM_UP) &&
		mpq_equal(chord->symbol.note.duration, scan->symbol.note.duration)
		) {
		/* OK, it is a chord continuation, as far as we can tell */
		VPRINTF(("\nFound a chord note"));
		q_remove(&f->unvoiced, scan);
		scan->symbol.note.chord = chord->symbol.note.chord;
		chord->symbol.note.chord = &scan->symbol.note;
		break;
	    }
	}
    }
}


void
xly_chord(void)
{
    int		p;
    int		f;

    fprintf(stderr, "Chord analysis...\n");
    for (p = 0; p < n_part; p++) {
	for (f = 0; f < part[p].n_staff; f++) {
	    fprintf(stderr, "      ........ part %d, staff %d\n", p, f);
	    do_staff_chording(&part[p].staff[f]);
	}
    }
}


void
xly_chord_init(void)
{
}


void
xly_chord_end(void)
{
}
