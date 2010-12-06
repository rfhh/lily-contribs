#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include <gmp.h>

#include "music.h"


void
rat2mpq(mpq_t q, RATIONAL *r)
{
    mpq_set_si(q, r->numerator, r->denominator);
    mpq_canonicalize(q);
}


void
mpq2rat(mpq_t t, int *nu, int *de)
{
    static mpz_t	zde;
    static mpz_t	znu;
    static int		initialized = 0;

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
    static mpq_t	z;
    static int	initialized = 0;

    if (! initialized) {
	mpq_init(z);
	mpq_set_ui(z, 0, 1);
    }

    return mpq_equal(t, z);
}


#if VERBOSE

int	verbose = 0;

#include <stdarg.h>

int verbose_printf(char *fmt, ...)
{
    va_list	ap;
    int		r;

    va_start(ap, fmt);
    r = vfprintf(stderr, fmt, ap);
    va_end(ap);

    return r;
}

#endif




/*
 * Yes, a 3-dim array:
 * 	part[n_part]
 * 	part[p][n_staff]
 * 	part[p][f][n_voice]
 */
part_p		part;

int		n_part;

tuplet_p	tuplet_current;

tuplet_p	global_tuplet;
int		n_tuplet;


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
	return;
    }

    n->next = NULL;
    n->prev = q->tail;
    q->tail->next = n;
    q->tail = n;
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
    symbol_p	s = calloc(1, sizeof(*s));

    mpq_init(s->start);
    mpq_set(s->start, t_current);

    return s;
}


symbol_p
symbol_clone(symbol_p s)
{
    symbol_p	c = malloc(sizeof(*c));

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


void
nf2_music_init(void)
{
}


void
nf2_music_end(void)
{
}
