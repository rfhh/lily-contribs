/* 
$Id: niffio.h,v 1.3 1996/06/15 02:53:49 tim Exp $ 
*/

#ifndef _NIFFIOH_
#define _NIFFIOH_

/***************************************************************************/
/*
 * NAME
 * ====
 * niffio - Data types of the NIFFIO API
 *
 * NIFFIO provides data types and functions to manipulate NIFF-specific
 * features of NIFF files (eg. tags, chunk length tables).
 * Because NIFF files are also RIFF files, NIFFIO makes heavy use 
 * of the RIFFIO API for ordinary RIFF features (eg. chunks).
 *
 * NIFF Specification
 * ==================
 * Where is NIFF described?
 * 
 * The NIFF file format is described in the NIFF specification.
 * 
 *    Notation Interchange File Format
 *    URL ftp://blackbox.cartah.washington.edu
 * 
 * NIFF Version
 * ============
 * Which version of NIFF does NIFFIO support?
 *
 * NIFFIO supports NIFF 6b.
 *
 */
/***************************************************************************/

#include <stdio.h>

#include "riffio.h"
#include "niff.h"

/***************************************************************************/
/*
 * NIFFIOChunkLengthTable
 * ======================
 * How are chunk length tables represented?
 * 
 * NIFFIO provides chunk length table routines that operate on
 * a pointer to a NIFFIOChunkLengthTable structure.
 * 
 * A NIFFIOChunkLengthTable is not necessarily stored in memory
 * the same way as a chunk length table is stored in a NIFF file.
 *
 * All of the members of a NIFFIOChunkLengthTable are private.
 */
/***************************************************************************/

typedef RIFFIOFCCTable NIFFIOChunkLengthTable;

/***************************************************************************/
/*
 * NIFFIOTag
 * =========
 * How are tags supported?
 * 
 * A NIFFIOTag holds tag id, size, and file location information about
 * a tag in a NIFF file.
 */    
  
typedef struct 
{
    /* Public - users need to read and set these members */
    BYTE         tagid;       /* type of tag */
    BYTE         tagsizeData; /* size of tag's data
                               * (does not include possible pad byte)
                               */
    
    /* Other Private members - not for NIFFIO users */
/***************************************************************************/
    RIFFIOOffset offsetData;  /* offset of data portion of tag,
                               * relative to beginning of file
                               */
    
    int          isDirty;     /* Boolean. True if a tag's size needs
                               * updating in the file.
                               * For example, a tag created by
                               * NIFFIOTagCreate is dirty until it is 
                               * finialized by NIFFIOTagFinalize
                               */

/***************************************************************************/
 
} NIFFIOTag;
/***************************************************************************/


/***************************************************************************/
/*
 * NIFFIOFile
 * ==========
 * How are NIFF files represented?
 *
 * Operations on NIFF files take pointers to a NIFFIOFile structure.
 * 
 * Represents a NIFF file, including I/O operations and its chunk
 * length table.
 * 
 * There are no public members of a NIFFIOFile.
 */
/***************************************************************************/
typedef struct
{
    RIFFIOFile             rf;
    NIFFIOChunkLengthTable *pclt;

} NIFFIOFile;
/***************************************************************************/
/*
 * Because NIFF files are also RIFF files,
 * all of the I/O operations defined for RIFFIO files have 
 * RIFFIO equivalents. (see *<NIFFIO Inherit>*)  
 */
/***************************************************************************/


extern NIFFIOFile *NIFFIOFileNew(void);
extern NIFFIOFile *NIFFIOFileNewSTDC(FILE *fp);

extern void        NIFFIOFileDelete(NIFFIOFile *pnf);
extern RIFFIOSuccess
NIFFIOFileInit(NIFFIOFile *pnf,
               void *userFilep,
               RIFFIOReader, RIFFIOWriter, RIFFIOSeeker, RIFFIOTeller);

extern RIFFIOSuccess
NIFFIOSeekChunkTags(NIFFIOFile *pnf, RIFFIOChunk *pchunk);

extern NIFFIOChunkLengthTable *
NIFFIOFileGetCLT(NIFFIOFile *pnf);

extern RIFFIOSuccess
NIFFIOFileReadCLT(NIFFIOFile *pnf, RIFFIOChunk *pchunkCLT);
 
extern RIFFIOSuccess
NIFFIOFileAdoptCLT(NIFFIOFile *pnf, NIFFIOChunkLengthTable *pclt);


/****************
 * TAGS
 ****************/
extern RIFFIOSuccess
NIFFIOTagCreate(NIFFIOFile *pnf,  NIFFIOTag *tagp);

extern RIFFIOSuccess
NIFFIOTagFinalize(NIFFIOFile *pnf,  NIFFIOTag *tagp);

extern RIFFIOSuccess
NIFFIOTagAscend(NIFFIOFile *pnf, NIFFIOTag *tagp);

extern RIFFIOSuccess
NIFFIOTagDescend(NIFFIOFile *pnf,  NIFFIOTag *tagp);

/*******************
 * Basic Types I/O
 *******************/


extern RIFFIOSuccess 
NIFFIOReadBYTE(NIFFIOFile *pnf, BYTE *pbyte);

extern RIFFIOSuccess 
NIFFIOReadCHAR(NIFFIOFile *pnf, CHAR *pchar);

extern RIFFIOSuccess 
NIFFIOReadSIGNEDBYTE(NIFFIOFile *pnf, SIGNEDBYTE *sbp);

extern RIFFIOSuccess
NIFFIOReadSHORT(NIFFIOFile *pnf, SHORT *ssp);

extern RIFFIOSuccess
NIFFIOReadLONG(NIFFIOFile *pnf, LONG *slp);

extern RIFFIOSuccess
NIFFIOReadRATIONAL(NIFFIOFile *pnf, RATIONAL *prat);

extern RIFFIOSuccess
NIFFIOReadSTROFFSET(NIFFIOFile *pnf, STROFFSET *pstroff);

extern RIFFIOSuccess
NIFFIOReadFONTIDX(NIFFIOFile *pnf, FONTIDX *pfidx);

extern RIFFIOSuccess
NIFFIOWriteBYTE(NIFFIOFile *pnf, BYTE byte);

extern RIFFIOSuccess
NIFFIOWriteSIGNEDBYTE(NIFFIOFile *pnf, SIGNEDBYTE sb);

extern RIFFIOSuccess
NIFFIOWriteSHORT(NIFFIOFile *pnf, SHORT ss);

extern RIFFIOSuccess
NIFFIOWriteLONG(NIFFIOFile *pnf, LONG sl);

extern RIFFIOSuccess
NIFFIOWriteRATIONAL(NIFFIOFile *pnf, RATIONAL rat);

extern RIFFIOSuccess
NIFFIOWriteSTROFFSET(NIFFIOFile *pnf, STROFFSET stroff);

extern RIFFIOSuccess
NIFFIOWriteFONTIDX(NIFFIOFile *pnf, FONTIDX fidx);


/************************ 
 * Chunk Length Tables 
 ************************/

extern NIFFIOChunkLengthTable *
NIFFIOCLTNew(void);

extern void
NIFFIOCLTDelete(NIFFIOChunkLengthTable *pclt);

extern int
NIFFIOCompareFOURCC(FOURCC fccA, FOURCC fccB);

extern RIFFIOSuccess
NIFFIOCLTMakeEntry(NIFFIOChunkLengthTable *pclt, 
                   niffChklentabEntry cltEntry);

extern RIFFIOSuccess
NIFFIOCLTLookup(NIFFIOChunkLengthTable *pclt, niffChklentabEntry *cltEntryp);

extern RIFFIOSuccess
NIFFIOCLTMakeDefaultEntries(NIFFIOChunkLengthTable *pclt);

extern unsigned
NIFFIOCLTCount(NIFFIOChunkLengthTable *pclt);

#ifndef NDEBUG
extern RIFFIOSuccess
NIFFIOCLTVerifyDefaults(NIFFIOChunkLengthTable *pclt);
#endif

/****************
 * String Tables
 ***************/

/* cg 2/17/96: added typedef for NIFFIOStbl.  */
/***************************************************************************/
/*
 * NIFFIOStbl
 * ==========
 * How is the NIFF String Table (stbl) represented?
 *
 * Operations on the String Table take a pointer to a NIFFIOStbl structure.
 * 
 * This structure represents a single string item, including a pointer to its
 * value and its offset into the NIFF file's string table. It is used by the
 * NIFF writing program.
 * 
 * When writing a NIFF file, the user supplies an array of type
 * NIFFIOStbl, with the pointers to each of the strings placed into
 * the <*str> fields.  The user then calls NIFFIOchunkStringTable with
 * this array and the number of strings supplied.
 * NIFFIOStoreStbl() puts the strings into string table format,
 * calculating the offsets and storing each offset into the matching
 * position for its string, and then stores the whole string table
 * into the file.  The string offsets can be thus be obtained and used
 * by the writing program when writing out chunks which refer to the
 * string later in the file.
 * 
 */

typedef struct 
{
    STROFFSET offset;  /* Offset of string relative to chunk data start */
    char      *str;    
} NIFFIOStbl;

/***************************************************************************/

extern RIFFIOSuccess
NIFFIOStblWrite(NIFFIOFile *pnf, 
                RIFFIOChunk *pchunkSTBL, 
                NIFFIOStbl *pstbl);

/***************************************************************************/
/*
 * NIFFIOParser
 * ============
 * How does NIFFIO know how to parse a NIFF file?
 *
 * NIFFIO uses a NIFFIOParser structure to store parsing information
 * for NIFF files.  A NIFFIOParser remembers user-supplied callbacks 
 * to process selected NIFF chunks and tags.
 * 
 * See Also:
 * - *<NIFFIO Parse>*
 * - *<NIFFIO Register>*
 *
 * A user may choose to trace the operation of a NIFFIOParser.
 * The default tracer uses the Standard C Library to print to <stderr>.
 * Alternatively, the user may supply their own routine to handle 
 * tracing output. 
 *
 * NIFFIOParserTracer
 * ------------------
 * Callback to handle parser tracing output.
 */
typedef void (*NIFFIOParserTracer)(const char *strParser,
                                   const unsigned nLevel, 
                                   const char *strFormat, 
                                   ...);
/*
 * PARAMETERS:
 * <strParser> is a string that identifies a parser in case there
 * is more than one parser in a program.
 *
 * <nLevel> is the nesting depth of the current chunk or tag.
 * It is the number of chunks above the current object. For example, the
 * the NIFF form chunk has an <nLevel> == 0.
 *
 * <strFormat> is a vprintf compatible format string that reflects the 
 * following variable argument list to the NIFFIOParserTracer.
 * 
 */
/***************************************************************************/

#define NIFFIO_FOURCC_WILDCARD MAKEFOURCC('*','*','*','*')

typedef struct NIFFIOPParser NIFFIOParser;

extern NIFFIOParser *
NIFFIOParserNew(void);

extern void
NIFFIOParserDelete(NIFFIOParser *pparser);

extern void
NIFFIOParserSetTracing(NIFFIOParser *pparser, int isTracing);

extern int
NIFFIOParserGetTracing(NIFFIOParser *pparser);

extern NIFFIOParserTracer
NIFFIOParserSetTracer(NIFFIOParser *pparser, NIFFIOParserTracer tracer);

/***************************************************************************/

/*
 * NIFFIOUserContext
 * -----------------
 * User-defined parser state information provided by parent chunk callbacks.
 */
typedef void *NIFFIOUserContext;
/*
 * While parsing a NIFF file, users may keep track of their own
 * parsing information by supplying pointers to custom, user-defined data 
 * structures.  In turn, the NIFFIO parser will pass chunk and tag callbacks
 * the "context" of their enclosing (parent) chunk. 
 * 
 * NIFFIOUserContexts are solely for the user's use.
 * NIFFIO will never dereference any NIFFIOUserContext.
 * User contexts may be safely ignored by any user who does not wish to
 * use them.  It is safe to assign a null pointer to a NIFFIOUserContext.
 */

/*
 * In addition to the NIFFIOUserContext of its parent chunk, a 
 * chunk callback routine also gets information about the chunk it
 * is processing through a NIFFIOChunkContext structure.
 *  
 * NIFFIOChunkContext
 * ------------------
 * Parser state information provided to chunk callbacks.
 *
 */
 
typedef struct
{
  unsigned          nLevel;        /* chunk depth */
  NIFFIOFile        *pnf;          /* NIFF file that contains the chunk   */
  RIFFIOChunk       *pchunk;       /* Chunk information from file         */
  NIFFIOUserContext ctxParent;     /* Parent chunk user context           */
  NIFFIOUserContext ctxMe;         /* Child user context, to be filled in
                                    * by chunk start callback
                                    */
} NIFFIOChunkContext;

/*
 * NIFFIOTagContext
 * ----------------
 * Parser state information provided to tag callbacks.
 */
typedef struct
{
  unsigned          nLevel;        /* tag depth */
  NIFFIOFile        *pnf;          /* NIFF file that contains the chunk  */
  NIFFIOTag         *ptag;         /* Tag information from file          */
  RIFFIOChunk       *pchunkParent; /* Parent chunk information from file */
  NIFFIOUserContext ctxParent;     /* Parent chunk user context          */
} NIFFIOTagContext;

/***************************************************************************/

extern RIFFIOSuccess
NIFFIOParseFile(NIFFIOParser *pparser, 
                NIFFIOFile *pnf, 
                NIFFIOUserContext userctxIn,
                NIFFIOUserContext *puserctxOut);


/***************************************************************************/
/*
 * Parser Callbacks
 * ================
 * Here are the types of functions that the user needs to define to
 * use as callbacks.
 *
 * NIFFIOChunkCallback
 * -------------------
 * Process a chunk encountered by the parser.
 */ 
typedef RIFFIOSuccess (*NIFFIOChunkCallback)(NIFFIOChunkContext *pchunkctx);

/* 
 * NIFFIOTagCallback
 * -----------------
 * Process a tag encountered by the parser.
 */
typedef RIFFIOSuccess (*NIFFIOTagCallback)(NIFFIOTagContext *ptagctx); 

/***************************************************************************/


extern RIFFIOSuccess 
NIFFIORegisterForm(NIFFIOParser *pparser,
                   NIFFIOChunkCallback cbStart, 
                   NIFFIOChunkCallback cbEnd);

extern RIFFIOSuccess
NIFFIORegisterList(NIFFIOParser *pparser,
                   FOURCC fccType, 
                   NIFFIOChunkCallback cbStart, 
                   NIFFIOChunkCallback cbEnd);

extern RIFFIOSuccess
NIFFIORegisterAtomicChunk(NIFFIOParser *pparser,
                          FOURCC fccId, 
                          NIFFIOChunkCallback cb);
 
extern RIFFIOSuccess
NIFFIORegisterUserChunk(NIFFIOParser *pparser,
                        DWORD userid, 
                        NIFFIOChunkCallback cb);

extern RIFFIOSuccess
NIFFIORegisterUserTag(NIFFIOParser *pparser,
                      DWORD userid, 
                      NIFFIOTagCallback cb);

extern RIFFIOSuccess
NIFFIORegisterDefaultList(NIFFIOParser *pparser,
                          NIFFIOChunkCallback cbStart,
                          NIFFIOChunkCallback cbEnd);

extern RIFFIOSuccess
NIFFIORegisterDefaultTaggedChunk(NIFFIOParser *pparser,
                                 NIFFIOChunkCallback cbStart,
                                 NIFFIOChunkCallback cbEnd);

extern RIFFIOSuccess
NIFFIORegisterDefaultAtomicChunk(NIFFIOParser *pparser,
                                 NIFFIOChunkCallback cb);

extern RIFFIOSuccess
NIFFIORegisterDefaultTag(NIFFIOParser *pparser,
                         NIFFIOTagCallback cbTag);

/*
 * Registration functions for Lists
 */
#define REG_LIST(nifftype)                                      \
extern RIFFIOSuccess                                            \
NIFFIORegisterList##nifftype(NIFFIOParser *pparser,             \
                             NIFFIOChunkCallback cbStart,       \
                             NIFFIOChunkCallback cbEnd);

REG_LIST(SetupSection)
REG_LIST(Parts)
REG_LIST(RiffInfo)
REG_LIST(Groupings)
REG_LIST(FontDescs)
REG_LIST(CustomGraphics)

REG_LIST(DataSection)
REG_LIST(Page)
REG_LIST(System)
REG_LIST(Staff)
                                                         
#undef REG_LIST

/*
 * Registration functions for atomic, raw chunks
 */
#define REG_ATOMIC(nifftype)                            \
extern RIFFIOSuccess                                    \
NIFFIORegisterChunk##nifftype(                          \
                              NIFFIOParser *pparser,    \
                              NIFFIOChunkCallback cb);
REG_ATOMIC(ChnkLenTable)
REG_ATOMIC(EpsGraphic)
REG_ATOMIC(PsType1Font)
REG_ATOMIC(PsType3Font)
REG_ATOMIC(StringTable)

#undef REG_ATOMIC

/*
 * Registration functions for tagable, raw Chunks
 */

#define REG_TAGRAW(nifftype)                                    \
extern RIFFIOSuccess                                            \
NIFFIORegisterChunk##nifftype(                                  \
                              NIFFIOParser *pparser,            \
                              NIFFIOChunkCallback cbStart,      \
                              NIFFIOChunkCallback cbEnd);

REG_TAGRAW(PageHeader)
REG_TAGRAW(StaffHeader)
REG_TAGRAW(SystemHeader)

REG_TAGRAW(AugDot)
REG_TAGRAW(Glissando)
REG_TAGRAW(Portamento)
REG_TAGRAW(Slur)
REG_TAGRAW(Stem)
REG_TAGRAW(TagActivate)
REG_TAGRAW(TagInactivate)
REG_TAGRAW(Tie)
REG_TAGRAW(Tuplet)

#undef REG_TAGRAW

/*
 * Registration functions for tagable, cooked Chunks
 */
#define REG_TAGCOOK(nifftype)                                           \
extern RIFFIOSuccess                                                    \
NIFFIORegisterChunk##nifftype(                                          \
        NIFFIOParser *pparser,                                          \
    RIFFIOSuccess (*cbStart)(NIFFIOChunkContext *, niff##nifftype *),   \
    RIFFIOSuccess (*cbEnd)(NIFFIOChunkContext *, niff##nifftype *)); 

REG_TAGCOOK(DefaultValues)
REG_TAGCOOK(FontDescription)
REG_TAGCOOK(NiffInfo)
REG_TAGCOOK(Part)
REG_TAGCOOK(StaffGrouping)
REG_TAGCOOK(TimeSlice)

REG_TAGCOOK(Accidental)
REG_TAGCOOK(AltEndingGraphic)
REG_TAGCOOK(Arpeggio)
REG_TAGCOOK(Articulation)
REG_TAGCOOK(Barline)
REG_TAGCOOK(Beam)
REG_TAGCOOK(ChordSymbol)
REG_TAGCOOK(Clef)
REG_TAGCOOK(CustomGraphicChk)
REG_TAGCOOK(Dynamic)
REG_TAGCOOK(FiguredBass)
REG_TAGCOOK(Fingering)
REG_TAGCOOK(GuitarGrid)
REG_TAGCOOK(GuitarTabNum)
REG_TAGCOOK(Hairpin)
REG_TAGCOOK(HarpPedal)
REG_TAGCOOK(KeySignature)
REG_TAGCOOK(KeySignNonstandard)
REG_TAGCOOK(Line)
REG_TAGCOOK(Lyric)
REG_TAGCOOK(MeasureNumbering)
REG_TAGCOOK(MidiDataStream)
REG_TAGCOOK(FontSymbol)
REG_TAGCOOK(Notehead)
REG_TAGCOOK(OctaveSign)
REG_TAGCOOK(Ornament)
REG_TAGCOOK(Parenthesis)
REG_TAGCOOK(PedalPiano)
REG_TAGCOOK(PedalOrgan)
REG_TAGCOOK(RehearsalMark)
REG_TAGCOOK(RepeatSign)
REG_TAGCOOK(Rest)
REG_TAGCOOK(SystemSeparation)
REG_TAGCOOK(TempoMarking)
REG_TAGCOOK(TempoMarkNonstandard)
REG_TAGCOOK(Text)
REG_TAGCOOK(TimeSignature)
REG_TAGCOOK(TimeSigNonstandard)
REG_TAGCOOK(Tremolo)

#undef REG_TAGCOOK

/*
 * Registration functions for raw Tags
 */
#define REG_TAGRAW(nifftype)                            \
extern RIFFIOSuccess                                    \
NIFFIORegisterTag##nifftype(                            \
                            NIFFIOParser *pparser,      \
                            FOURCC,                     \
                            NIFFIOTagCallback cb);

REG_TAGRAW(EndOfSystem)
REG_TAGRAW(GuitarTabTag)
REG_TAGRAW(Invisible)
REG_TAGRAW(LargeSize)
REG_TAGRAW(MultiNodeEndOfSyst)
REG_TAGRAW(MultiNodeStartOfSyst)
REG_TAGRAW(Silent)
REG_TAGRAW(SlashedStem)
REG_TAGRAW(SmallSize)
REG_TAGRAW(SpacingByPart)
REG_TAGRAW(SplitStem)
REG_TAGRAW(StaffName)

#undef REG_TAGRAW


/*
 * Registration functions for cooked Tags
 */
#define REG_TAGCOOK(nifftype)                   \
extern RIFFIOSuccess                            \
NIFFIORegisterTag##nifftype(                    \
              NIFFIOParser *pparser,            \
              FOURCC,                           \
              RIFFIOSuccess (*cb)(NIFFIOTagContext *, niff##nifftype *));
   

REG_TAGCOOK(AbsPlacement)
REG_TAGCOOK(AltEnding)
REG_TAGCOOK(AnchorOverride)
REG_TAGCOOK(ArticDirection)
REG_TAGCOOK(BezierIncoming)
REG_TAGCOOK(BezierOutgoing)
REG_TAGCOOK(ChordSymbolsOffset)
REG_TAGCOOK(CustomFontChar)
REG_TAGCOOK(CustomGraphicTag)
REG_TAGCOOK(FannedBeam)
REG_TAGCOOK(FigBassOffset)
REG_TAGCOOK(FontID)
REG_TAGCOOK(GraceNote)
REG_TAGCOOK(GuitarGridOffset)
REG_TAGCOOK(Height)
REG_TAGCOOK(ID)
REG_TAGCOOK(LineQuality)
REG_TAGCOOK(LogicalPlacement)
REG_TAGCOOK(LyricVerseOffset)
REG_TAGCOOK(MidiPerformance)
REG_TAGCOOK(NumberOfFlags)
REG_TAGCOOK(NumberOfNodes)
REG_TAGCOOK(NumStaffLines)
REG_TAGCOOK(Ossia)
REG_TAGCOOK(PartDescOverride)
REG_TAGCOOK(PartID)
REG_TAGCOOK(RefPtOverride)
REG_TAGCOOK(RehearsalOffset)
REG_TAGCOOK(RestNumeral)
REG_TAGCOOK(StaffStep)
REG_TAGCOOK(Thickness)
REG_TAGCOOK(TieDirection)
REG_TAGCOOK(TupletDesc)
REG_TAGCOOK(NumberStyle)
REG_TAGCOOK(VoiceID)
REG_TAGCOOK(Width)

#undef REG_TAGCOOK


/*******************************
 * Inherit functions from RIFFIO
 *******************************/

extern long NIFFIORead(NIFFIOFile *pnf, void *bufferp, long n);
extern long NIFFIOWrite(NIFFIOFile *pnf, void *bufferp, long n);
extern RIFFIOSuccess
NIFFIOSeek(NIFFIOFile *pnf, 
                   RIFFIOOffset offset,
                   RIFFIOSeekOrigin origin);
extern long NIFFIOTell(NIFFIOFile *pnf);


extern RIFFIOSuccess 
NIFFIOChunkAscend(NIFFIOFile *pnf, RIFFIOChunk *pchunk);
extern RIFFIOSuccess 
NIFFIOChunkDescend(NIFFIOFile *pnf, RIFFIOChunk *pchunk);
extern RIFFIOSuccess 
NIFFIOChunkCreate(NIFFIOFile *pnf, RIFFIOChunk *pchunk);
extern RIFFIOSuccess 
NIFFIOChunkFinalize(NIFFIOFile *pnf, RIFFIOChunk *pchunk);

extern RIFFIOSuccess
NIFFIOChunkSeekData(NIFFIOFile *pnf, const RIFFIOChunk *pchunk);

extern int 
NIFFIOChunkEnd(NIFFIOFile *pnf, RIFFIOChunk *pchunk);

extern int
NIFFIOChunkDataEnd(NIFFIOFile *pnf, RIFFIOChunk *pchunk);

extern RIFFIOFormType NIFFIOFileGetFormType(NIFFIOFile *pnf);

extern RIFFIOSuccess NIFFIOWrite8(NIFFIOFile *pnf, unsigned char uc);
extern RIFFIOSuccess NIFFIOWrite16(NIFFIOFile *pnf, unsigned short us);
extern RIFFIOSuccess NIFFIOWrite32(NIFFIOFile *pnf, unsigned long ul);
extern RIFFIOSuccess NIFFIORead8(NIFFIOFile *pnf, unsigned char *ucp);
extern RIFFIOSuccess NIFFIORead16(NIFFIOFile *pnf, unsigned short *usp);
extern RIFFIOSuccess NIFFIORead32(NIFFIOFile *pnf, unsigned long *ulp);

extern RIFFIOSuccess NIFFIOWriteFOURCC(NIFFIOFile *pnf, FOURCC fcc);
extern RIFFIOSuccess NIFFIOReadFOURCC(NIFFIOFile *pnf, FOURCC *fccp);


/*****************
 * Niff Types IO
 *****************/
#define NIFFIO_DECLARE(nifftype)                                \
extern RIFFIOSuccess                                            \
NIFFIOReadniff##nifftype(NIFFIOFile *pnf, niff##nifftype *p);   \
extern RIFFIOSuccess                                            \
NIFFIOWriteniff##nifftype(NIFFIOFile *pnf, niff##nifftype *p);

NIFFIO_DECLARE(ChklentabEntry)
NIFFIO_DECLARE(DefaultValues)
NIFFIO_DECLARE(FontDescription)
NIFFIO_DECLARE(NiffInfo)
NIFFIO_DECLARE(Part)
NIFFIO_DECLARE(StaffGrouping)
NIFFIO_DECLARE(TimeSlice)

NIFFIO_DECLARE(Accidental)
NIFFIO_DECLARE(AltEndingGraphic)
NIFFIO_DECLARE(Arpeggio)
NIFFIO_DECLARE(Articulation)
NIFFIO_DECLARE(Barline)
NIFFIO_DECLARE(Beam)
NIFFIO_DECLARE(ChordSymbol)
NIFFIO_DECLARE(Clef)
NIFFIO_DECLARE(CustomGraphicChk)
NIFFIO_DECLARE(Dynamic)
NIFFIO_DECLARE(FiguredBass)
NIFFIO_DECLARE(Fingering)
NIFFIO_DECLARE(GuitarGrid)
NIFFIO_DECLARE(GuitarTabNum)
NIFFIO_DECLARE(Hairpin)
NIFFIO_DECLARE(HarpPedal)
NIFFIO_DECLARE(KeySignature)
NIFFIO_DECLARE(KeySignNonstandard)
NIFFIO_DECLARE(Line)
NIFFIO_DECLARE(Lyric)
NIFFIO_DECLARE(MeasureNumbering)
NIFFIO_DECLARE(MidiDataStream)
NIFFIO_DECLARE(FontSymbol)
NIFFIO_DECLARE(Notehead)
NIFFIO_DECLARE(OctaveSign)
NIFFIO_DECLARE(Ornament)
NIFFIO_DECLARE(Parenthesis)
NIFFIO_DECLARE(PedalPiano)
NIFFIO_DECLARE(PedalOrgan)
NIFFIO_DECLARE(RehearsalMark)
NIFFIO_DECLARE(RepeatSign)
NIFFIO_DECLARE(Rest)
NIFFIO_DECLARE(SystemSeparation)
NIFFIO_DECLARE(TempoMarking)
NIFFIO_DECLARE(TempoMarkNonstandard)
NIFFIO_DECLARE(Text)
NIFFIO_DECLARE(TimeSignature)
NIFFIO_DECLARE(TimeSigNonstandard)
NIFFIO_DECLARE(Tremolo)

NIFFIO_DECLARE(AbsPlacement)
NIFFIO_DECLARE(AltEnding)
NIFFIO_DECLARE(AnchorOverride)
NIFFIO_DECLARE(ArticDirection)
NIFFIO_DECLARE(BezierIncoming)
NIFFIO_DECLARE(BezierOutgoing)
NIFFIO_DECLARE(ChordSymbolsOffset)
NIFFIO_DECLARE(CustomFontChar)
NIFFIO_DECLARE(CustomGraphicTag)
NIFFIO_DECLARE(FannedBeam)
NIFFIO_DECLARE(FigBassOffset)
NIFFIO_DECLARE(FontID)
NIFFIO_DECLARE(GraceNote)
NIFFIO_DECLARE(GuitarGridOffset)
NIFFIO_DECLARE(Height)
NIFFIO_DECLARE(ID)
NIFFIO_DECLARE(LineQuality)
NIFFIO_DECLARE(LogicalPlacement)
NIFFIO_DECLARE(LyricVerseOffset)
NIFFIO_DECLARE(MidiPerformance)
NIFFIO_DECLARE(NumberOfFlags)
NIFFIO_DECLARE(NumberOfNodes)
NIFFIO_DECLARE(NumStaffLines)
NIFFIO_DECLARE(Ossia)
NIFFIO_DECLARE(PartDescOverride)
NIFFIO_DECLARE(PartID)
NIFFIO_DECLARE(RefPtOverride)
NIFFIO_DECLARE(RehearsalOffset)
NIFFIO_DECLARE(RestNumeral)
NIFFIO_DECLARE(StaffStep)
NIFFIO_DECLARE(Thickness)
NIFFIO_DECLARE(TieDirection)
NIFFIO_DECLARE(TupletDesc)
NIFFIO_DECLARE(NumberStyle)
NIFFIO_DECLARE(VoiceID)
NIFFIO_DECLARE(Width)


#undef NIFFIO_DECLARE

/*****************
 * NIFF Storage
 *****************/

/*
 * NIFFIOStorage
 * =============
 */
typedef struct NIFFIOPStore NIFFIOStorage;

extern NIFFIOStorage *
NIFFIOStorageNew(void);

extern RIFFIOSuccess 
NIFFIOStorageInit(NIFFIOStorage *pstore, NIFFIOFile *pnf);

extern NIFFIOStorage *
NIFFIOStorageNewSTDC(FILE *fp);

extern void NIFFIOStorageDelete(NIFFIOStorage *pstore);

extern void           NIFFIOStorageSetCurrent(NIFFIOStorage *pstore);
extern NIFFIOStorage *NIFFIOStorageGetCurrent(void);

extern NIFFIOFile *NIFFIOStorageGetFile(void);

extern int NIFFIOStorageIsTagPending(void);
extern int NIFFIOStorageIsChunkPending(void);
extern int NIFFIOStorageIsListPending(void);

extern NIFFIOTag *NIFFIOStoragePendingTag(void);
extern RIFFIOChunk *NIFFIOStoragePendingChunk(void);
extern RIFFIOChunk *NIFFIOStoragePendingList(void);

extern RIFFIOSuccess NIFFIOStorageTagEnd(void);
extern RIFFIOSuccess NIFFIOStorageChunkEnd(void);
extern RIFFIOSuccess NIFFIOStorageListEnd(void);

extern RIFFIOSuccess NIFFIOStorageTagStart(NIFFIOTag *ptag);
extern RIFFIOSuccess NIFFIOStorageChunkStart(RIFFIOChunk *pchunk);
extern RIFFIOSuccess NIFFIOStorageListStart(RIFFIOChunk *pchunk);

/* Special objects */
extern int
NIFFIOStoreStbl(NIFFIOStbl *pstbl, int nEntries);

extern RIFFIOSuccess
NIFFIOStoreCLT(NIFFIOChunkLengthTable *pclt);

extern RIFFIOSuccess
NIFFIOStoreDefaultCLT(void);

/***********************
 * names 
 ***********************/

const char *
NIFFIONameListType(RIFFIOFOURCC fccType);

const char *
NIFFIONameChunkId(RIFFIOFOURCC fccId);

const char *
NIFFIONameTagId(BYTE tagid); 

const char *
NIFFIOSymbolTS(BYTE ts);

const char *
NIFFIOSymbolBAREXT(BYTE be);

const char *
NIFFIOSymbolBARTYPE(BYTE bt);

const char *
NIFFIOSymbolCLEFSHAPE(BYTE cs);

const char *
NIFFIOSymbolCLEFOCT(BYTE co);

const char *
NIFFIOSymbolNOTESHAPE(BYTE ns);
 
const char *
NIFFIOSymbolREST(BYTE rs);

const char *
NIFFIOSymbolLOGPLACEH(BYTE lh);

const char *
NIFFIOSymbolLOGPLACEV(BYTE lv);

const char *
NIFFIOSymbolLOGPLACEPROX(BYTE lp);

/*
 * The following was generated automatically 
 */

/***************************
 * Form
 ****************************/
extern RIFFIOSuccess
NIFFIOStartNiff(void);
extern RIFFIOSuccess
NIFFIOEndNiff(void);

/***************************
 * Lists
 ****************************/
extern RIFFIOSuccess
NIFFIOStartCustomGraphics(void);
extern RIFFIOSuccess
NIFFIOEndCustomGraphics(void);

extern RIFFIOSuccess
NIFFIOStartDataSection(void);
extern RIFFIOSuccess
NIFFIOEndDataSection(void);

extern RIFFIOSuccess
NIFFIOStartFontDescs(void);
extern RIFFIOSuccess
NIFFIOEndFontDescs(void);

extern RIFFIOSuccess
NIFFIOStartGroupings(void);
extern RIFFIOSuccess
NIFFIOEndGroupings(void);

extern RIFFIOSuccess
NIFFIOStartPage(void);
extern RIFFIOSuccess
NIFFIOEndPage(void);

extern RIFFIOSuccess
NIFFIOStartParts(void);
extern RIFFIOSuccess
NIFFIOEndParts(void);

extern RIFFIOSuccess
NIFFIOStartRiffInfo(void);
extern RIFFIOSuccess
NIFFIOEndRiffInfo(void);

extern RIFFIOSuccess
NIFFIOStartSetupSection(void);
extern RIFFIOSuccess
NIFFIOEndSetupSection(void);

extern RIFFIOSuccess
NIFFIOStartStaff(void);
extern RIFFIOSuccess
NIFFIOEndStaff(void);

extern RIFFIOSuccess
NIFFIOStartSystem(void);
extern RIFFIOSuccess
NIFFIOEndSystem(void);

/***************************
 * Raw chunks
 ****************************/
extern RIFFIOSuccess
NIFFIOchunkAugDot(void);

extern RIFFIOSuccess
NIFFIOchunkChnkLenTable(void);

extern RIFFIOSuccess
NIFFIOchunkEpsGraphic(void);

extern RIFFIOSuccess
NIFFIOchunkGlissando(void);

extern RIFFIOSuccess
NIFFIOchunkPageHeader(void);

extern RIFFIOSuccess
NIFFIOchunkPortamento(void);

extern RIFFIOSuccess
NIFFIOchunkPsType1Font(void);

extern RIFFIOSuccess
NIFFIOchunkPsType3Font(void);

extern RIFFIOSuccess
NIFFIOchunkSlur(void);

extern RIFFIOSuccess
NIFFIOchunkStaffHeader(void);

extern RIFFIOSuccess
NIFFIOchunkStem(void);

extern RIFFIOSuccess
NIFFIOchunkStringTable(void);

extern RIFFIOSuccess
NIFFIOchunkSystemHeader(void);

extern RIFFIOSuccess
NIFFIOchunkTagActivate(void);

extern RIFFIOSuccess
NIFFIOchunkTagInactivate(void);

extern RIFFIOSuccess
NIFFIOchunkTie(void);

extern RIFFIOSuccess
NIFFIOchunkTuplet(void);

/***************************
 * Cooked chunks
 ****************************/
extern
RIFFIOSuccess
NIFFIOchunkAccidental(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkAltEndingGraphic(
    BYTE bracketShape  ,
    STROFFSET textString);

extern
RIFFIOSuccess
NIFFIOchunkArpeggio(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkArticulation(
    SHORT shape);

extern
RIFFIOSuccess
NIFFIOchunkBarline(
    BYTE type  ,
    BYTE extendsTo  ,
    SHORT numberOfStaves);

extern
RIFFIOSuccess
NIFFIOchunkBeam(
    BYTE beamPartsToLeft  ,
    BYTE beamPartsToRight);

extern
RIFFIOSuccess
NIFFIOchunkChordSymbol(
    STROFFSET textDescription);

extern
RIFFIOSuccess
NIFFIOchunkClef(
    BYTE shape  ,
    SIGNEDBYTE staffStep  ,
    BYTE octaveNumber);

extern
RIFFIOSuccess
NIFFIOchunkCustomGraphicChk(
    SHORT value);

extern
RIFFIOSuccess
NIFFIOchunkDefaultValues(
    FONTIDX musicFont  ,
    FONTIDX partNameFont  ,
    FONTIDX lyricFont  ,
    FONTIDX chordSymbolFont  ,
    FONTIDX measureNumberFont  ,
    FONTIDX rehearsalMarkFont  ,
    BYTE tupletGroupingSymbol  ,
    BYTE tupletNumberStyle);

extern
RIFFIOSuccess
NIFFIOchunkDynamic(
    BYTE code);

extern
RIFFIOSuccess
NIFFIOchunkFiguredBass(
    STROFFSET textDescription);

extern
RIFFIOSuccess
NIFFIOchunkFingering(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkFontDescription(
    STROFFSET fontNamePtr  ,
    SHORT size  ,
    SHORT spaceHeight  ,
    SHORT where  ,
    BYTE style);

extern
RIFFIOSuccess
NIFFIOchunkFontSymbol(
    FOURCC chunkType  ,
    SHORT spaceHeight  ,
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkGuitarGrid(
    BYTE numberOfFrets  ,
    BYTE numberOfStrings  ,
    STROFFSET textDescription);

extern
RIFFIOSuccess
NIFFIOchunkGuitarTabNum(
    BYTE number  ,
    SIGNEDBYTE staffStep);

extern
RIFFIOSuccess
NIFFIOchunkHairpin(
    BYTE direction);

extern
RIFFIOSuccess
NIFFIOchunkHarpPedal(
    STROFFSET pedalPositions);

extern
RIFFIOSuccess
NIFFIOchunkKeySignNonstandard(
    BYTE numberOfChunks);

extern
RIFFIOSuccess
NIFFIOchunkKeySignature(
    SIGNEDBYTE standardCode);

extern
RIFFIOSuccess
NIFFIOchunkLine(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkLyric(
    STROFFSET text  ,
    BYTE lyricVerseID);

extern
RIFFIOSuccess
NIFFIOchunkMeasureNumbering(
    BYTE numberWhichMeasures  ,
    BYTE numberFrequency  ,
    SHORT startingNumber  ,
    FONTIDX fontID  ,
    BYTE aboveOrBelow  ,
    BYTE horizontalCentering  ,
    BYTE enclosure);

extern
RIFFIOSuccess
NIFFIOchunkMidiDataStream(
    BYTE startTime);

extern
RIFFIOSuccess
NIFFIOchunkNiffInfo(
    CHAR NIFFVersion [8] ,
    BYTE programType  ,
    BYTE standardUnits  ,
    SHORT absoluteUnits  ,
    SHORT midiClocksPerQuarter);

extern
RIFFIOSuccess
NIFFIOchunkNotehead(
    BYTE shape  ,
    SIGNEDBYTE staffStep  ,
    RATIONAL duration);

extern
RIFFIOSuccess
NIFFIOchunkOctaveSign(
    BYTE numberOfOctaves  ,
    BYTE aboveOrBelow  ,
    BYTE type  ,
    STROFFSET textString);

extern
RIFFIOSuccess
NIFFIOchunkOrnament(
    SHORT shape);

extern
RIFFIOSuccess
NIFFIOchunkParenthesis(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkPart(
    SHORT partID  ,
    STROFFSET name  ,
    STROFFSET abbreviation  ,
    BYTE numberOfStaves  ,
    SIGNEDBYTE midiChannel  ,
    SIGNEDBYTE midiCable  ,
    SIGNEDBYTE transpose);

extern
RIFFIOSuccess
NIFFIOchunkPedalOrgan(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkPedalPiano(
    BYTE shape);

extern
RIFFIOSuccess
NIFFIOchunkRehearsalMark(
    STROFFSET textString  ,
    BYTE enclosure);

extern
RIFFIOSuccess
NIFFIOchunkRepeatSign(
    SHORT graphicalCode  ,
    BYTE logicalCode);

extern
RIFFIOSuccess
NIFFIOchunkRest(
    BYTE shape  ,
    SIGNEDBYTE staffStep  ,
    RATIONAL duration);

extern
RIFFIOSuccess
NIFFIOchunkStaffGrouping(
    BYTE groupingType  ,
    SHORT firstStaff  ,
    SHORT lastStaff);

extern
RIFFIOSuccess
NIFFIOchunkSystemSeparation(
    BYTE where);

extern
RIFFIOSuccess
NIFFIOchunkTempoMarkNonstandard(
    BYTE numberOfChunks);

extern
RIFFIOSuccess
NIFFIOchunkTempoMarking(
    STROFFSET textString  ,
    RATIONAL noteValue  ,
    SHORT beatsPerMinute);

extern
RIFFIOSuccess
NIFFIOchunkText(
    STROFFSET value);

extern
RIFFIOSuccess
NIFFIOchunkTimeSigNonstandard(
    BYTE numberOfChunks);

extern
RIFFIOSuccess
NIFFIOchunkTimeSignature(
    SIGNEDBYTE topNumber  ,
    BYTE bottomNumber);

extern
RIFFIOSuccess
NIFFIOchunkTimeSlice(
    BYTE type  ,
    RATIONAL startTime);

extern
RIFFIOSuccess
NIFFIOchunkTremolo(
    BYTE attachedBeamParts  ,
    BYTE unattachedBeamParts);

/***************************
 * Raw tags
 ****************************/
extern RIFFIOSuccess
NIFFIOtagEndOfSystem(void);

extern RIFFIOSuccess
NIFFIOtagGuitarTabTag(void);

extern RIFFIOSuccess
NIFFIOtagInvisible(void);

extern RIFFIOSuccess
NIFFIOtagLargeSize(void);

extern RIFFIOSuccess
NIFFIOtagMultiNodeEndOfSyst(void);

extern RIFFIOSuccess
NIFFIOtagMultiNodeStartOfSyst(void);

extern RIFFIOSuccess
NIFFIOtagSilent(void);

extern RIFFIOSuccess
NIFFIOtagSlashedStem(void);

extern RIFFIOSuccess
NIFFIOtagSmallSize(void);

extern RIFFIOSuccess
NIFFIOtagSpacingByPart(void);

extern RIFFIOSuccess
NIFFIOtagSplitStem(void);

extern RIFFIOSuccess
NIFFIOtagStaffName(void);

/***************************
 * Cooked composite tags
 ****************************/
extern
RIFFIOSuccess
NIFFIOtagAbsPlacement(
    SHORT horizontal  ,
    SHORT vertical);

extern
RIFFIOSuccess
NIFFIOtagBezierIncoming(
    SHORT horizontal  ,
    SHORT vertical);

extern
RIFFIOSuccess
NIFFIOtagBezierOutgoing(
    SHORT horizontal  ,
    SHORT vertical);

extern
RIFFIOSuccess
NIFFIOtagCustomFontChar(
    FONTIDX fontID  ,
    CHAR characterCode [2]);

extern
RIFFIOSuccess
NIFFIOtagLogicalPlacement(
    BYTE horizontal  ,
    BYTE vertical  ,
    BYTE proximity);

extern
RIFFIOSuccess
NIFFIOtagLyricVerseOffset(
    SHORT lyricLineOffset  ,
    BYTE lyricVerseID);

extern
RIFFIOSuccess
NIFFIOtagMidiPerformance(
    LONG startTime  ,
    LONG duration  ,
    BYTE pitch  ,
    BYTE velocity);

extern
RIFFIOSuccess
NIFFIOtagPartDescOverride(
    SIGNEDBYTE MIDIchannel  ,
    SIGNEDBYTE MIDIcable  ,
    SIGNEDBYTE transpose);

extern
RIFFIOSuccess
NIFFIOtagRefPtOverride(
    BYTE anchor_h  ,
    BYTE dependent_h  ,
    BYTE anchor_v  ,
    BYTE dependent_v);

extern
RIFFIOSuccess
NIFFIOtagTupletDesc(
    RATIONAL transformRatioAB  ,
    RATIONAL transformRatioCD  ,
    BYTE groupingSymbol);

/***************************
 * Cooked primitive tags
 ****************************/
extern
RIFFIOSuccess
NIFFIOtagAltEnding(niffAltEnding AltEnding);

extern
RIFFIOSuccess
NIFFIOtagAnchorOverride(niffAnchorOverride AnchorOverride);

extern
RIFFIOSuccess
NIFFIOtagArticDirection(niffArticDirection ArticDirection);

extern
RIFFIOSuccess
NIFFIOtagChordSymbolsOffset(niffChordSymbolsOffset ChordSymbolsOffset);

extern
RIFFIOSuccess
NIFFIOtagCustomGraphicTag(niffCustomGraphicTag CustomGraphicTag);

extern
RIFFIOSuccess
NIFFIOtagFannedBeam(niffFannedBeam FannedBeam);

extern
RIFFIOSuccess
NIFFIOtagFigBassOffset(niffFigBassOffset FigBassOffset);

extern
RIFFIOSuccess
NIFFIOtagFontID(niffFontID FontID);

extern
RIFFIOSuccess
NIFFIOtagGraceNote(niffGraceNote GraceNote);

extern
RIFFIOSuccess
NIFFIOtagGuitarGridOffset(niffGuitarGridOffset GuitarGridOffset);

extern
RIFFIOSuccess
NIFFIOtagHeight(niffHeight Height);

extern
RIFFIOSuccess
NIFFIOtagID(niffID ID);

extern
RIFFIOSuccess
NIFFIOtagLineQuality(niffLineQuality LineQuality);

extern
RIFFIOSuccess
NIFFIOtagNumStaffLines(niffNumStaffLines NumStaffLines);

extern
RIFFIOSuccess
NIFFIOtagNumberOfFlags(niffNumberOfFlags NumberOfFlags);

extern
RIFFIOSuccess
NIFFIOtagNumberOfNodes(niffNumberOfNodes NumberOfNodes);

extern
RIFFIOSuccess
NIFFIOtagNumberStyle(niffNumberStyle NumberStyle);

extern
RIFFIOSuccess
NIFFIOtagOssia(niffOssia Ossia);

extern
RIFFIOSuccess
NIFFIOtagPartID(niffPartID PartID);

extern
RIFFIOSuccess
NIFFIOtagRehearsalOffset(niffRehearsalOffset RehearsalOffset);

extern
RIFFIOSuccess
NIFFIOtagRestNumeral(niffRestNumeral RestNumeral);

extern
RIFFIOSuccess
NIFFIOtagStaffStep(niffStaffStep StaffStep);

extern
RIFFIOSuccess
NIFFIOtagThickness(niffThickness Thickness);

extern
RIFFIOSuccess
NIFFIOtagTieDirection(niffTieDirection TieDirection);

extern
RIFFIOSuccess
NIFFIOtagVoiceID(niffVoiceID VoiceID);

extern
RIFFIOSuccess
NIFFIOtagWidth(niffWidth Width);


#endif

