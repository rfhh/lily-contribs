
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gmp.h>

#include "music/music.h"

#include "xly_partial.h"

mpq_t	xly_t_partial;


static void
do_staff_partial(mpq_t *t, staff_p f)
{
    symbol_p	scan;
    mpq_t	t_sig;

    mpq_init(t_sig);

    mpq_set_ui(t_sig, 1, 1);
    for (scan = f->unvoiced.front; scan != NULL; scan = scan->next) {
	if (scan->type == SYM_TIME_SIGNATURE) {
	    if (scan->symbol.time_signature.top < 0) {
		mpq_set_ui(t_sig, 1, 1);
	    } else {
		mpq_set_ui(t_sig, scan->symbol.time_signature.top, scan->symbol.time_signature.bottom);
	    }
	} else if (scan->type == SYM_BAR_START) {
	    if (! mpq_zero(scan->start)) {
		if (mpq_cmp(scan->start, t_sig) != 0) {
		    mpq_set(*t, scan->start);
		}
		break;
	    }
	}
    }
    mpq_clear(t_sig);
}


void
xly_partial(void)
{
    int		p;
    int		f;
    mpq_t	t;

    fprintf(stderr, "Find partial start...\n");
    mpq_init(t);
    for (p = 0; p < n_part; p++) {
	for (f = 0; f < part[p].n_staff; f++) {
	    do_staff_partial(&t, &part[p].staff[f]);
	    if (p == 0 && f == 0) {
		mpq_set(xly_t_partial, t);
	    } else if (! mpq_equal(t, xly_t_partial)) {
		fprintf(stderr, "Uh oh -- partial part %d staff %d = ", p, f);
		mpq_out_str(stderr, 10, t);
		fprintf(stderr, " global = ");
		mpq_out_str(stderr, 10, xly_t_partial);
		fprintf(stderr, "\n");
	    }
	}
    }
    mpq_clear(t);

    VPRINTF("Partial = ");
    VPRINT_MPQ(xly_t_partial);
    VPRINTF("\n");
}


void
xly_partial_init(void)
{
    mpq_init(xly_t_partial);
}


void
xly_partial_end(void)
{
}
