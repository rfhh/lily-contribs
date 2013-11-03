
#ifndef lint
static char rcsid[] = 
"$Id: clt.c,v 1.2 1996/06/15 02:53:52 tim Exp $";
#endif
/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
/***************************************************************************/
/*
 * NAME
 * ====
 * NIFFIO chunk length table routines.
 *
 * - NIFFIOCLTNew()
 * - NIFFIOCLTDelete()
 *
 * - NIFFIOCLTMakeEntry()
 * - NIFFIOCLTMakeDefaultEntries()
 * - NIFFIOCLTLookup()
 *
 * - NIFFIOCLTCount()
 * - NIFFIOCompareFOURCC()
 * 
 */
/***************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "niffio.h"

/*
 * Chunk length tables are implemented as RIFFIOFCCTables.
 * Store the FOURCC in the RIFFIOFCCTableEntry.fcc
 * Store the chunk length in the RIFFIOTableEntry.value.l
 */

/*
 * Provide a canned set of CLT entries in our own private array
 *
 * I count about 63 different chunk types 
 */
#define DEFAULT_CLT_LIM 62
static niffChklentabEntry _DefaultCLTEntries[]; 

static
RIFFIOSuccess
NIFFIOPCLTReallocate(NIFFIOChunkLengthTable *pclt, unsigned newReserved);


/***************************************************************************/
/*
 * NIFFIOCLTNew
 * ============
 * Return a new, empty NIFFIOChunkLengthTable, or null on failure.
 */
NIFFIOChunkLengthTable *
NIFFIOCLTNew()
/***************************************************************************/
{
    NIFFIOChunkLengthTable *pclt;  /* The table to return */
    
    pclt = RIFFIOFCCTableNew();
    
    return pclt;


}

/***************************************************************************/
/*
 * NIFFIOCLTDelete
 * ===============
 * Free the memory allocated to a chunk length table.
 */
void
NIFFIOCLTDelete(NIFFIOChunkLengthTable *pclt)
/***************************************************************************/
{
    assert(pclt != 0);

    RIFFIOFCCTableDelete(pclt);

}

/***************************************************************************/
/*
 * NIFFIOCLTMakeEntry
 * ==================
 * Make an entry into a chunk length table.
 */
RIFFIOSuccess
NIFFIOCLTMakeEntry(NIFFIOChunkLengthTable *pclt, 
                   niffChklentabEntry cltEntry)
/*
 * ENTRY
 * -----
 * T <cltEntry> is filled with the values for the new entry
 * 
 * EXIT
 * ----
 *  - T <cltEntry> is added to <*pclt>
 *  - If an existing entry in <*pclt> matches <cltEntry> then
 *    the entry's value is replaced.
 */
/***************************************************************************/
{
    char strModule[] = "NIFFIOCLTMakeEntry";
    char strChunkName[RIFFIO_FOURCC_LIM];
    
    RIFFIOFCCTableEntry  tableEntry; /* interface to RIFFIOFCCTableMakeEntry */
    
    RIFFIOFOURCCToString(cltEntry.chunkName, strChunkName);
    
    /*
     * Watch out for bogus chunkNames
     */
    if (!(RIFFIOFOURCCIsValid(cltEntry.chunkName)))
    {
        RIFFIOError(strModule, "Entry has an invalid chunkName <%s>",
                    strChunkName);
        return RIFFIO_FAIL;
    }
    
    tableEntry.fcc = cltEntry.chunkName;
    tableEntry.value.l = cltEntry.offsetOfFirstTag;
    
    return RIFFIOFCCTableMakeEntry(pclt, tableEntry);
    
}

/***************************************************************************/
/*
 * NIFFIOCLTMakeDefaultEntries
 * ===========================
 * Fill a chunk length table with "canned" entries.
 */
RIFFIOSuccess
NIFFIOCLTMakeDefaultEntries(NIFFIOChunkLengthTable *pclt)
/*
 * This routine is useful if you don't need a custom chunk length
 * table.  It fills an existing chunk length table with a complete set
 * of chunk length entries for each chunk in the NIFF specification.
 *
 * TWB - Should this include INFO chunks? probably.
 */
/**************************************************************************/
{

    niffChklentabEntry *pentry;

    assert(pclt);
    
    /* Insert our default entries until we 
     * reach the sentinel offset of -9999 
     */
    for (pentry = _DefaultCLTEntries; 
         pentry->offsetOfFirstTag != -9999; 
         pentry++)
    {
        if (! NIFFIOCLTMakeEntry(pclt, *pentry))
            return RIFFIO_FAIL;
    }
    
    return RIFFIO_OK;
    
}

/**************************************************************************/
/*
 * NIFFIOCLTLookup
 * ===============
 * Lookup a chunk length given an FOURCC.
 */
RIFFIOSuccess
NIFFIOCLTLookup(NIFFIOChunkLengthTable *pclt, niffChklentabEntry *cltEntryp)
/*
 * ENTRY
 * -----
 * T <cltEntryp->chunkName> contains the FOURCC to look up in <*pclt>.
 *
 * EXIT
 * ----
 * Fills in <cltEntryp->offsetOfFirstTag> according to <*pclt>.
 *
 * RETURN
 * ------
 *  RIFFIO_OK :
 *     if <cltEntryp->chunkName> was found.
 *
 *  RIFFIO_FAIL : 
 *     otherwise 
 */
/**************************************************************************/
{
    RIFFIOSuccess findResult;
    RIFFIOFCCTableEntry tableEntry;
    
    assert(pclt);
    assert(cltEntryp);
    
    tableEntry.fcc = cltEntryp->chunkName;
    findResult = RIFFIOFCCTableLookup(pclt, &tableEntry);
    
    if (findResult)
    {
        cltEntryp->offsetOfFirstTag = tableEntry.value.l;
    }
    
    return findResult;
}


/**************************************************************************/
/*
 * NIFFIOCLTCount
 * ==============
 * Return the number of entries in a chunk length table
 */
unsigned
NIFFIOCLTCount(NIFFIOChunkLengthTable *pclt)
/**************************************************************************/
{
    assert(pclt != 0);
    return RIFFIOFCCTableCount(pclt);
}

/**************************************************************************/
/* 
 * NIFFIOCompareFOURCC
 * ===================
 * Compare FOURCCs alphabetically.
 */
int
NIFFIOCompareFOURCC(FOURCC fccA, FOURCC fccB)
/*
 * RETURN
 * ------
 *       * <0 if a comes before b
 *       *  0 if a is the same as b
 *       * >0 if a comes after b
 *
 * The return results are compatible with bsearch()  
 * and strcmp() in the Standard C library.
 *
 */
/**************************************************************************/
{
    char strA[RIFFIO_FOURCC_LIM];
    char strB[RIFFIO_FOURCC_LIM];
    
    RIFFIOFOURCCToString(fccA, strA);
    RIFFIOFOURCCToString(fccB, strB);
    
    return strcmp(strA, strB);
    
}

#ifndef NDEBUG

/*
 * Make sure our canned CLT entries jive with a given chunk length table
 */
RIFFIOSuccess
NIFFIOCLTVerifyDefaults(NIFFIOChunkLengthTable *pclt)
{

    RIFFIOSuccess bFound;
    int i;     /* index into default chunk length table entries */
    niffChklentabEntry cltEntry; /* entry to lookup in clt */ 
    
    assert(pclt);
    
    for (i = 0; i < DEFAULT_CLT_LIM; i++)
    {
        char strFOURCC[RIFFIO_FOURCC_LIM];
        
        cltEntry.chunkName = _DefaultCLTEntries[i].chunkName;
        
        bFound = NIFFIOCLTLookup(pclt, &cltEntry);
        assert (bFound);
        assert (cltEntry.chunkName == _DefaultCLTEntries[i].chunkName);
        assert (cltEntry.offsetOfFirstTag 
                == _DefaultCLTEntries[i].offsetOfFirstTag);
        RIFFIOFOURCCToString(cltEntry.chunkName, strFOURCC);
        printf("<%s> %ld\n", strFOURCC, cltEntry.offsetOfFirstTag);
        
    }
    
    return RIFFIO_OK;
}


#endif

/*
 * Here are our canned CLT entries
 * 
 * WARNING
 * -------
 * If you add an entry, you must change DEFAULT_CLT_LIM.
 * A better way to do this would be to store a sentinel value
 * at the end of the array.
 */
#define DEFCLTENTRY(name) \
{niffckid##name, niffcklen##name},

static 
niffChklentabEntry _DefaultCLTEntries[] = 
{
    DEFCLTENTRY(ChnkLenTable)
    DEFCLTENTRY(DefaultValues)
    DEFCLTENTRY(EpsGraphic)
    DEFCLTENTRY(FontDescription)
    DEFCLTENTRY(NiffInfo)
    DEFCLTENTRY(Part)
    DEFCLTENTRY(PsType1Font)
    DEFCLTENTRY(PsType3Font)
    DEFCLTENTRY(StaffGrouping)
    DEFCLTENTRY(StringTable)
    
    DEFCLTENTRY(PageHeader)
    DEFCLTENTRY(StaffHeader)
    DEFCLTENTRY(SystemHeader)
    DEFCLTENTRY(TimeSlice)
    DEFCLTENTRY(Accidental)
    DEFCLTENTRY(AltEndingGraphic)
    DEFCLTENTRY(Arpeggio)
    DEFCLTENTRY(Articulation)
    DEFCLTENTRY(AugDot)
    DEFCLTENTRY(Barline)
    
    DEFCLTENTRY(Beam)
    DEFCLTENTRY(ChordSymbol)
    DEFCLTENTRY(Clef)
    DEFCLTENTRY(CustomGraphicChk)
    DEFCLTENTRY(Dynamic)
    DEFCLTENTRY(FiguredBass)
    DEFCLTENTRY(Fingering)
    DEFCLTENTRY(Glissando)
    DEFCLTENTRY(GuitarGrid)
    DEFCLTENTRY(GuitarTabNum)
    
    DEFCLTENTRY(Hairpin)
    DEFCLTENTRY(HarpPedal)
    DEFCLTENTRY(KeySignature)
    DEFCLTENTRY(KeySignNonstandard)
    DEFCLTENTRY(Line)
    DEFCLTENTRY(Lyric)
    DEFCLTENTRY(MeasureNumbering)
    DEFCLTENTRY(MidiDataStream)
    DEFCLTENTRY(FontSymbol)
    DEFCLTENTRY(Notehead)
    
    DEFCLTENTRY(OctaveSign)
    DEFCLTENTRY(Ornament)
    DEFCLTENTRY(Parenthesis)
    DEFCLTENTRY(PedalPiano)
    DEFCLTENTRY(PedalOrgan)
    DEFCLTENTRY(Portamento)
    DEFCLTENTRY(RehearsalMark)
    DEFCLTENTRY(RepeatSign)
    DEFCLTENTRY(Rest)
    DEFCLTENTRY(Slur)
    
    DEFCLTENTRY(Stem)
    DEFCLTENTRY(SystemSeparation)
    DEFCLTENTRY(TagActivate)
    DEFCLTENTRY(TagInactivate)
    DEFCLTENTRY(TempoMarking)
    DEFCLTENTRY(TempoMarkNonstandard)
    DEFCLTENTRY(Text)
    DEFCLTENTRY(Tie)
    DEFCLTENTRY(TimeSignature)
    DEFCLTENTRY(TimeSigNonstandard)
    
    DEFCLTENTRY(Tremolo)
    DEFCLTENTRY(Tuplet)

    { RIFFIO_FOURCC_IARL , -1 },
    { RIFFIO_FOURCC_IART , -1 },
    { RIFFIO_FOURCC_ICMS , -1 },
    { RIFFIO_FOURCC_ICMT , -1 },
    { RIFFIO_FOURCC_ICOP , -1 },
    { RIFFIO_FOURCC_ICRD , -1 },
    { RIFFIO_FOURCC_ICRP , -1 },
    { RIFFIO_FOURCC_IDIM , -1 },
    { RIFFIO_FOURCC_IDPI , -1 },
    { RIFFIO_FOURCC_IENG , -1 },
    { RIFFIO_FOURCC_IGNR , -1 },
    { RIFFIO_FOURCC_IKEY , -1 },
    { RIFFIO_FOURCC_ILGT , -1 },
    { RIFFIO_FOURCC_IMED , -1 },
    { RIFFIO_FOURCC_INAM , -1 },
    { RIFFIO_FOURCC_IPLT , -1 },
    { RIFFIO_FOURCC_IPRD , -1 },
    { RIFFIO_FOURCC_ISBJ , -1 },
    { RIFFIO_FOURCC_ISFT , -1 },
    { RIFFIO_FOURCC_ISHP , -1 },
    { RIFFIO_FOURCC_ISRC , -1 },
    { RIFFIO_FOURCC_ISRF , -1 },

    { RIFFIOMAKEFOURCC(' ', ' ', ' ', ' '), -9999 } /* Sentinel value,
                                                     *  don't process this */
};








