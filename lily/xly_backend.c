
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gmp.h>

#include "music/music.h"

#include "xly_backend.h"
#include "xly_chord.h"
#include "xly_dump.h"
#include "xly_partial.h"
#include "xly_repeat.h"
#include "xly_voice.h"


void
xly_dump(FILE *f, int do_chording)
{
    xly_partial();
    xly_chord(do_chording);
    xly_repeat();
    xly_voice();

    xly_dump_file(f);
}


void
xly_init(void)
{
    xly_partial_init();
    xly_chord_init();
    xly_repeat_init();
    xly_voice_init();
    xly_dump_init();
}


void
xly_end(void)
{
    xly_dump_end();
    xly_partial_end();
    xly_repeat_end();
    xly_chord_end();
    xly_voice_end();
}
