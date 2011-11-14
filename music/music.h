#ifndef __NIFF2LY_MUSIC_MUSIC_H__
#define __NIFF2LY_MUSIC_MUSIC_H__

#include <niffio.h>

#include <gmp.h>

#include <stddef.h>


#define VERBOSE		1

#if VERBOSE

#include <stdio.h>

int verbose_printf(char *fmt, ...);

int mpq_printf(FILE *f, const mpq_t t);
int mpq_dump(const mpq_t t);


extern int	verbose;
#define VPRINTF(s)      ((verbose) ? verbose_printf s : RIFFIO_OK)
#define VPRINT_MPQ(t)   ((verbose) ? mpq_out_str(stderr, 10, t) : RIFFIO_OK)
#else
#define VPRINTF(s)
#define VPRINT_MPQ(t)
#endif


#define OCTAVE_DIATON	7
#define OCTAVE_CHROM	11

#define NOTE_VALUES	256	/* Support these line values for notes */



typedef enum SYMBOL_TYPE {
    SYM_ARPEGGIO,
    SYM_ARTICULATION,
    SYM_BARLINE,
    SYM_BAR_START,
    SYM_CHORD,
    SYM_CLEF,
    SYM_DYNAMIC,
    SYM_GLISSANDO,
    SYM_HAIRPIN,
    SYM_KEY_SIGN,
    SYM_MEASURE_NUMBERING,
    SYM_MIDI,
    SYM_NOTE,
    SYM_OTTAVA,
    SYM_ORNAMENT,
    SYM_PARENTH,
    SYM_PEDAL,
    SYM_PORTAMENTO,
    SYM_REHEARSAL_MARK,
    SYM_REPEAT,
    SYM_STEM,
    SYM_TEMPO,
    SYM_TEXT,
    SYM_TIE,
    SYM_TIME_SIGNATURE,
    SYM_TREMOLO,
    SYM_TUPLET,
    SYM_NUMBER
} symbol_type_t, *symbol_type_p;



typedef struct arpeggio arpeggio_t, *arpeggio_p;
typedef struct articulation articulation_t, *articulation_p;
typedef struct barline barline_t, *barline_p;
typedef struct bar_start bar_start_t, *bar_start_p;
typedef struct chord chord_t, *chord_p;
typedef struct clef clef_t, *clef_p;
typedef struct dynamic dynamic_t, *dynamic_p;
typedef struct glissando glissando_t, *glissando_p;
typedef struct hairpin hairpin_t, *hairpin_p;
typedef struct key_sign key_sign_t, *key_sign_p;
typedef struct measure_numbering measure_numbering_t, *measure_numbering_p;
typedef struct midi midi_t, *midi_p;
typedef struct note note_t, *note_p;
typedef struct ottava ottava_t, *ottava_p;
typedef struct ornament ornament_t, *ornament_p;
typedef struct parenth parenth_t, *parenth_p;
typedef struct pedal pedal_t, *pedal_p;
typedef struct portamento portamento_t, *portamento_p;
typedef struct rehearsal_mark rehearsal_mark_t, *rehearsal_mark_p;
typedef struct repeat repeat_t, *repeat_p;
typedef struct stem stem_t, *stem_p;
typedef struct tempo tempo_t, *tempo_p;
typedef struct text text_t, *text_p;
typedef struct tie tie_t, *tie_p;
typedef struct time_signature time_signature_t, *time_signature_p;
typedef struct tremolo tremolo_t, *tremolo_p;
typedef struct tuplet tuplet_t, *tuplet_p;

typedef struct SYMBOL symbol_t, *symbol_p;


struct arpeggio {
};


struct articulation {
    int			shape;
};


struct barline {
};


struct bar_start {
};


struct chord {
};


struct clef {
    int		shape;
    int		step;
    int		octave;
    int		offset;
};


struct dynamic {
};


struct glissando {
};


struct hairpin {
};


#define KEY_RESET	127

struct key_sign {
    int		code;
};


struct measure_numbering {
};


struct midi {
};


typedef enum NOTE_FLAGS {
    FLAG_GRACE		= (0x1 << 0),
    FLAG_SLASHED	= (0x1 << 1),
    FLAG_SMALL		= (0x1 << 2),
    FLAG_INVISIBLE	= (0x1 << 3),
    FLAG_REST		= (0x1 << 4),
    FLAG_STEM_EXPLICIT	= (0x1 << 5),
    FLAG_STEM_UP	= (0x1 << 6)
} note_flags_t, *note_flags_p;


struct note {
    mpq_t		duration;
    int			value;
    int			accidental;
    int			voice;
    stem_p		stem;
    note_flags_t	flags;
    note_p		chord;
    int			tie_start;
    int			tie_end;
};


struct ottava {
};


struct ornament {
    int			shape;
};


struct parenth {
};


struct pedal {
};


struct portamento {
};


struct rehearsal_mark {
};


struct repeat {
    int		graphic;
    int		logic;
};


struct stem {
    mpq_t		t;
    int			voiceID;
    int			partID;
    int			height;
    note_flags_t	flags;
    int			tuplet;
    int			beam;
    short int		beam_left;
    short int		beam_right;
    int			slur_start;
    int			slur_end;
    symbol_p		articulations;
};


struct tempo {
};


struct text {
};


struct tie {
    int		occur;
    int		occurred;
    note_p     *notes;
};


struct time_signature {
    int		top;
    int		bottom;
    mpq_t	duration;
};


struct tremolo {
};


struct tuplet {
    int		num;
    int		den;
    mpq_t	ratio;		/* Multiply by this to get real time val */
    BYTE	groupingSymbol;
    int		ID;
    int		next;
};



struct SYMBOL {
    symbol_type_t	type;
    mpq_t		start;
    union {
	arpeggio_t	arpeggio;
	articulation_t	articulation;
	barline_t	barline;
	bar_start_t	bar_start;
	chord_t		chord;
	clef_t		clef;
	dynamic_t	dynamic;
	glissando_t	glissando;
	hairpin_t	hairpin;
	key_sign_t	key_sign;
	measure_numbering_t	measure_numbering;
	midi_t		midi;
	note_t		note;
	ottava_t	ottava;
	ornament_t	ornament;
	parenth_t	parenth;
	pedal_t		pedal;
	portamento_t	portamento;
	rehearsal_mark_t	rehearsal_mark;
	repeat_t	repeat;
	stem_t		stem;
	tempo_t		tempo;
	text_t		text;
	tie_t		tie;
	time_signature_t	time_signature;
	tremolo_t	tremolo;
	tuplet_t	tuplet;
    } symbol;
    symbol_p		prev;
    symbol_p		next;
};


#define SYMBOL_OF_SYM(sym)   ((symbol_p)((char *)(sym) - offsetof(symbol_t, symbol)))


typedef struct symbol_q {
    symbol_p	front;
    symbol_p	tail;
} symbol_q_t, *symbol_q_p;


#define MAX_CONCURRENT_SLURS	10

typedef struct voice {
    mpq_t	t_finish;
    note_p	tail;
    int		id;
    symbol_q_t	q;
    int		key_current;
    int		key_previous_current;
    int		n_slur;
    int		slur[MAX_CONCURRENT_SLURS];
} voice_t, *voice_p;


typedef struct staff {
    clef_p	clef;
    int		n_voice;
    int		slur_pending;
    symbol_p	next_after_backtrack;
    symbol_p	start_backtrack;
    symbol_q_t	unvoiced;
    symbol_q_t	replicated;
    voice_p	voice;
} staff_t, *staff_p;


typedef struct part {
    int		n_staff;
    staff_p	staff;
} part_t, *part_p;



/*
 * Yes, a 3-dim array:
 * 	part[n_part]
 * 	part[p][n_staff]
 * 	part[p][f][n_voice]
 */
extern part_p		part;

extern int		n_part;

extern tuplet_p		tuplet_current;

extern tuplet_p		global_tuplet;
extern int		n_tuplet;


extern int		n_ties;
extern tie_p		ties;

void ties_increase(int ID);


typedef struct SLUR {
    int		n;
    int		pending;
} slur_t, *slur_p;

extern slur_p	slur;
extern int	n_slurs;

void slurs_increase(int ID);


void q_insert(symbol_q_p q, symbol_p n);
void q_append(symbol_q_p q, symbol_p n);
void q_remove(symbol_q_p q, symbol_p n);

void stack_push(symbol_p *s, symbol_p n);

void rat2mpq(mpq_t q, RATIONAL *r);
void mpq2rat(mpq_t q, int *nu, int *de);
int  mpq_zero(mpq_t t);

symbol_p symbol_create(mpq_t t_current);
symbol_p symbol_clone(symbol_p s);
void symbol_clear(symbol_p s);

void nf2_music_init(void);
void nf2_music_end(void);

#endif
