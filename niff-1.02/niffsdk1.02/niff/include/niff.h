/* 
$Id: niff.h,v 1.3 1996/06/04 05:48:21 tim Exp $
*/
/*

    File:       NIFF.h
    Copyright:  None
    Version:    NIFF 6a* (with minor corrections -TWB)
    Date:       June 3, 1996


This header includes typedefs for NIFF data types and tag, chunk, list
and form structures.  Also, declarations for the four character list
ID's and chunk ID's, and the one character tag ID's.

A macro is defined, called MAKEFOURCC, which assembles four ascii
characters into a four byte DWORD variable.  Comment it out if you are
using the Microsoft Multimedia I/O API.  This macro assembles the four
ascii characters into the DWORD in reverse order (i.e. from last to
first).  The NIFFIO/RIFFIO Parser (see below) contains functions for
reading FOURCC's from a NIFF file and writing them to a NIFF file, so
if you use this parser, the order of the characters should be
transparent to you.

You may also want to comment out the other typedefs which duplicate
those in Windows, such as BYTE, SIGNEDBYTE, DWORD, etc.

CORRECTIONS FROM NIFF 6a
========================
    - System Separation Mark chunk has its field named "where" (pg. 66).
    - FONTIDX replaces FONTPTR (pg. 21).
    - User chunk (described pg. 21) is defined here, 
      but not in NIFF 6a chunk definitions.
    - User tag (described pg. 21) is defined here, 
      but not in NIFF 6a tag definitions.
    - Midi Performance tag fields "startTime" and "duration" are LONG instead
      of RATIONAL

    - MidiDataStream cannot have a variable length.
  
*** NIFFIO/RIFFIO Parser ***

This header may be used as part of the RIFFIO/NIFFIO public-domain
parser developed by Tim Butler. The parser source code contains
high-level and low-level functions for reading and writing RIFF/NIFF
files.  It was developed in part to help overcome the following
programming challenges inherent in a multi-platform standard:

1. Integer values are always stored in a NIFF file with their most
significant byte first and their least significant byte last, like
Motorola/Macintosh machines, not Intel/IBM-compatible/Windows
machines.

2. In NIFF files, fields of type SHORT and LONG will always be
exactly 16 and 32 bits, respectively.  However, "short" and "long" in
some C compilers may be longer.  Therefore, the structure members are
sufficient for in-memory use, but don't be misled into thinking they
are neccessarily the same size as the data in the file.

3. Most compilers do some padding between fields within a structure
for the purpose of byte alignment. This keeps you from being able to
read these structures in and write them out directly.


*/

#ifndef _NIFFH_
#define _NIFFH_


/**********************************
 ******   NIFF DATA TYPES    ******
 **********************************/

typedef unsigned char   BYTE;           /* a one byte unsigned integer */
typedef signed char     SIGNEDBYTE;     /* a one byte signed integer */
typedef unsigned char   CHAR;           /* a one byte ascii character */
typedef signed short    SHORT;          /* a two byte signed integer */
typedef signed long     LONG;           /* a four byte signed integer */
typedef unsigned long   FOURCC;         /* a 4 byte ascii character value */
typedef unsigned long   DWORD;          /* used in FOURBYTES macro     */

typedef struct {
    SHORT   numerator;
    SHORT   denominator;
}                       RATIONAL;       /* two 2-byte signed
                                           integers, used for timing
                                           representation */

typedef long            STROFFSET;       /* a four byte signed integer
                                           pointing to a RIFF ZSTR in
                                           the String Table chunk in
                                           the Setup Section. */

typedef unsigned short  FONTIDX;         /* a 2-byte index to a Font
                                            Description chunk in the
                                            Font Descriptions list. */

typedef char            VARLEN[1];       /* used for fields of
                                            variable-length */

typedef struct {
    BYTE tagID;
    BYTE tagSize;               /* the size of the tag's data,
                                   beginning with <tagData> */
    BYTE tagData[1];            /* first byte of the tag's data */
}                       NIFF_TAG;

typedef struct {
    FOURCC  ckID;
    DWORD   ckSize;                 /* the size of the chunk's data,
                                       beginning with <ckData> */
    BYTE    ckData[1];              /* first byte of the chunk's data */
}                       NIFF_CHUNK;

typedef struct {
    FOURCC  listConst;              /* the constant 'LIST'   */
    DWORD   listSize;               /* the size of the list's data, beginning with <listID> */
    FOURCC  listID;             
    BYTE    listData[1];            /* first byte of the list's data */
}                       NIFF_LIST;

typedef struct {
    FOURCC  formConst;              /* the constant 'RIFX'   */
    DWORD   formSize;               /* the size of the file, beginning
                                       with <formID> */
    FOURCC  formID;                 /* the constant 'NIFF' */
    BYTE    formData[1];            /* first byte of the list's data */
}                       NIFF_RIFF_FORM;

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)    \
        ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
        ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

#define niffformNiff  MAKEFOURCC('N','I','F','F')

/*************************
 ******   LISTS     ******
 *************************/

/********************************
 ******   SETUP SECTION    ******
 ********************************/



#define nifflistSetupSection     MAKEFOURCC('s','e','t','p')

#define nifflistParts            MAKEFOURCC('p','r','t','s')

#define nifflistRiffInfo         MAKEFOURCC('I','N','F','O')

#define nifflistGroupings        MAKEFOURCC('g','r','u','p')

#define nifflistFontDescs        MAKEFOURCC('f','n','t','s')

#define nifflistCustomGraphics   MAKEFOURCC('g','r','p','h')


/********************************
 ******   DATA SECTION     ******
 ********************************/

#define nifflistDataSection  MAKEFOURCC('d','a','t','a')

#define nifflistPage         MAKEFOURCC('p','a','g','e')

#define nifflistSystem       MAKEFOURCC('s','y','s','t')

#define nifflistStaff        MAKEFOURCC('s','t','a','f')


/*************************
 ******   CHUNKS    ******
 *************************/

/********************************
 ******   SETUP SECTION    ******
 ********************************/

typedef struct  niffChklentabEntry 
{           
        FOURCC  chunkName;
        LONG    offsetOfFirstTag;
}       niffChklentabEntry;

/*      niffChnkLenTable - no structure defined */
#define niffckidChnkLenTable  MAKEFOURCC('c','l','t',' ')
#define niffcklenChnkLenTable -1

typedef struct niffDefaultValues 
{
        FONTIDX musicFont;
        FONTIDX partNameFont;
        FONTIDX lyricFont;
        FONTIDX chordSymbolFont;
        FONTIDX measureNumberFont;
        FONTIDX rehearsalMarkFont;
        BYTE    tupletGroupingSymbol;
        BYTE    tupletNumberStyle;
}       niffDefaultValues;

#define niffckidDefaultValues  MAKEFOURCC('d','f','l','t')
#define niffcklenDefaultValues 14

enum 
{       deftupsymbNone = 0,
        deftupsymbNumberOnly = 1,
        deftupsymbNumberBrokenSlur = 2,
        deftupsymbNumberOutsideSlur = 3,
        deftupsymbNumberInsideSlur = 4,
        deftupsymbNumberBrokenBracket = 5,
        deftupsymbNumberOutsideBracket = 6,
        deftupsymbNumberInsideBracket = 7
};

enum 
{       deftupnumNone = 0,
        deftupnumFirstNumberOnly = 1,
        deftupnumTwoNumbersColon = 2,
        deftupnumStringOverride = 3
};

/*      niffEpsGraphic - no structure defined */
#define niffckidEpsGraphic  MAKEFOURCC('e','p','s','g')
#define niffcklenEpsGraphic -1

typedef struct  niffFontDescription
{   
        STROFFSET fontNamePtr;
        SHORT   size;
        SHORT   spaceHeight;
        SHORT   where;
        BYTE    style;
}       niffFontDescription;

#define niffckidFontDescription  MAKEFOURCC('f','n','t','d')
#define niffcklenFontDescription 11

#define     fontKnown   -1

enum 
{       fontstylePlain = 0,
        fontstyleBold = 1,
        fontstyleItalic = 2,
        fontstyleUnderscored = 4
};


typedef struct  niffNiffInfo {  
        CHAR    NIFFVersion [8];
        BYTE    programType;
        BYTE    standardUnits;
        SHORT   absoluteUnits;
        SHORT   midiClocksPerQuarter;
}       niffNiffInfo;

#define niffckidNiffInfo  MAKEFOURCC('n','n','f','o')
#define niffcklenNiffInfo 14

enum 
{       progtypeNone = 0,
        progtypeEngraving = 1,
        progtypeScanning = 2,
        progtypeMidiInterpreter = 3,
        progtypeSequencer = 4,
        progtypeResearch = 5,
        progtypeEducational = 6
};

enum 
{       unitsNoUnits = -1,
        unitsInches = 1,
        unitsCentimeters = 2,
        unitsPoints = 3
};

typedef struct  niffPart
{       
        SHORT       partID;
        STROFFSET   name;
        STROFFSET   abbreviation;
        BYTE        numberOfStaves;
        SIGNEDBYTE  midiChannel;
        SIGNEDBYTE  midiCable;  
        SIGNEDBYTE  transpose;  
}       niffPart;

#define niffckidPart  MAKEFOURCC('p','a','r','t')
#define niffcklenPart 14

/*      niffPsType1Font - no structure defined */
#define niffckidPsType1Font  MAKEFOURCC('p','s','t','1')
#define niffcklenPsType1Font -1

/*      niffPsType3Font - no structure defined */
#define niffckidPsType3Font  MAKEFOURCC('p','s','t','3')
#define niffcklenPsType3Font -1

typedef struct  niffStaffGrouping
{ 
        BYTE    groupingType;
        SHORT   firstStaff;
        SHORT   lastStaff;
}       niffStaffGrouping;

#define niffckidStaffGrouping  MAKEFOURCC('s','t','f','g')
#define niffcklenStaffGrouping 5

enum 
{       stgrpVertLine = 1,
        stgrpBrace = 2,
        stgrpBracket = 3
};

/*      niffStringTable - no structure defined */
#define niffckidStringTable  MAKEFOURCC('s','t','b','l')
#define niffcklenStringTable -1

/********************************
 ******   DATA SECTION    ******
 ******   HEADER CHUNKS   ******
 ********************************/

/*      niffPageHeader - no structure defined */
#define niffckidPageHeader  MAKEFOURCC('p','g','h','d')
#define niffcklenPageHeader 0

/*      niffStaffHeader - no structure defined */
#define niffckidStaffHeader  MAKEFOURCC('s','t','h','d')
#define niffcklenStaffHeader 0

/*      niffSystemHeader - no structure defined */
#define niffckidSystemHeader  MAKEFOURCC('s','y','h','d')
#define niffcklenSystemHeader 0

typedef struct  niffTimeSlice 
{ 
        BYTE        type;
        RATIONAL    startTime;  
}       niffTimeSlice;

#define niffckidTimeSlice  MAKEFOURCC('t','m','s','l')
#define niffcklenTimeSlice 5

enum    
{       tsMeasureStart = 1,
        tsEvent = 2
};

/********************************
 ******   DATA SECTION    ******
 ******   SYMBOL CHUNKS   ******
 ********************************/


typedef struct  niffAccidental
{
        BYTE    shape;
}       niffAccidental;

#define niffckidAccidental  MAKEFOURCC('a','c','d','l')
#define niffcklenAccidental 1

enum 
{       accDoubleFlat = 1,
        accFlat = 2,
        accNatural = 3,
        accSharp = 4,
        accDoubleSharp = 5,
        accQuarterToneFlat = 6,
        acc3QuarterTonesFlat = 7,
        accQuarterToneSharp = 8,
        accThreeQuarterTonesSharp = 9
};

typedef struct niffAltEndingGraphic 
{
        BYTE        bracketShape;
        STROFFSET   textString;
}       niffAltEndingGraphic;

#define niffckidAltEndingGraphic  MAKEFOURCC('a','l','t','g')
#define niffcklenAltEndingGraphic 5

enum 
{       altendNoDownjog = 0,
        altendDownjogBeginning = 1,
        altendDownjogEnd = 2,
        altendDownjogBegAndEnd = 3
};

typedef struct niffArpeggio
{
        BYTE        shape;
}       niffArpeggio;

#define niffckidArpeggio  MAKEFOURCC('a','r','p','g')
#define niffcklenArpeggio 1

enum 
{       arpeggWavyLine = 1,
        arpeggVertSlur = 2
};

typedef struct niffArticulation
{
        SHORT   shape;
}       niffArticulation;

#define niffckidArticulation  MAKEFOURCC('a','r','t','c')
#define niffcklenArticulation  2

enum 
{       artStrongAccent = 1,
        artMediumAccent = 2,
        artLightAccent = 3,
        artStaccato = 4,
        artDownBow = 5,
        artUpBow = 6,
        artHarmonic = 7,
        artFermata = 8,
        artArsisSign = 9,
        artThesisSign = 10,
        artPlusSign = 11,
        artVerticalFilledWedge = 12,
        artDoubleTonguing = 13,
        artTripleTonguing = 14,
        artSnapPizzicato = 15
};

/*      niffAugDot - no structure defined */
#define niffckidAugDot  MAKEFOURCC('a','u','g','d')
#define niffcklenAugDot 0

typedef struct  niffBarline 
{
        BYTE    type;   
        BYTE    extendsTo;
        SHORT   numberOfStaves; 
}       niffBarline;

#define niffckidBarline  MAKEFOURCC('b','a','r','l')
#define niffcklenBarline  4

enum 
{       bartypeThin = 1,
        bartypeThick = 2
};

enum 
{       barextThruLastStaff = 1,
        barextThruSpace = 2,
        barextBetweenStaves = 3
};
        
typedef struct  niffBeam
{
        BYTE    beamPartsToLeft;    
        BYTE    beamPartsToRight;
}       niffBeam;

#define niffckidBeam  MAKEFOURCC('b','e','a','m')
#define niffcklenBeam  2

typedef struct  niffChordSymbol 
{
        STROFFSET   textDescription;
}       niffChordSymbol;

#define niffckidChordSymbol  MAKEFOURCC('c','h','r','d')
#define niffcklenChordSymbol 4

typedef struct  niffClef
{
        BYTE        shape;  
        SIGNEDBYTE  staffStep;  
        BYTE        octaveNumber;   
}       niffClef;

#define niffckidClef  MAKEFOURCC('c','l','e','f')
#define niffcklenClef  3

enum  
{       clefshapeGclef = 1,
        clefshapeFclef = 2,
        clefshapeCclef = 3,
        clefshapePercussion = 4,
        clefshapeDoubleGclef = 5,
        clefshapeGuitarTab = 6
};

enum 
{       clefoctNoNumber = 0,
        clefoct8Above = 1,
        clefoct8Below = 2,
        clefoct15Above = 3,
        clefoct15Below = 4
};

typedef struct   niffCustomGraphicChk
{
        SHORT   value;
}       niffCustomGraphicChk;

#define niffckidCustomGraphicChk  MAKEFOURCC('c','s','t','g')
#define niffcklenCustomGraphicChk  2

typedef struct  niffDynamic
{
        BYTE    code;   
}       niffDynamic;

#define niffckidDynamic  MAKEFOURCC('d','y','n','m')
#define niffcklenDynamic  1

enum 
{       dynpppp = 1,
        dynppp = 2,
        dynpp = 3,
        dynp = 4, 
        dynmp = 5,
        dynmf = 6,
        dynf = 7,
        dynff = 8,
        dynfff = 9,
        dynffff = 10,
        dynsp = 11,
        dynsf = 12,
        dynsfz = 13, 
        dynfz = 14,
        dynfp = 15,
        dyncresc = 16,
        dyncrescendo = 17,
        dyndim = 18,
        dyndiminuendo = 19
};

typedef struct  niffFiguredBass
{
        STROFFSET   textDescription;
}       niffFiguredBass;

#define niffckidFiguredBass  MAKEFOURCC('f','i','g','b')
#define niffcklenFiguredBass  4

    
typedef struct  niffFingering
{
        BYTE    shape;  
}       niffFingering;

#define niffckidFingering  MAKEFOURCC('f','i','n','g')
#define niffcklenFingering  1

enum 
{       finger0 = 0,
        finger1 = 1,
        finger2 = 2,
        finger3 = 3,
        finger4 = 4,
        finger5 = 5
};


/*      niffGlissando - no structure defined */
#define niffckidGlissando  MAKEFOURCC('g','l','i','s')
#define niffcklenGlissando  0

typedef struct  niffGuitarGrid
{
        BYTE        numberOfFrets;
        BYTE        numberOfStrings;
        STROFFSET   textDescription;
}       niffGuitarGrid;

#define niffckidGuitarGrid  MAKEFOURCC('g','t','g','r')
#define niffcklenGuitarGrid 6

typedef struct  niffGuitarTabNum    
{
        BYTE        number;
        SIGNEDBYTE  staffStep;
}       niffGuitarTabNum;

#define niffckidGuitarTabNum  MAKEFOURCC('g','t','t','b')
#define niffcklenGuitarTabNum  2

typedef struct  niffHairpin 
{
        BYTE    direction;
}       niffHairpin;

#define niffckidHairpin  MAKEFOURCC('h','r','p','n')
#define niffcklenHairpin  1

enum 
{       hairpinOpenLeft = 1,
        hairpinOpenRight = 2
};

typedef struct niffHarpPedal
{
        STROFFSET   pedalPositions;
}       niffHarpPedal;

#define niffckidHarpPedal  MAKEFOURCC('h','a','r','p')
#define niffcklenHarpPedal 4

typedef struct   niffKeySignature
{
        SIGNEDBYTE  standardCode;
}       niffKeySignature;

#define niffckidKeySignature  MAKEFOURCC('k','e','y','s')
#define niffcklenKeySignature  1

typedef struct   niffKeySignNonstandard
{
        BYTE    numberOfChunks;
}       niffKeySignNonstandard;
    
#define niffckidKeySignNonstandard  MAKEFOURCC('k','e','y','n')
#define niffcklenKeySignNonstandard 1

typedef struct   niffLine
{
        BYTE    shape;  
}       niffLine;

#define niffckidLine  MAKEFOURCC('l','i','n','e')
#define niffcklenLine  1

enum 
{       lineOrdinary = 0,
        lineArrowhead = 1,
        lineDownjog = 2,
        lineUpjog = 3
};

typedef struct  niffLyric
{
        STROFFSET   text;   
        BYTE        lyricVerseID;
}       niffLyric;

#define niffckidLyric  MAKEFOURCC('l','y','r','c')
#define niffcklenLyric  5

typedef struct   niffMeasureNumbering
{
        BYTE    numberWhichMeasures;
        BYTE    numberFrequency;
        SHORT   startingNumber; 
        FONTIDX fontID;
        BYTE    aboveOrBelow;
        BYTE    horizontalCentering;
        BYTE    enclosure;  
}       niffMeasureNumbering;

#define niffckidMeasureNumbering  MAKEFOURCC('m','e','a','s')
#define niffcklenMeasureNumbering  9

enum 
{       measnumwhichNone = 0,
        measnumwhichFreqField = 1,
        measnumwhichEachSystem = 2,
        measnumwhichStartOfPage = 3
};

enum 
{       measnumwhereCentered = 0,
        measnumwhereToLeft = 1,
        measnumwhereToRight = 2,
        measnumwhereSystemStart = 3
};

enum 
{       measnumstaffAbove = 0,
        measnumstaffBelow = 1
};

enum 
{       measnumencNone = 0,
        measnumencBox = 1,
        measnumencCircle = 2
};

typedef struct   niffMidiDataStream
{
        BYTE    startTime;  
}       niffMidiDataStream;
#define niffckidMidiDataStream  MAKEFOURCC('m','i','d','i')
#define niffcklenMidiDataStream  -1

typedef struct  niffFontSymbol
{
        FOURCC  chunkType;
        SHORT   spaceHeight;
        BYTE    shape;
}       niffFontSymbol;

#define niffckidFontSymbol  MAKEFOURCC('f','o','n','t')
#define niffcklenFontSymbol 7


typedef struct  niffNotehead
{
        BYTE        shape;  
        SIGNEDBYTE  staffStep;  
        RATIONAL    duration;   
}       niffNotehead;

#define niffckidNotehead  MAKEFOURCC('n','o','t','e')
#define niffcklenNotehead  6

enum 
{       noteshapeBreve = 1,
        noteshapeWhole = 2,
        noteshapeHalf = 3,
        noteshapeFilled = 4,
        noteshapeOpenDiamond = 5,
        noteshapeSolidDiamond = 6,
        noteshapeX = 7,
        noteshapeOpenX = 8,
        noteshapeGuitarSlash = 9,
        noteshapeOpenGuitarSlash = 10,
        noteshapeFilledSquare = 11,
        noteshapeOpenSquare = 12,
        noteshapeFilledTriangle = 13,
        noteshapeOpenTriangle = 14
};

typedef struct  niffOctaveSign
{
        BYTE        numberOfOctaves;
        BYTE        aboveOrBelow;
        BYTE        type;   
        STROFFSET   textString;
}       niffOctaveSign;

#define niffckidOctaveSign  MAKEFOURCC('o','c','t','v')
#define niffcklenOctaveSign  7

enum 
{       octaveAbove = 1,
        octaveBelow = 2
};
enum 
{       octavetypeTransposed = 1,
        octavetypeDoubled = 2
};


typedef struct  niffOrnament
{
        SHORT   shape;  
}       niffOrnament;

#define niffckidOrnament  MAKEFOURCC('o','r','n','m')
#define niffcklenOrnament 2

enum 
{       ornMordent = 1,
        ornInvertedMordent = 2,
        ornDoubleMordent = 3,
        ornInvertedDoubleMordent = 4,
        ornTurn = 5,
        ornInvertedTurn = 6,
        ornTr = 7,
        ornTrill2Bumps = 8,
        ornTrill3Bumps = 9,
        ornTrillLong = 10,
        ornTrillLongUphook = 11,
        ornTrillLongDownhook = 12
};

typedef struct  niffParenthesis
{
        BYTE    shape;
}       niffParenthesis;

#define niffckidParenthesis  MAKEFOURCC('p','r','e','n')
#define niffcklenParenthesis  1

enum 
{       parenLeftParen = 1,
        parenRightParen = 2,
        parenLeftBrace = 3,
        parentRightBrace = 4,
        parenLeftBracket = 5,
        parenRightBracket = 6,
        parenLeftRightParen = 7,
        parenLeftRightBrace = 8,
        parenLeftRightBracket = 9
};


typedef struct  niffPedalPiano
{
        BYTE    shape;  
}       niffPedalPiano;

#define niffckidPedalPiano  MAKEFOURCC('p','d','l','p')
#define niffcklenPedalPiano  1

enum 
{       pedalpianoNone = 0,
        pedalpianoPedGraphic = 1,
        pedalpianoAsterisk = 2,
        pedalpianoAlternateEnd = 3,
        pedalpianoHorizUpjog = 4,
        pedalpianoHorizDownjog = 5,
        pedalpianoUpslantUpjog = 6,
        pedalpianoUpslantDownjog = 7,
        pedalpianoNotch = 8,
        pedalpianoUnaCorda = 9,
        pedalpianoTreCorde = 10,
        pedalpianoSP = 11
};
        
typedef struct  niffPedalOrgan
{
        BYTE    shape;
}       niffPedalOrgan;

#define niffckidPedalOrgan  MAKEFOURCC('p','d','l','o')
#define niffcklenPedalOrgan  1

enum 
{       pedalorganHeelU = 1,
        pedalorganHeelO = 2,
        pedalorganToeUp = 3,
        pedalOrganToeDown = 4
};


/*      niffPortamento - no structure defined */
#define niffckidPortamento  MAKEFOURCC('p','o','r','t')
#define niffcklenPortamento  0

typedef struct  niffRehearsalMark
{
        STROFFSET   textString;
        BYTE        enclosure;  
}       niffRehearsalMark;

#define niffckidRehearsalMark  MAKEFOURCC('r','h','r','s')
#define niffcklenRehearsalMark  5

enum 
{       rehearsalencNone = 0,
        rehearsalencBox = 1,
        rehearsalencCircle = 2
};


typedef struct  niffRepeatSign
{
        SHORT   graphicalCode;
        BYTE    logicalCode;    
}       niffRepeatSign;

#define niffckidRepeatSign  MAKEFOURCC('r','e','p','t')
#define niffcklenRepeatSign 3

enum 
{       repeatgrNone = 0,
        repeatgrSegno = 1,
        repeatgrCoda = 2,
        repeatgrSlash1 = 3,
        repeatgrSlash2 = 4,
        repeatgrSlash3 = 5,
        repeatgrSlash2wDots = 6,
        repeatgrSlash1wDots = 7,
        repeatgrSlash1wDotsBar = 8,
        repeatgrSlash2wDotsBar = 9,
        repeatgrDots = 10,
        repeatgrDC = 11,
        repeatgrDCalFine = 12,
        repeatgrDCalSegno = 13,
        repeatgrDCalSegnoCoda = 14,
        repeatgrDS = 15,
        repeatgrDSalFine = 16,
        repeatgrFine = 17,
        repeatgrCodaText = 18
};

enum 
{       repeatlogNone = 0,
        repeatlogBeginning = 1,
        repeatlogEndSection = 2,
        repeatlogprevBeat = 3, 
        repeatlogprevMeasure = 4, 
        repeatlogprev2Measures = 5,
        repeatlogWords = 6,
        repeatlogChord = 7
};

typedef struct niffRest
{ 
        BYTE        shape;  
        SIGNEDBYTE  staffStep;  
        RATIONAL    duration;   
} niffRest;

#define niffckidRest  MAKEFOURCC('r','e','s','t')
#define niffcklenRest  6

enum 
{       restBreve = 1,
        restWhole = 2,
        restHalf = 3,
        restQuarter = 4,
        restEighth = 5,
        rest16th = 6,
        rest32nd = 7,
        rest64th = 8,
        rest128th = 9,
        rest256th = 10,
        restMult4measures = 11,
        restMultHzBar = 12,
        restMultSlantedBar = 13,
        restVocalComma = 14,
        restVocal2Slashes = 15
};

/*      niffSlur - no structure defined */
#define niffckidSlur  MAKEFOURCC('s','l','u','r')
#define niffcklenSlur  0

/*      niffStem - no structure defined */
#define niffckidStem  MAKEFOURCC('s','t','e','m')
#define niffcklenStem  0

typedef struct niffSystemSeparation
{
        BYTE    where;
}       niffSystemSeparation;

#define niffckidSystemSeparation  MAKEFOURCC('s','s','e','p')
#define niffcklenSystemSeparation  1

enum 
{       systsepLeftMargin = 1,
        systsepRightMargin = 2
};  

/*      niffTagActivate - no structure defined */
#define niffckidTagActivate  MAKEFOURCC('t','a','g','a')
#define niffcklenTagActivate  0

/*      niffTagInactivate - no structure defined */
#define niffckidTagInactivate  MAKEFOURCC('t','a','g','i')
#define niffcklenTagInactivate  0

typedef struct niffTempoMarking
{
        STROFFSET   textString;
        RATIONAL    noteValue;  
        SHORT       beatsPerMinute;
}       niffTempoMarking;

#define niffckidTempoMarking  MAKEFOURCC('t','m','p','o')
#define niffcklenTempoMarking  10

typedef struct niffTempoMarkNonstandard
{
        BYTE    numberOfChunks;
}       niffTempoMarkNonstandard;

#define niffckidTempoMarkNonstandard  MAKEFOURCC('t','m','p','n')
#define niffcklenTempoMarkNonstandard 1

typedef struct niffText
{
        STROFFSET   value;
}       niffText;

#define niffckidText  MAKEFOURCC('t','e','x','t')
#define niffcklenText  4

/*      niffTie - no structure defined */
#define niffckidTie  MAKEFOURCC('t','i','e',' ')
#define niffcklenTie  0

typedef struct niffTimeSignature
{
        SIGNEDBYTE  topNumber;  
        BYTE        bottomNumber;   
}       niffTimeSignature;

#define niffckidTimeSignature  MAKEFOURCC('t','i','m','e')
#define niffcklenTimeSignature 2

typedef struct niffTimeSigNonstandard
{
        BYTE    numberOfChunks;
}       niffTimeSigNonstandard;

#define niffckidTimeSigNonstandard  MAKEFOURCC('t','i','m','n')
#define niffcklenTimeSigNonstandard 1


typedef struct niffTremolo
{
        BYTE    attachedBeamParts;
        BYTE    unattachedBeamParts;    
}       niffTremolo;

#define niffckidTremolo  MAKEFOURCC('t','m','l','o')
#define niffcklenTremolo  2

/*      niffTuplet - no structure defined */
#define niffckidTuplet  MAKEFOURCC('t','u','p','l')
#define niffcklenTuplet  0

/*      niffUserDefined - no structure defined */
#define niffckidUserDefined  MAKEFOURCC('u','s','e','r')

/*************************
 ******   TAGS    ******
 *************************/

typedef struct niffAbsPlacement
{
        SHORT   horizontal;
        SHORT   vertical;   
}       niffAbsPlacement;
enum {  nifftagAbsPlacement = 0x01};

typedef BYTE    niffAltEnding;  
enum {  nifftagAltEnding = 0x02};   

typedef FOURCC  niffAnchorOverride; 
enum {  nifftagAnchorOverride = 0x03};  


typedef BYTE    niffArticDirection; 
enum {  nifftagArticDirection = 0x04};  

enum 
{       articUp = 1,
        articDown = 2
};

typedef struct niffBezierIncoming
{
        SHORT   horizontal;
        SHORT   vertical;
}       niffBezierIncoming;
enum {  nifftagBezierIncoming = 0x05};

typedef struct niffBezierOutgoing
{
        SHORT   horizontal;
        SHORT   vertical;
}       niffBezierOutgoing;
enum {  nifftagBezierOutgoing = 0x06};

typedef SHORT   niffChordSymbolsOffset;
enum {  nifftagChordSymbolsOffset = 0x07};

typedef struct niffCustomFontChar
{
        FONTIDX fontID;
        CHAR    characterCode[2];   
}       niffCustomFontChar;
enum {  nifftagCustomFontChar = 0x08};

typedef SHORT   niffCustomGraphicTag;   
enum {  nifftagCustomGraphicTag = 0x09};

/*      niffEndOfSystem  - tag without data */  
enum {  nifftagEndOfSystem = 0x0A};


typedef SHORT   niffFannedBeam;
enum {  nifftagFannedBeam = 0x0B};

enum 
{       fanbeamExpanding = 1,
        fanbeamShrinking = 2
};


typedef SHORT       niffFigBassOffset;  
enum {      nifftagFigBassOffset = 0x0C};

typedef FONTIDX     niffFontID;
enum {      nifftagFontID = 0x0D};

typedef RATIONAL    niffGraceNote;
enum {      nifftagGraceNote = 0x0E};

typedef SHORT       niffGuitarGridOffset;   
enum {      nifftagGuitarGridOffset = 0x0F};

/*          niffGuitarTabTag - tag without data */
enum {      nifftagGuitarTabTag = 0x10};

typedef SHORT       niffHeight; 
enum {      nifftagHeight = 0x11};

typedef SHORT       niffID; 
enum {      nifftagID = 0x12};

/*          niffInvisible - tag without data */ 
enum {      nifftagInvisible = 0x13};

/*          niffLargeSize - tag without data */
enum {      nifftagLargeSize = 0x14};

typedef BYTE        niffLineQuality;
enum {      nifftagLineQuality = 0x15};

enum 
{       linequalNoLine = 0,
        linequalDotted = 1,
        linequalDashed = 2,
        linequalWavy = 3
};

typedef struct niffLogicalPlacement
{
        BYTE    horizontal;
        BYTE    vertical;
        BYTE    proximity;  
}       niffLogicalPlacement;
enum {  nifftagLogicalPlacement = 0x16};

enum 
{       logplaceHDefault = 0,
        logplaceHLeft = 1,
        logplaceHRight = 2,
        logplaceHStemside = 3,
        logplaceHNoteside = 4,
        logplaceHCentered = 5
};

enum 
{       logplaceVDefault = 0,
        logplaceVAbove = 1,
        logplaceVBelow = 2,
        logplaceVStemside = 3,
        logplaceVNoteside = 4,
        logplaceVCentered = 5
};

enum 
{       logplaceProxDefault = 0,
        logplaceProxAligned = 1,
        logplaceProxOffset = 2
};

typedef struct niffLyricVerseOffset
{
        SHORT   lyricLineOffset;    
        BYTE    lyricVerseID;
}       niffLyricVerseOffset;
enum {  nifftagLyricVerseOffset = 0x17};


typedef struct niffMidiPerformance
{
        LONG        startTime;
        LONG        duration;
        BYTE        pitch;
        BYTE        velocity;
}       niffMidiPerformance;    
enum {  nifftagMidiPerformance = 0x18};

    
/*      niffMultiNodeEndOfSyst - tag without data */ 
enum {  nifftagMultiNodeEndOfSyst = 0x19};

/*      niffMultiNodeStartOfSyst - tag without data */ 
enum {  nifftagMultiNodeStartOfSyst = 0x1A};

        
typedef BYTE    niffNumberOfFlags;  
enum {  nifftagNumberOfFlags = 0x1B};

typedef SHORT   niffNumberOfNodes;  
enum {  nifftagNumberOfNodes = 0x1C};

typedef BYTE    niffNumStaffLines;
enum {  nifftagNumStaffLines = 0x1D};

typedef BYTE    niffOssia;  
enum {  nifftagOssia = 0x1E};

enum 
{       ossiaDisplayOnly = 0,
        ossiaPlayback = 1
};
 
typedef struct niffPartDescOverride
{
        SIGNEDBYTE  MIDIchannel;    
        SIGNEDBYTE  MIDIcable;
        SIGNEDBYTE  transpose;  
}       niffPartDescOverride;
enum {  nifftagPartDescOverride = 0x1F};

#define MIDIchannelNone -1
#define MIDIcableNone  -1

typedef SHORT   niffPartID;
enum {  nifftagPartID = 0x20};
        
typedef struct niffRefPtOverride
{
        BYTE    anchor_h;   
        BYTE    dependent_h;
        BYTE    anchor_v;
        BYTE    dependent_v;
}       niffRefPtOverride;
enum {  nifftagRefPtOverride = 0x21};

enum 
{       refptHDefault = 0,
        refptHLeft = 1,
        refptHRight = 2,
        refptHCenter = 3
};

enum 
{       refptVDefault = 0,
        refptVTop = 1,
        refptVBottom = 2,
        refptVCenter = 3
};

typedef SHORT   niffRehearsalOffset;
enum {  nifftagRehearsalOffset = 0x22};

typedef SHORT   niffRestNumeral;
enum {  nifftagRestNumeral = 0x23};

/*      niffSilent - tag without data   */
enum {  nifftagSilent = 0x24};

/*      niffSlashedStem - tag without data */
enum {  nifftagSlashedStem = 0x25};

/*      niffSmallSize - tag without data */
enum {  nifftagSmallSize = 0x26};

/*      niffSpacingByPart - tag without data */
enum {  nifftagSpacingByPart = 0x27};

/*      niffSplitStem - tag without data */
enum {  nifftagSplitStem = 0x28};

/*      niffStaffName - tag without data */
enum {  nifftagStaffName = 0x29};

typedef SIGNEDBYTE  niffStaffStep; 
enum {      nifftagStaffStep = 0x2A};

typedef SHORT   niffThickness;
enum {  nifftagThickness = 0x2B};

typedef BYTE    niffTieDirection;
enum {  nifftagTieDirection = 0x2C};

enum 
{       tieRoundedAbove = 1,
        tieRoundedBelow = 2
};

typedef struct niffTupletDesc
{
        RATIONAL    transformRatioAB;
        RATIONAL    transformRatioCD;
        BYTE        groupingSymbol;
}       niffTupletDesc;
enum {  nifftagTupletDesc = 0x2D};

enum 
{       tupletgrpDefault = 0,
        tupletgrpNumOnly = 1,
        tupletgrpNumBrokenSlur = 2,
        tupletgrpNumOutsideSlur = 3,
        tupletgrpNumInsideSlur = 4,
        tupletgrpNumBrokenBracket = 5,
        tupletgrpNumOutsideBracket = 6,
        tupletgrpNumInsideBracket = 7,
        tupletgrpBracketOnly = 8,
        tupletgrpSlurOnly = 9,
        tupletgrpNoSymbol = 10
};

typedef BYTE    niffNumberStyle;
enum {  nifftagNumberStyle = 0x2E};

enum 
{       numstyleDefault = 0,
        numstyleFirstNumOnly = 1,
        numstyle2NumsWithColon = 2,
        numstyleNumString = 3,
        numstyleNoNumber = 4
};

typedef SHORT   niffVoiceID;
enum {  nifftagVoiceID = 0x2F};

typedef SHORT   niffWidth;
enum {  nifftagWidth = 0x30};

enum {  nifftagUserDefined = 0xFF};


#endif    
/* #ifndef _NIFFH_ */
