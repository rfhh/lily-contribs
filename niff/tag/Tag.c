
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <niffio.h>

#include "niff/frontend.h"

#include "music/music.h"

#include "Tag.h"
#include "niff/list/Staff.h"

#include "AbsPlacement.h"
#include "AltEnding.h"
#include "AnchorOverride.h"
#include "ArticDirection.h"
#include "BezierIncoming.h"
#include "BezierOutgoing.h"
#include "ChordSymbolsOffset.h"
#include "CustomFontChar.h"
#include "CustomGraphicTag.h"
#include "EndOfSystem.h"
#include "FannedBeam.h"
#include "FigBassOffset.h"
#include "FontID.h"
#include "GraceNote.h"
#include "GuitarGridOffset.h"
#include "GuitarTabTag.h"
#include "Height.h"
#include "ID.h"
#include "Invisible.h"
#include "LargeSize.h"
#include "LineQuality.h"
#include "LogicalPlacement.h"
#include "LyricVerseOffset.h"
#include "MidiPerformance.h"
#include "MultiNodeEndOfSyst.h"
#include "MultiNodeStartOfSyst.h"
#include "NumberOfFlags.h"
#include "NumberOfNodes.h"
#include "NumberStyle.h"
#include "NumStaffLines.h"
#include "Ossia.h"
#include "PartDescOverride.h"
#include "PartID.h"
#include "RefPtOverride.h"
#include "RehearsalOffset.h"
#include "RestNumeral.h"
#include "Silent.h"
#include "SlashedStem.h"
#include "SmallSize.h"
#include "SpacingByPart.h"
#include "SplitStem.h"
#include "StaffName.h"
#include "StaffStep.h"
#include "Thickness.h"
#include "TieDirection.h"
#include "TupletDesc.h"
#include "VoiceID.h"
#include "Width.h"

#include "Tag.h"


typedef struct NF2_TAG nf2_tag_t, *nf2_tag_p;

struct NF2_TAG {
    NIFFIOTagContext	tag;
    void	       *ptr;
    RIFFIOSuccess     (*callback)(NIFFIOTagContext *t, void *p);
    int			partID;
    int			voiceID;
    nf2_tag_p		next;
};


static nf2_tag_p	tag_activate_list;
static nf2_tag_p	tag_activate_new_list;

static int		tag_activate_collect = 0;
static int		tag_inactivate_collect = 0;
static int		tag_activate_partID  = NO_PART_ID;
static int		tag_activate_voiceID = NO_VOICE_ID;


static void
tag_activate_add(NIFFIOTagContext *pctxTag, void *p, void *callback)
{
    if (pctxTag->ptag->tagid == nifftagPartID) {
	tag_activate_partID = *(int *)p;
    } else if (pctxTag->ptag->tagid == nifftagVoiceID) {
	tag_activate_voiceID = *(int *)p;
    } else {
	nf2_tag_p t = calloc(1, sizeof(*t));
	t->tag = *pctxTag;
	t->ptr = p;
	t->callback = callback;

	t->next = tag_activate_new_list;
	tag_activate_new_list = t;
    }
}


void
tag_activate_start(void)
{
    tag_activate_collect = 1;
}


void
tag_activate_stop(void)
{
    nf2_tag_p	t;
    nf2_tag_p	next;

    for (t = tag_activate_new_list; t != NULL; t = next) {
	t->partID  = tag_activate_partID;
	t->voiceID = tag_activate_voiceID;
	next = t->next;
	t->next = tag_activate_list;
	tag_activate_list = t;
    }

    tag_activate_new_list = NULL;
    tag_activate_collect = 0;
    tag_activate_partID  = NO_PART_ID;
    tag_activate_voiceID = NO_VOICE_ID;
}


void
tag_activate_reset(void)
{
    nf2_tag_p t;

    while (tag_activate_list != NULL) {
	t = tag_activate_list;
	tag_activate_list = t->next;
	free(t);
    }
}


static void
tag_activate_delete(NIFFIOTagContext *pctxTag)
{
    nf2_tag_p t;
    nf2_tag_p prev;

    t = tag_activate_list;
    while (t != NULL && pctxTag->ptag->tagid != t->tag.ptag->tagid) {
	prev = t;
	t = t->next;
    }

    if (t == NULL) {
	fprintf(stderr, "Want to deactivate tag %s but cannot find it\n",
		NIFFIONameTagId(pctxTag->ptag->tagid));
	return;
    }

    if (t == tag_activate_list) {
	tag_activate_list = t->next;
    } else {
	prev->next = t->next;
    }
    free(t);
}


void
tag_inactivate_start(void)
{
    tag_inactivate_collect = 1;
}


void
tag_inactivate_stop(void)
{
    tag_inactivate_collect = 0;
}


void
tag_activate_apply(void)
{
    nf2_tag_p	t;

    if (tag_activate_collect) {
	return;
    }

    for (t = tag_activate_list; t != NULL; t = t->next) {
	if (t->partID != NO_PART_ID && partID_current != t->partID) {
	    continue;
	}
	if (t->voiceID != NO_VOICE_ID && voiceID_current != t->voiceID) {
	    continue;
	}
	t->callback(&t->tag, t->ptr);
    }
}


void
tags_reset(void)
{
    ID_current = NO_ID;
    MultiN = 0;
    if (staff_partID == NO_PART_ID) {
	part_current = NULL;
	voice_current = &voice_unparted;
    }
    partID_current = staff_partID;
}


RIFFIOSuccess
cbTagStart(NIFFIOTagContext *pctxTag, void *p, void *callback)
{
    assert(pctxTag != 0);
    assert(pctxTag->pnf != 0);
    assert(pctxTag->pchunkParent != 0);

    if (tag_activate_collect) {
	tag_activate_add(pctxTag, p, callback);
	return 0;
    }

    if (tag_inactivate_collect) {
	tag_activate_delete(pctxTag);
	return 0;
    }

    VPRINTF(" /%s", NIFFIONameTagId(pctxTag->ptag->tagid));
    // VPRINTF("%uC sizeData = %d", pctxTag->ptag->tagid, pctxTag->ptag->tagsizeData);

    return 1;
}


RIFFIOSuccess
cbTagEnd(NIFFIOTagContext *pctxTag)
{
    VPRINTF(";");

    return RIFFIO_OK;
}


/* cbTag
 * =====
 * Default callback for any tag
 */
static RIFFIOSuccess
cbTag(NIFFIOTagContext *pctxTag)
{
    if (cbTagStart(pctxTag, NULL, cbTag)) {
	VPRINTF(" unknown meaning");
    }

    return cbTagEnd(pctxTag);
}


void nf2_tag_Tag_init(NIFFIOParser *pparser)
{
    NIFFIORegisterDefaultTag(pparser, cbTag);

    nf2_tag_AbsPlacement_init(pparser);
    nf2_tag_AltEnding_init(pparser);
    nf2_tag_AnchorOverride_init(pparser);
    nf2_tag_ArticDirection_init(pparser);
    nf2_tag_BezierIncoming_init(pparser);
    nf2_tag_BezierOutgoing_init(pparser);
    nf2_tag_ChordSymbolsOffset_init(pparser);
    nf2_tag_CustomFontChar_init(pparser);
    nf2_tag_CustomGraphicTag_init(pparser);
    nf2_tag_EndOfSystem_init(pparser);
    nf2_tag_FannedBeam_init(pparser);
    nf2_tag_FigBassOffset_init(pparser);
    nf2_tag_FontID_init(pparser);
    nf2_tag_GraceNote_init(pparser);
    nf2_tag_GuitarGridOffset_init(pparser);
    nf2_tag_GuitarTabTag_init(pparser);
    nf2_tag_Height_init(pparser);
    nf2_tag_ID_init(pparser);
    nf2_tag_Invisible_init(pparser);
    nf2_tag_LargeSize_init(pparser);
    nf2_tag_LineQuality_init(pparser);
    nf2_tag_LogicalPlacement_init(pparser);
    nf2_tag_LyricVerseOffset_init(pparser);
    nf2_tag_MidiPerformance_init(pparser);
    nf2_tag_MultiNodeEndOfSyst_init(pparser);
    nf2_tag_MultiNodeStartOfSyst_init(pparser);
    nf2_tag_NumberOfFlags_init(pparser);
    nf2_tag_NumberOfNodes_init(pparser);
    nf2_tag_NumberStyle_init(pparser);
    nf2_tag_NumStaffLines_init(pparser);
    nf2_tag_Ossia_init(pparser);
    nf2_tag_PartDescOverride_init(pparser);
    nf2_tag_PartID_init(pparser);
    nf2_tag_RefPtOverride_init(pparser);
    nf2_tag_RehearsalOffset_init(pparser);
    nf2_tag_RestNumeral_init(pparser);
    nf2_tag_Silent_init(pparser);
    nf2_tag_SlashedStem_init(pparser);
    nf2_tag_SmallSize_init(pparser);
    nf2_tag_SpacingByPart_init(pparser);
    nf2_tag_SplitStem_init(pparser);
    nf2_tag_StaffName_init(pparser);
    nf2_tag_StaffStep_init(pparser);
    nf2_tag_Thickness_init(pparser);
    nf2_tag_TieDirection_init(pparser);
    nf2_tag_TupletDesc_init(pparser);
    nf2_tag_VoiceID_init(pparser);
    nf2_tag_Width_init(pparser);
}


void nf2_tag_Tag_end(NIFFIOParser *pparser)
{
    nf2_tag_AbsPlacement_end(pparser);
    nf2_tag_AltEnding_end(pparser);
    nf2_tag_AnchorOverride_end(pparser);
    nf2_tag_ArticDirection_end(pparser);
    nf2_tag_BezierIncoming_end(pparser);
    nf2_tag_BezierOutgoing_end(pparser);
    nf2_tag_ChordSymbolsOffset_end(pparser);
    nf2_tag_CustomFontChar_end(pparser);
    nf2_tag_CustomGraphicTag_end(pparser);
    nf2_tag_EndOfSystem_end(pparser);
    nf2_tag_FannedBeam_end(pparser);
    nf2_tag_FigBassOffset_end(pparser);
    nf2_tag_FontID_end(pparser);
    nf2_tag_GraceNote_end(pparser);
    nf2_tag_GuitarGridOffset_end(pparser);
    nf2_tag_GuitarTabTag_end(pparser);
    nf2_tag_Height_end(pparser);
    nf2_tag_ID_end(pparser);
    nf2_tag_Invisible_end(pparser);
    nf2_tag_LargeSize_end(pparser);
    nf2_tag_LineQuality_end(pparser);
    nf2_tag_LogicalPlacement_end(pparser);
    nf2_tag_LyricVerseOffset_end(pparser);
    nf2_tag_MidiPerformance_end(pparser);
    nf2_tag_MultiNodeEndOfSyst_end(pparser);
    nf2_tag_MultiNodeStartOfSyst_end(pparser);
    nf2_tag_NumberOfFlags_end(pparser);
    nf2_tag_NumberOfNodes_end(pparser);
    nf2_tag_NumberStyle_end(pparser);
    nf2_tag_NumStaffLines_end(pparser);
    nf2_tag_Ossia_end(pparser);
    nf2_tag_PartDescOverride_end(pparser);
    nf2_tag_PartID_end(pparser);
    nf2_tag_RefPtOverride_end(pparser);
    nf2_tag_RehearsalOffset_end(pparser);
    nf2_tag_RestNumeral_end(pparser);
    nf2_tag_Silent_end(pparser);
    nf2_tag_SlashedStem_end(pparser);
    nf2_tag_SmallSize_end(pparser);
    nf2_tag_SpacingByPart_end(pparser);
    nf2_tag_SplitStem_end(pparser);
    nf2_tag_StaffName_end(pparser);
    nf2_tag_StaffStep_end(pparser);
    nf2_tag_Thickness_end(pparser);
    nf2_tag_TieDirection_end(pparser);
    nf2_tag_TupletDesc_end(pparser);
    nf2_tag_VoiceID_end(pparser);
    nf2_tag_Width_end(pparser);
}

