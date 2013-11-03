
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "MidiDataStream.h"


static RIFFIOSuccess
cbMidiDataStreamStart(NIFFIOChunkContext *pctxChunk, niffMidiDataStream *p)
{
    cbChunkStart(pctxChunk);
    UNIMPLEMENTED_CHUNK("MidiDataStreamStart");
}


static RIFFIOSuccess
cbMidiDataStreamEnd(NIFFIOChunkContext *pctxChunk, niffMidiDataStream *p)
{
    cbChunkEnd(pctxChunk);
    UNIMPLEMENTED_CHUNK("MidiDataStreamEnd");
}


void nf2_chunk_MidiDataStream_init(NIFFIOParser *pparser)
{
    NIFFIORegisterChunkMidiDataStream(pparser, cbMidiDataStreamStart, cbMidiDataStreamEnd);
}


void nf2_chunk_MidiDataStream_end(NIFFIOParser *pparser)
{
}

