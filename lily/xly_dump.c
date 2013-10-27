
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gmp.h>

#include "niff/tag/ID.h"

#include "music/music.h"

#include "xly_partial.h"
#include "xly_dump.h"

void debugMeAt(mpq_t t);
void debugMe(void);


static clef_p   clef_current;
static int     *measure_accidental;
static symbol_p time_line;

/* Want to remove redundant bar checks. Remember whether our last symbol
 * was a bar check or something else: a note/rest/skip. */
static symbol_p last_dumped_symbol;
static symbol_p last_dumped_note;
static symbol_t sym_any_skip;

static time_signature_p time_sig_current;

static FILE    *lily_out;


#define INDENT  4

static int      indentation;


#define ONLY    -1       /* make this -1 if you don't want just 1 staff */


static void
newline(void)
{
    int         i;

    fprintf(lily_out, "\n");
    for (i = 0; i < indentation - 1; i++) {
        fprintf(lily_out, " ");
    }
}


static void
indup(void)
{
    indentation += INDENT;
    newline();
}


static void
indown(void)
{
    indentation -= INDENT;
    newline();
}


static int      dump_tuplet_current;


static void
tuplet_push(int *s, int t)
{
    global_tuplet[t].next = *s;
    *s = t;
}


static void
tuplet_pop(int *s)
{
    *s = global_tuplet[*s].next;
}


static char *
key_sign_name(int key)
{
    switch (key) {
    case 0:     return "c";

    case 1:     return "g";
    case 2:     return "d";
    case 3:     return "a";
    case 4:     return "e";
    case 5:     return "b";
    case 6:     return "fis";
    case 7:     return "cis";

    case 8:     return "f";
    case 9:     return "bes";
    case 10:    return "es";
    case 11:    return "as";
    case 12:    return "des";
    case 13:    return "ges";
    case 14:    return "ces";
    }

    return NULL;
}


static void
key_set(symbol_p s, int code, voice_p voice)
{
    if (code == KEY_RESET) {
        if (voice->key_current != KEY_RESET) {
            voice->key_previous_current = voice->key_current;
            voice->key_current = KEY_RESET;
        }
        return;
    }

    if (code < 0) {
        assert(voice->key_current > -code);
        assert(voice->key_current < 8 || -code >= 8);
        assert(voice->key_current >= 8 || -code < 8);
        voice->key_current += code;
    } else {
        voice->key_current = code;
    }

    if (voice->key_current != voice->key_previous_current) {
        last_dumped_symbol = s;
        fprintf(lily_out, " \\key %s \\major",
                key_sign_name(voice->key_current));
        newline();
    }

    VPRINTF("Dump this key; key_current := %d\n", voice->key_current);
}


static int
defaultKey(int s, voice_p voice)
{
    static int  step[15][OCTAVE_DIATON] = {
                        /* c,  d,  e,  f,  g,  a,  b */
                        {  0,  0,  0,  0,  0,  0,  0 }, /* C */
                        {  0,  0,  0,  1,  0,  0,  0 }, /* 1# G */
                        {  1,  0,  0,  1,  0,  0,  0 }, /* 2# D */
                        {  1,  0,  0,  1,  1,  0,  0 }, /* 3# A */
                        {  1,  1,  0,  1,  1,  0,  0 }, /* 4# E */
                        {  1,  1,  0,  1,  1,  1,  0 }, /* 5# B */
                        {  1,  1,  1,  1,  1,  1,  0 }, /* 6# Fis */
                        {  1,  1,  1,  1,  1,  1,  1 }, /* 7# Cis */
                        {  0,  0,  0,  0,  0,  0, -1 }, /* 1b F */
                        {  0,  0, -1,  0,  0,  0, -1 }, /* 2b Bes */
                        {  0,  0, -1,  0,  0, -1, -1 }, /* 3b Es */
                        {  0, -1, -1,  0,  0, -1, -1 }, /* 4b As */
                        {  0, -1, -1,  0, -1, -1, -1 }, /* 5b Des */
                        { -1, -1, -1,  0, -1, -1, -1 }, /* 6b Ges */
                        { -1, -1, -1, -1, -1, -1, -1 }};        /* 7b Ces */

    if (voice->key_current == KEY_RESET) {      /* Missed a C major? */
		if (1) {
			fprintf(stderr, "Warning: key reset not followed by key, ignore\n");
			key_set(NULL, voice->key_previous_current, voice);
		} else {

			fprintf(stderr, "Warning: key reset not followed by key, assume C major\n");
			key_set(NULL, 0, voice);
		}
    }

    return step[voice->key_current][s];
}


static char *
keyName(int s)
{
    static char *n[OCTAVE_DIATON] = {
                        "c",
                        "d",
                        "e",
                        "f",
                        "g",
                        "a",
                        "b"};

    return n[s];
}


static void
dumpLayout(void)
{
    fprintf(lily_out, "\\version \"1.8.0\"\n\n");
}


static int
is_two_pow(unsigned int i)
{
    while (! (i & 0x1)) {
        i >>= 1;
    }

    return i == 0x1;
}


static void
dumpAccidental(int accidental)
{
    switch (accidental) {
    case 1: fprintf(lily_out, "eses"); break;
    case 2: fprintf(lily_out, "es"); break;
    case 3: break;
    case 4: fprintf(lily_out, "is"); break;
    case 5: fprintf(lily_out, "isis"); break;
    case 6: fprintf(lily_out, "esis"); break;
    case 7: fprintf(lily_out, "(3/4 flat)"); break;
    case 8: fprintf(lily_out, "ises"); break;
    case 9: fprintf(lily_out, "(3/4 sharp)"); break;
    }
}


static void
dumpKeySign(mpq_t *t, symbol_p s, voice_p voice)
{
    key_sign_p k = &s->symbol.key_sign;

    key_set(s, k->code, voice);
}


static void
dump_noteval(note_p note, voice_p voice)
{
    /* Find out the pitch */
    int val = note->value + clef_current->offset;
    int octave;
    int key;
    int accidental;

    if (val < 0) {
        octave = - ((- val + OCTAVE_DIATON - 1) / OCTAVE_DIATON);
        val = (val - octave * OCTAVE_DIATON) % OCTAVE_DIATON;
    } else {
        octave = val / OCTAVE_DIATON;
        val = val % OCTAVE_DIATON;
    }

    accidental = measure_accidental[note->value];
    if (note->tie_end != NO_ID) {
        tie_p tie = &ties[note->tie_end];
        accidental = tie->notes[0]->accidental;
    }

    key = defaultKey(val, voice);
    VPRINTF(" dump note, keyed value = %d, accidental %d, measure accidental %d, key ofdset %d\n",
            val, note->accidental, measure_accidental[note->value],
            defaultKey(val, voice));
    fprintf(lily_out, "%s", keyName(val));
    if (note->accidental != 0) {
        dumpAccidental(note->accidental);
        measure_accidental[note->value] = note->accidental;
    } else if (accidental != 0) {
        /* Require the actual accidental for cross-bar ties */
        note->accidental = accidental;
        dumpAccidental(accidental);
    } else {
        switch (key) {
        case 0: break;
        case 1: fprintf(lily_out, "is"); break;
        case 2: fprintf(lily_out, "isis"); break;
        case -1: fprintf(lily_out, "es"); break;
        case -2: fprintf(lily_out, "eses"); break;
        }
    }

    while (octave > 0) {
        fprintf(lily_out, "'");
        octave--;
    }
    while (octave < 0) {
        fprintf(lily_out, ",");
        octave++;
    }
}


static void
dumpSkip(mpq_t *t, const symbol_t *scan)
{
    static int          initialized = 0;
    static mpq_t        dt;
    static int          de;
    static int          nu;
    static mpz_t        zde;
    static mpz_t        znu;
    int rnd;

    if (! initialized) {
        mpq_init(dt);
        mpz_init(zde);
        mpz_init(znu);
        initialized = 1;
    }

    if (mpq_equal(*t, scan->start)) {
        return;
    }

    while (dump_tuplet_current != NO_ID) {
        /* stop */
        fprintf(lily_out, " }");
        tuplet_pop(&dump_tuplet_current);
    }

    if (mpq_cmp(*t, scan->start) > 0) {
        fprintf(stderr, "Uh oh -- start time ");
        mpq_out_str(stderr, 10, scan->start);
        fprintf(stderr, " is too low, should be ");
        mpq_out_str(stderr, 10, *t);
        fprintf(stderr, " -- is your voice analysis correct?\n");
        return;
    }
    mpq_sub(dt, scan->start, *t);
    mpq_get_num(znu, dt);
    mpq_get_den(zde, dt);
    nu = mpz_get_ui(znu);
    de = mpz_get_ui(zde);
    rnd = nu / de;
    if (rnd != 0) {
        fprintf(lily_out, " s1*%d", rnd);
        nu -= rnd * de;
    }
    if (! is_two_pow(de)) {
        fprintf(stderr, "Uh oh -- skip now already a tuplet %d/%d??\n", nu, de);
        fprintf(lily_out, " s1*%d/%d", nu, de);
    } else {
        if (nu != 0) {
            fprintf(lily_out, " s%d*%d", de, nu);
        }
    }
    last_dumped_symbol = &sym_any_skip;
    mpq_set(*t, scan->start);
    VPRINTF(" skip to t = ");
    VPRINT_MPQ(*t);
}


static void
dumpArticulation(symbol_p s)
{
    articulation_p      a = &s->symbol.articulation;

    VPRINTF("OK, an articulation\n");

    switch (a->shape) {
    case artStrongAccent:
        fprintf(lily_out, "-\\marcato");
        break;
    case artMediumAccent:
        fprintf(lily_out, "-\\accent");
        break;
    case artLightAccent:
        fprintf(lily_out, "-\\tenuto");
        break;
    case artStaccato:
        fprintf(lily_out, "-.");
        break;
    case artDownBow:
        fprintf(lily_out, "-\\downbow");
        break;
    case artUpBow:
        fprintf(lily_out, "-\\upbow");
        break;
    case artHarmonic:
        fprintf(lily_out, "-\\flageolet");
        break;
    case artFermata:
        fprintf(lily_out, "-\\fermata");
        break;
    case artArsisSign:
        fprintf(lily_out, "-\\lheel");
        break;
    case artThesisSign:
        fprintf(lily_out, "-\\rheel");
        break;
    case artPlusSign:
        fprintf(lily_out, "-\\stopped");
        break;
    case artVerticalFilledWedge:
        fprintf(lily_out, "-\\staccatissimo");
        break;
    case artDoubleTonguing:
        fprintf(stderr, "Double tounguing articulation not implemented\n");
        break;
    case artTripleTonguing:
        fprintf(stderr, "Triple tounguing articulation not implemented\n");
        break;
    case artSnapPizzicato:
        fprintf(lily_out, "-\\thumb");
        break;

    default:
        fprintf(stderr, "Undefined articulation %d\n", a->shape);
    }
}


static void
dumpNote(mpq_t *t, symbol_p scan, voice_p voice)
{
    static int          initialized = 0;
    static mpq_t        dt;
    static int          de;
    static int          nu;
    note_p              note = &scan->symbol.note;
    note_p              chord = NULL;
    int                 u;
    symbol_p            a;

    if (! initialized) {
        mpq_init(dt);
        initialized = 1;
    }

    VPRINTF("At t = ");
    VPRINT_MPQ(*t);

    dumpSkip(t, scan);

    VPRINTF(" dump note, value = %d, duration = ", note->value);
    VPRINT_MPQ(note->duration);
    VPRINTF(" tie = %d", note->tie_start);
    VPRINTF(" slur> = %d slur< = %d\n",
            note->stem->slur_start, note->stem->slur_end);

    fprintf(lily_out, " ");

    if (0) {
        if (note->stem->tuplet != dump_tuplet_current) {
            /* Tuplet on or off or both? */
            if (global_tuplet[note->stem->tuplet].next != dump_tuplet_current) {
                /* stop */
                fprintf(lily_out, "} ");
                tuplet_pop(&dump_tuplet_current);
            }
            if (global_tuplet[note->stem->tuplet].next == dump_tuplet_current) {
                /* start */
                tuplet_push(&dump_tuplet_current, note->stem->tuplet);
                fprintf(lily_out, " \\times %d/%d {",
                        global_tuplet[dump_tuplet_current].num,
                        global_tuplet[dump_tuplet_current].den);
            }
        }
    }

    if (note->tuplet != dump_tuplet_current) {
        /* Tuplet on or off or both? */
        if (global_tuplet[note->tuplet].next != dump_tuplet_current) {
            /* stop */
            fprintf(lily_out, "} ");
            tuplet_pop(&dump_tuplet_current);
        }
        if (global_tuplet[note->tuplet].next == dump_tuplet_current) {
            /* start */
            tuplet_push(&dump_tuplet_current, note->tuplet);
            fprintf(lily_out, " \\times %d/%d {",
                    global_tuplet[dump_tuplet_current].num,
                    global_tuplet[dump_tuplet_current].den);
        }
    }

    /* Find out the duration */
    mpq2rat(note->duration, &nu, &de);

    if (note->flags & FLAG_REST) {
        if (mpq_equal(time_sig_current->duration, note->duration)) {
            if (note->multibar == 0) {
                last_dumped_note = scan;
                return;
            } else {
                fprintf(lily_out, "R");
            }
        } else {
            fprintf(lily_out, "r");
        }
    } else {
        chord = note->chord;
        if (chord != NULL) {
            fprintf(lily_out, "<");
        }

        if (note->stem->slur_end != NO_ID) {
            fprintf(lily_out, ")");
        }

        dump_noteval(note, voice);
    }

    if (! is_two_pow(de)) {
        fprintf(stderr, "Uh oh -- now already a tuplet??\n");
    } else {
        int i;
        int dots = 0;

        if (nu >= 2 * de) {
            int len;
            if (is_two_pow(nu)) {
                len = nu / de;
            } else {
                assert(is_two_pow(nu + 1));
                len = (nu + 1) / (2 * de);
                while (de > 0) {
                    dots++;
                    de /= 2;
                }
            }
            switch (len) {
            case 2:
                fprintf(lily_out, "\\breve");
                break;
            case 4:
                fprintf(lily_out, "\\longa");
                break;
            default:
                fprintf(stderr, "Have no symbol for note of length %d\n", len);
                break;
            }
        } else {
            if (is_two_pow(nu + 1)) {
                while (nu > 1) {
                    dots++;
                    de /= 2;
                    nu /= 2;
                }
                fprintf(lily_out, "%d", de);
            } else {
                int     ts_de;
                int     ts_nu;
                mpq2rat(time_sig_current->duration, &ts_nu, &ts_de);

                assert(mpq_equal(time_sig_current->duration, note->duration));
                // special type of whole-bar-rest
                fprintf(lily_out, "%d*%d", de, nu);
            }
        }
        for (i = 0; i < dots; i++) {
            fprintf(lily_out, ".");
        }
    }

    while (chord != NULL) {
        fprintf(lily_out, " ");
        dump_noteval(chord, voice);

        chord = chord->chord;
    }

    for (a = note->stem->articulations; a != NULL; a = a->next) {
        dumpArticulation(a);
    }

    if (note->stem->slur_start != NO_ID) {
        fprintf(lily_out, "(");
    }

    if (note->chord != NULL && ! (note->flags & FLAG_REST)) {
        fprintf(lily_out, "> ");
    }

    if (note->tie_start != NO_ID) {
        fprintf(lily_out, "~ ");
    }

    mpq_set(dt, note->duration);
    if (0) {
        for (u = note->stem->tuplet; u != NO_ID; u = global_tuplet[u].next) {
            mpq_mul(dt, dt, global_tuplet[u].ratio);
        }
    }
    for (u = note->tuplet; u != NO_ID; u = global_tuplet[u].next) {
        mpq_mul(dt, dt, global_tuplet[u].ratio);
    }
    if ((note->flags & FLAG_REST) &&
			mpq_equal(time_sig_current->duration, note->duration) &&
			note->multibar > 1) {
		fprintf(lily_out, "*%d ", note->multibar);
		mpq_t mb;
		mpq_init(mb);
		mpq_set_si(mb, note->multibar, 1);
		mpq_mul(dt, dt, mb);
	}
    mpq_add(*t, *t, dt);

    last_dumped_symbol = scan;
    last_dumped_note = scan;
}


static char *
clef_name(clef_p c)
{
    switch (c->shape) {
    case 1:     /* G clef */
        if (c->step == 2) {
            return "\\clef G";
        }
        if (c->step == 0) {
            return "\\clef french";
        }
        return "Unimplemented: G cleff on step ??";
    case 2:     /* F clef */
        if (c->step == 6) {
            return "\\clef F";
        }
        if (c->step == 4) {
            return "\\clef varbaritone";
        }
        if (c->step == 8) {
            return "\\clef subbass";
        }
        return "Unimplemented: F cleff on step ??";
    case 3:     /* C clef */
        if (c->step == 4) {
            return "\\clef C";
        }
        if (c->step == 6) {
            return "\\clef tenor";
        }
        if (c->step == 8) {
            return "\\clef baritone";
        }
        if (c->step == 2) {
            return "\\clef mezzosoprano";
        }
        if (c->step == 0) {
            return "\\clef soprano";
        }
        return "Unimplemented: C cleff on step ??";
    case 4:     /* percussion "clef" */
        return "\\cleff percussion";
    case 5:     /* Double G clef (C'est quoi????) */
        return "Unimplemented: Double G cleff on step ??";
    case 6:     /* TAB for guitar tabulature */
        return "guitar";
    default:
        return "Unimplemented: unknown cleff shape on step ??";
    }
}


static int
clef_equals(clef_p c1, clef_p c2)
{
    if (c1 == c2) {
        return 1;
    }

    return (c1->shape == c2->shape &&
            c1->step == c2->step &&
            c1->offset == c2->offset &&
            c1->octave == c2->octave);
}


static void
dumpClef(symbol_p s)
{
    clef_p      c = &s->symbol.clef;

    if (clef_equals(clef_current, c)) {
        return;
    }

    clef_current = c;

    VPRINTF("At t = ");
    VPRINT_MPQ(s->start);
    VPRINTF(" this clef\n");
    fprintf(lily_out, " %s", clef_name(clef_current));
    newline();
}


static void
dumpRepeat(mpq_t *t, symbol_p s)
{
    repeat_p    r = &s->symbol.repeat;

    if (r->graphic != 10) {
        printf("Implement repeat sign graphic %d\n", r->graphic);
        return;
    }

    switch (r->logic) {
    case 0:
        break;
    case 1:
        fprintf(lily_out, " \\repeat volta 2 {");
        last_dumped_symbol = s;
        indup();
        break;
    case 2:
        indown();
        fprintf(lily_out, " }");
        last_dumped_symbol = s;
        newline();
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        printf("Implement repeat sign logical %d\n", r->logic);
        return;
    }
}


static int
bar_number(mpq_t *now, mpq_t remain)
{
    int         n = 0;
    symbol_p    scan;
    mpq_t       t;
    mpq_t       t2;
    mpq_t       start;
    mpq_t       end;
    mpq_t       num;

    mpq_init(t);
    mpq_init(t2);
    mpq_init(start);
    mpq_init(end);
    mpq_init(num);

    mpq_set_si(start, 0, 1);

    if (time_line == NULL) {
        time_line = symbol_create(start);
        time_line->type = SYM_TIME_SIGNATURE;
        mpq_init(time_line->symbol.time_signature.duration);
        mpq_set_si(time_line->symbol.time_signature.duration, 1, 1);
        time_line->next = NULL;
    }
    scan = time_line;
    
    if (scan->next == NULL) {
        mpq_set(end, *now);
    } else {
        mpq_set(end, scan->next->start);
    }
    mpq_set(remain, *now);
    mpq_sub(remain, remain, xly_t_partial);
    VPRINTF("now = "); VPRINT_MPQ(*now);
    VPRINTF("\n start = "); VPRINT_MPQ(start);
    VPRINTF("\n end = "); VPRINT_MPQ(end);
    VPRINTF("\n remain = "); VPRINT_MPQ(remain);
    VPRINTF("\n");

    while (scan != NULL && mpq_cmp(*now, end) >= 0) {
        int nu;
        int de;

        VPRINTF("\n now = "); VPRINT_MPQ(*now);
        VPRINTF("\n end = "); VPRINT_MPQ(end);

        mpq_sub(t, end, start);
        VPRINTF("\n t = "); VPRINT_MPQ(t);
        mpq_set(t2, t);
        mpq_div(t, t2, scan->symbol.time_signature.duration);
        VPRINTF("\n t = "); VPRINT_MPQ(t);
        mpq2rat(t, &nu, &de);
        VPRINTF("\n t = "); VPRINT_MPQ(t);
        n += nu / de;   /* Discount partial bars */
        mpq_set_si(num, n, 1);
        mpq_mul(t, scan->symbol.time_signature.duration, num);
        mpq_sub(remain, remain, t);
        VPRINTF("\n remain = "); VPRINT_MPQ(remain);
        VPRINTF("\n");

        mpq_set(start, end);
        if (scan->next == NULL || scan->next->next == NULL) {
            mpq_set(end, *now);
        } else {
            mpq_set(end, scan->next->next->start);
        }
        scan = scan->next;
    }

    mpq_clear(t);
    mpq_clear(start);
    mpq_clear(end);
    mpq_clear(num);

    return n;
}


static void
time_line_clear(void)
{
    symbol_p    scan;
    symbol_p    prev;

    scan = time_line;
    while (scan != NULL) {
        prev = scan;
        scan = scan->next;
        symbol_clear(prev);
    }

    time_line = NULL;
}


static void
dumpTimeSig(mpq_t *now, symbol_p s)
{
    symbol_p t;
    symbol_p scan;
    symbol_p prev = NULL;

    if (mpq_equal(time_sig_current->duration, s->symbol.time_signature.duration)) {
        return;
    }

    dumpSkip(now, s);

    t = symbol_clone(s);

    scan = time_line;
    while (scan != NULL) {
        prev = scan;
        scan = scan->next;
    }
    if (prev == NULL) {
        time_line = t;
    } else {
        prev->next = t;
    }
    t->next = NULL;

    time_sig_current->top = t->symbol.time_signature.top;
    time_sig_current->bottom = t->symbol.time_signature.bottom;
    mpq_set(time_sig_current->duration, t->symbol.time_signature.duration);

    VPRINTF("At t = ");
    VPRINT_MPQ(s->start);
    VPRINTF(" this time sig: %d/%d\n",
             time_sig_current->top, time_sig_current->bottom);

    if (time_sig_current->top == -1) {
        fprintf(lily_out, " \\property Staff.TimeSignature \\override #'style = #'C \\time 4/4 ");
    } else if (time_sig_current->top == -2) {
        fprintf(lily_out, " \\property Staff.TimeSignature \\override #'style = #'C \\time 2/2 ");
    } else {
        fprintf(lily_out, " \\time %d/%d ", time_sig_current->top, time_sig_current->bottom);
    }
    newline();
}


static void
dumpOrnament(mpq_t *t, symbol_p s)
{
    ornament_p  o = &s->symbol.ornament;

    VPRINTF("OK, an ornament\n");
    switch (o->shape) {
    case ornMordent:
        fprintf(lily_out, "-\\mordent");
        break;
    case ornInvertedMordent:
        fprintf(lily_out, "-\\mordent");
        break;
    case ornDoubleMordent:
        fprintf(lily_out, "-\\upmordent");
        break;
    case ornInvertedDoubleMordent:
        fprintf(lily_out, "-\\downmordent");
        break;
    case ornTurn:
        fprintf(lily_out, "-\\turn");
        break;
    case ornInvertedTurn:
        fprintf(lily_out, "-\\reverseturn");
        break;
    case ornTr:
        fprintf(lily_out, "-\\trill");
        break;
    case ornTrill2Bumps:
        fprintf(lily_out, "-\\prall");
        break;
    case ornTrill3Bumps:
        fprintf(lily_out, "-\\prallprall");
        break;
    case ornTrillLong:
        fprintf(stderr, "Don't know how to make a spanning trill\n");
        fprintf(lily_out, "-\\prallprall");
        break;
    case ornTrillLongUphook:
        fprintf(stderr, "Don't know how to make a spanning trill\n");
        fprintf(lily_out, "-\\uprall");
        break;
    case ornTrillLongDownhook:
        fprintf(stderr, "Don't know how to make a spanning trill\n");
        fprintf(lily_out, "-\\downprall");
        break;

    default:
        fprintf(stderr, "Undefined ornament %d\n", o->shape);
    }
}


static int
isEmptyMultibar(symbol_p s) {
    if (s->type != SYM_NOTE) {
        return 0;
    }

    note_p note = &s->symbol.note;
    if ((note->flags & FLAG_REST) &&
            mpq_equal(time_sig_current->duration, note->duration) &&
            note->multibar == 0) {
        return 1;
    }

    return 0;
}



static void
dumpBarStart(mpq_t *t, symbol_p s)
{
    VPRINTF("OK, a bar start\n");

    memset(measure_accidental - NOTE_VALUES, 0,
            (2 * NOTE_VALUES + 1) * sizeof(*measure_accidental));
    assert(measure_accidental[-1] == 0);

    if (! mpq_zero(*t) &&
            (last_dumped_symbol == NULL ||
             last_dumped_symbol->type != SYM_BAR_START ||
             ! mpq_equal(last_dumped_symbol->start, s->start))) {
        mpq_t   remain;
        int     num;

        while (dump_tuplet_current != NO_ID) {
            /* stop */
            fprintf(lily_out, " }");
            tuplet_pop(&dump_tuplet_current);
        }

        mpq_init(remain);
        dumpSkip(t, s);

        num = bar_number(t, remain);
        if ((mpq_zero(remain) || last_dumped_symbol->type != SYM_REPEAT) &&
                ! isEmptyMultibar(last_dumped_note)) {
            fprintf(lily_out, " |");
            fprintf(lily_out, " %% bar %d", num);
            last_dumped_symbol = s;
            newline();
        }
        mpq_clear(remain);
    }
}


static void
dumpBarLine(mpq_t *t, symbol_p s)
{
    fprintf(stderr, "Now meet barLine.... implement!\n");
}


static void
dumpVoice(voice_p voice)
{
    symbol_p    scan;
    mpq_t       t;

    mpq_init(t);
    mpq_set_si(t, 0, 1);

    time_line_clear();

    if (! mpq_zero(xly_t_partial)) {
        int     nu;
        int     de;

        mpq2rat(xly_t_partial, &nu, &de);
        fprintf(lily_out, " \\partial %d*%d", de, nu);
        newline();
    }

    for (scan = voice->q.front; scan != NULL; scan = scan->next) {
		debugMeAt(t);
        switch (scan->type) {

        case SYM_ARPEGGIO:
            break;

        case SYM_ARTICULATION:
            abort();
            break;

        case SYM_BARLINE:
            dumpBarLine(&t, scan);
			debugMeAt(t);
            break;

        case SYM_BAR_START:
			debugMeAt(t);
            dumpBarStart(&t, scan);
            break;

        case SYM_CHORD:
            break;

        case SYM_CLEF:
            dumpClef(scan);
            break;

        case SYM_DYNAMIC:
            break;

        case SYM_GLISSANDO:
            break;

        case SYM_HAIRPIN:
            break;

        case SYM_KEY_SIGN:
            dumpKeySign(&t, scan, voice);
			debugMeAt(t);
            break;

        case SYM_MEASURE_NUMBERING:
            break;

        case SYM_MIDI:
            break;

        case SYM_NOTE:
            dumpNote(&t, scan, voice);
            debugMeAt(t);
            break;

        case SYM_OTTAVA:
            break;

        case SYM_ORNAMENT:
            dumpOrnament(&t, scan);
			debugMeAt(t);
            break;

        case SYM_PARENTH:
            break;

        case SYM_PEDAL:
            break;

        case SYM_PORTAMENTO:
            break;

        case SYM_REHEARSAL_MARK:
            break;

        case SYM_REPEAT:
            dumpRepeat(&t, scan);
			debugMeAt(t);
            break;

        case SYM_STEM:
            break;

        case SYM_TEMPO:
            break;

        case SYM_TEXT:
            break;

        case SYM_TIE:
            break;

        case SYM_TIME_SIGNATURE:
            dumpTimeSig(&t, scan);
			debugMeAt(t);
            break;

        case SYM_TREMOLO:
            break;

        case SYM_TUPLET:
            break;

        case SYM_NUMBER:
            break;

        }
    }

    while (dump_tuplet_current != NO_ID) {
        /* stop */
        fprintf(lily_out, " }");
        tuplet_pop(&dump_tuplet_current);
    }

    mpq_clear(t);
}


static char *
i2count(int i)
{
    switch ((i % 4) + 1) {
    case 1:     return "One";
    case 2:     return "Two";
    case 3:     return "Three";
    case 4:     return "Four";
    default:    return "Error";
    }
}


static char *
part_name(int p, int f, int v)
{
    static char name[256];

    assert(p < 26);
    assert(f < 26);
    assert(v < 26);
    sprintf(name, "part%cstaff%cvoice%c", 'A' + p, 'A' + f, 'A' + v);

    return name;
}


static void
clef_clear(void)
{
    clef_current->shape = 1;
    clef_current->step = 2;
    clef_current->offset = 2 + OCTAVE_DIATON;
    clef_current->octave = 0;
}


static void
time_sig_clear(void)
{
    RATIONAL    r;

    r.numerator = 4;
    r.denominator = 4;
    time_sig_current->top = r.numerator;
    time_sig_current->bottom = r.denominator;
    rat2mpq(time_sig_current->duration, &r);
}


static void
voice_reset(void)
{
    dump_tuplet_current = NO_ID;
    time_line_clear();
    time_sig_clear();
    clef_clear();
}


static void
dump_notes(void)
{
    int         f;
    int         p;
    int         v;

    fprintf(stderr, "Now write parts...\n");
    for (p = 0; p < n_part; p++) {
        if (ONLY != -1 && p != ONLY) {
            fprintf(stderr, "Skip staff %d, do only staff %d\n", p, ONLY);
            continue;
        }
        fprintf(stderr, "      ........ part %d, ", p);
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(stderr, "staff %d, ", f);
            for (v = 0; v < part[p].staff[f].n_voice; v++) {
                fprintf(stderr, "voice %d ", v);
                VPRINTF("Now dump part %d staff %d voice %d", p, f, v);
                voice_reset();
                fprintf(lily_out, "%s = \\notes {", part_name(p, f, v));
                indup();
                dumpVoice(&part[p].staff[f].voice[v]);
                indown();
                fprintf(lily_out, "}");
                newline();
                newline();
            }
        }
        fprintf(stderr, "\n");
    }
}


static void
dump_score(void)
{
    int         f;
    int         p;
    int         v;
    int         staff;

    newline();
    fprintf(lily_out, "\\score {");
    indup();
    fprintf(lily_out, "<");
    indup();

    staff = 0;
    for (p = 0; p < n_part; p++) {
        if (ONLY != -1 && p != ONLY) {
            continue;
            fprintf(stderr, "Skip staff %d, do only staff %d\n", p, ONLY);
        }
        for (f = 0; f < part[p].n_staff; f++) {
            fprintf(lily_out, " \\context Staff = staff%c <", 'A' + staff);
            staff++;
            indup();
            if (part[p].staff[f].n_voice == 1) {
                fprintf(lily_out, "\\%s", part_name(p, f, 0));
            } else {
                for (v = 0; v < part[p].staff[f].n_voice; v++) {
                    char *count = i2count(v);
                    fprintf(lily_out, " \\context Voice=%s {", count);
                    indup();
                    fprintf(lily_out, "\\voice%s \\%s", count, part_name(p, f, v));
                    indown();
                    fprintf(lily_out, "}");
                    newline();
                }
            }
            indown();
            fprintf(lily_out, " >");
            newline();
        }
    }

    indown();
    fprintf(lily_out, ">");
    indown();
    fprintf(lily_out, "}");
    newline();
}


static void
dumpFinish(void)
{
}


void
xly_dump_file(FILE *f)
{
    lily_out = f;

    dumpLayout();
    dump_notes();
    dump_score();

    dumpFinish();
}


void
xly_dump_init(void)
{
    clef_current = malloc(sizeof(*clef_current));
    clef_clear();

    measure_accidental = calloc(2 * NOTE_VALUES + 1, sizeof(int));
    measure_accidental += NOTE_VALUES;

    time_sig_current = calloc(1, sizeof(*time_sig_current));
    mpq_init(time_sig_current->duration);
    time_sig_clear();
}


void
xly_dump_end(void)
{
}
