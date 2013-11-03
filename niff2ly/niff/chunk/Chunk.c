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

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Chunk.h"

#include "Accidental.h"
#include "AltEndingGraphic.h"
#include "Arpeggio.h"
#include "Articulation.h"
#include "AugDot.h"
#include "Barline.h"
#include "Beam.h"
#include "ChnkLenTable.h"
#include "ChordSymbol.h"
#include "Clef.h"
#include "CustomGraphicChk.h"
#include "DefaultValues.h"
#include "Dynamic.h"
#include "EpsGraphic.h"
#include "FiguredBass.h"
#include "Fingering.h"
#include "FontDescription.h"
#include "FontSymbol.h"
#include "Glissando.h"
#include "GuitarGrid.h"
#include "GuitarTabNum.h"
#include "Hairpin.h"
#include "HarpPedal.h"
#include "KeySignNonstandard.h"
#include "KeySignature.h"
#include "Line.h"
#include "Lyric.h"
#include "MeasureNumbering.h"
#include "MidiDataStream.h"
#include "NiffInfo.h"
#include "Notehead.h"
#include "OctaveSign.h"
#include "Ornament.h"
#include "PageHeader.h"
#include "Parenthesis.h"
#include "Part.h"
#include "PedalOrgan.h"
#include "PedalPiano.h"
#include "Portamento.h"
#include "PsType1Font.h"
#include "PsType3Font.h"
#include "RehearsalMark.h"
#include "RepeatSign.h"
#include "Rest.h"
#include "Slur.h"
#include "StaffGrouping.h"
#include "StaffHeader.h"
#include "Stem.h"
#include "StringTable.h"
#include "SystemHeader.h"
#include "SystemSeparation.h"
#include "TagActivate.h"
#include "TagInactivate.h"
#include "TempoMarkNonstandard.h"
#include "TempoMarking.h"
#include "Text.h"
#include "Tie.h"
#include "TimeSigNonstandard.h"
#include "TimeSignature.h"
#include "TimeSlice.h"
#include "Tremolo.h"
#include "Tuplet.h"

#include "Chunk.h"

#include "niff/tag/Tag.h"


/*
 * cbChunkStart
 * ==========
 * Print the chunk's name and size
 */
RIFFIOSuccess
cbChunkStart(NIFFIOChunkContext *pctxChunk)
{
    char strId[RIFFIO_FOURCC_LIM];

    assert(pctxChunk != 0);
    assert(pctxChunk->pnf != 0);
    assert(pctxChunk->pchunk != 0);

    RIFFIOFOURCCToString(pctxChunk->pchunk->fccId, strId);

    VPRINTF("\nchunk %s (", NIFFIONameChunkId(pctxChunk->pchunk->fccId));
    // VPRINTF("'%s' sizeData=%lu ", strId, pctxChunk->pchunk->sizeData);

    return RIFFIO_OK;
}


/*
 * cbChunkEnd
 * ==========
 * Print the closing parenthesis of a chunk
 */
RIFFIOSuccess
cbChunkEnd(NIFFIOChunkContext *pctxChunk)
{
    // VPRINTF(")) end CHUNK %s ", NIFFIONameChunkId(pctxChunk->pchunk->fccId);
    VPRINTF(")");

    tag_activate_apply();
    tags_reset();

    return RIFFIO_OK;
}


void nf2_chunk_Chunk_init(NIFFIOParser *pparser)
{
    nf2_chunk_Accidental_init(pparser);
    nf2_chunk_AltEndingGraphic_init(pparser);
    nf2_chunk_Arpeggio_init(pparser);
    nf2_chunk_Articulation_init(pparser);
    nf2_chunk_AugDot_init(pparser);
    nf2_chunk_Barline_init(pparser);
    nf2_chunk_Beam_init(pparser);
    nf2_chunk_ChnkLenTable_init(pparser);
    nf2_chunk_ChordSymbol_init(pparser);
    nf2_chunk_Clef_init(pparser);
    nf2_chunk_CustomGraphicChk_init(pparser);
    nf2_chunk_DefaultValues_init(pparser);
    nf2_chunk_Dynamic_init(pparser);
    nf2_chunk_EpsGraphic_init(pparser);
    nf2_chunk_FiguredBass_init(pparser);
    nf2_chunk_Fingering_init(pparser);
    nf2_chunk_FontDescription_init(pparser);
    nf2_chunk_FontSymbol_init(pparser);
    nf2_chunk_Glissando_init(pparser);
    nf2_chunk_GuitarGrid_init(pparser);
    nf2_chunk_GuitarTabNum_init(pparser);
    nf2_chunk_Hairpin_init(pparser);
    nf2_chunk_HarpPedal_init(pparser);
    nf2_chunk_KeySignNonstandard_init(pparser);
    nf2_chunk_KeySignature_init(pparser);
    nf2_chunk_Line_init(pparser);
    nf2_chunk_Lyric_init(pparser);
    nf2_chunk_MeasureNumbering_init(pparser);
    nf2_chunk_MidiDataStream_init(pparser);
    nf2_chunk_NiffInfo_init(pparser);
    nf2_chunk_Notehead_init(pparser);
    nf2_chunk_OctaveSign_init(pparser);
    nf2_chunk_Ornament_init(pparser);
    nf2_chunk_PageHeader_init(pparser);
    nf2_chunk_Parenthesis_init(pparser);
    nf2_chunk_Part_init(pparser);
    nf2_chunk_PedalOrgan_init(pparser);
    nf2_chunk_PedalPiano_init(pparser);
    nf2_chunk_Portamento_init(pparser);
    nf2_chunk_PsType1Font_init(pparser);
    nf2_chunk_PsType3Font_init(pparser);
    nf2_chunk_RehearsalMark_init(pparser);
    nf2_chunk_RepeatSign_init(pparser);
    nf2_chunk_Rest_init(pparser);
    nf2_chunk_Slur_init(pparser);
    nf2_chunk_StaffGrouping_init(pparser);
    nf2_chunk_StaffHeader_init(pparser);
    nf2_chunk_Stem_init(pparser);
    nf2_chunk_StringTable_init(pparser);
    nf2_chunk_SystemHeader_init(pparser);
    nf2_chunk_SystemSeparation_init(pparser);
    nf2_chunk_TagActivate_init(pparser);
    nf2_chunk_TagInactivate_init(pparser);
    nf2_chunk_TempoMarkNonstandard_init(pparser);
    nf2_chunk_TempoMarking_init(pparser);
    nf2_chunk_Text_init(pparser);
    nf2_chunk_Tie_init(pparser);
    nf2_chunk_TimeSigNonstandard_init(pparser);
    nf2_chunk_TimeSignature_init(pparser);
    nf2_chunk_TimeSlice_init(pparser);
    nf2_chunk_Tremolo_init(pparser);
    nf2_chunk_Tuplet_init(pparser);
}


void nf2_chunk_Chunk_end(NIFFIOParser *pparser)
{
    nf2_chunk_Accidental_end(pparser);
    nf2_chunk_AltEndingGraphic_end(pparser);
    nf2_chunk_Arpeggio_end(pparser);
    nf2_chunk_Articulation_end(pparser);
    nf2_chunk_AugDot_end(pparser);
    nf2_chunk_Barline_end(pparser);
    nf2_chunk_Beam_end(pparser);
    nf2_chunk_ChnkLenTable_end(pparser);
    nf2_chunk_ChordSymbol_end(pparser);
    nf2_chunk_Clef_end(pparser);
    nf2_chunk_CustomGraphicChk_end(pparser);
    nf2_chunk_DefaultValues_end(pparser);
    nf2_chunk_Dynamic_end(pparser);
    nf2_chunk_EpsGraphic_end(pparser);
    nf2_chunk_FiguredBass_end(pparser);
    nf2_chunk_Fingering_end(pparser);
    nf2_chunk_FontDescription_end(pparser);
    nf2_chunk_FontSymbol_end(pparser);
    nf2_chunk_Glissando_end(pparser);
    nf2_chunk_GuitarGrid_end(pparser);
    nf2_chunk_GuitarTabNum_end(pparser);
    nf2_chunk_Hairpin_end(pparser);
    nf2_chunk_HarpPedal_end(pparser);
    nf2_chunk_KeySignNonstandard_end(pparser);
    nf2_chunk_KeySignature_end(pparser);
    nf2_chunk_Line_end(pparser);
    nf2_chunk_Lyric_end(pparser);
    nf2_chunk_MeasureNumbering_end(pparser);
    nf2_chunk_MidiDataStream_end(pparser);
    nf2_chunk_NiffInfo_end(pparser);
    nf2_chunk_Notehead_end(pparser);
    nf2_chunk_OctaveSign_end(pparser);
    nf2_chunk_Ornament_end(pparser);
    nf2_chunk_PageHeader_end(pparser);
    nf2_chunk_Parenthesis_end(pparser);
    nf2_chunk_Part_end(pparser);
    nf2_chunk_PedalOrgan_end(pparser);
    nf2_chunk_PedalPiano_end(pparser);
    nf2_chunk_Portamento_end(pparser);
    nf2_chunk_PsType1Font_end(pparser);
    nf2_chunk_PsType3Font_end(pparser);
    nf2_chunk_RehearsalMark_end(pparser);
    nf2_chunk_RepeatSign_end(pparser);
    nf2_chunk_Rest_end(pparser);
    nf2_chunk_Slur_end(pparser);
    nf2_chunk_StaffGrouping_end(pparser);
    nf2_chunk_StaffHeader_end(pparser);
    nf2_chunk_Stem_end(pparser);
    nf2_chunk_StringTable_end(pparser);
    nf2_chunk_SystemHeader_end(pparser);
    nf2_chunk_SystemSeparation_end(pparser);
    nf2_chunk_TagActivate_end(pparser);
    nf2_chunk_TagInactivate_end(pparser);
    nf2_chunk_TempoMarkNonstandard_end(pparser);
    nf2_chunk_TempoMarking_end(pparser);
    nf2_chunk_Text_end(pparser);
    nf2_chunk_Tie_end(pparser);
    nf2_chunk_TimeSigNonstandard_end(pparser);
    nf2_chunk_TimeSignature_end(pparser);
    nf2_chunk_TimeSlice_end(pparser);
    nf2_chunk_Tremolo_end(pparser);
    nf2_chunk_Tuplet_end(pparser);
}


