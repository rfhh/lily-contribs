#ifndef lint
static const char rcsid[] =
"$Id: callback.c,v 1.3 1996/06/15 02:53:50 tim Exp $";
#endif

/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
 
#include <assert.h>
#include "niffio.h"

extern void doindent(unsigned nLevel);

doline(int indent)
{

    int i;

    if ((--indent) < 0)
        indent = 0;

    
    for (i = indent * 4; i < 80; i++)
        printf("/");

    printf("\n");
}


int 
printSHORT(int nIndent, int nFieldWidth, 
           SHORT h, const char *strComment)
{

    doindent(nIndent+1);
    return printf("%*hd  // %s\n", nFieldWidth, h, strComment); 
}

int
printSTROFFSET(int nIndent, int nFieldWidth,
               STROFFSET offset, const char *strComment)
{
    doindent(nIndent+1);
    return printf("%*luL // %s\n", nFieldWidth, offset, strComment);

}

int
printRATIONAL(int nIndent, int nFieldWidth,
              RATIONAL r, const char *strComment)
{
    doindent(nIndent+1);
    printf("%*hd  // %s numerator\n", 
           nFieldWidth, r.numerator, strComment);
    doindent(nIndent+1);
    return 
        printf("%*hd  // %s denominator\n", 
               nFieldWidth, r.denominator, strComment);

}

int
printBYTE(int nIndent, int nFieldWidth,
          BYTE b, const char *strComment)
{
    doindent(nIndent+1);
    return printf("%*huC // %s\n", nFieldWidth, b, strComment);

}

printSIGNEDBYTE(int nIndent, int nFieldWidth,
                SIGNEDBYTE sb, const char *strComment)
{
    doindent(nIndent + 1);
    return printf("%*hdC // %s\n", nFieldWidth, sb, strComment);
}

/* Print a Time-Slice type */
printTS(int nIndent, int nFieldWidth,
        BYTE ts, const char *strComment)
{

    if (! NIFFIOSymbolTS(ts))
        return printBYTE(nIndent, nFieldWidth, ts, strComment);

    doindent(nIndent + 1);
    return printf("%*s  // %s\n", nFieldWidth, NIFFIOSymbolTS(ts), strComment);

}

/* Print a Clef Shape */
printCLEFSHAPE(int nIndent, int nFieldWidth,
               BYTE cs, const char *strComment)
{

    if (! NIFFIOSymbolCLEFSHAPE(cs))
        return printBYTE(nIndent, nFieldWidth, cs, strComment);
    
    doindent(nIndent + 1);
    return printf("%*s  // %s\n", 
                  nFieldWidth, NIFFIOSymbolCLEFSHAPE(cs), strComment);
    
}

printSymbol( const char * (decoder(BYTE)),
             int nIndent, int nFieldWidth,
             BYTE symbol, const char *strComment)
{

    /* Do we know about this value */
    if (! decoder(symbol))
    {
        printf("//\n");
        printf("// WARNING: UNKNOWN VALUE for %s\n", strComment);
        printf("//\n");
        return printBYTE(nIndent, nFieldWidth, symbol, strComment);
    }   

    doindent(nIndent + 1);
    return printf("%*s  // %s\n", 
                  nFieldWidth, decoder(symbol), strComment);
    
    
}

/*
 * cbListStart
 * ===========
 * Print the beginning of a list
 */
RIFFIOSuccess
cbListStart(NIFFIOChunkContext *pctxChunk)
{
    int i;

    char strId[RIFFIO_FOURCC_LIM];
    char strType[RIFFIO_FOURCC_LIM];

    assert (pctxChunk);
    assert(pctxChunk->pnf);
    assert(pctxChunk->pchunk);


    RIFFIOFOURCCToString(pctxChunk->pchunk->fccId, strId);
    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);


    doindent(pctxChunk->nLevel);
    doline(pctxChunk->nLevel);
    doindent(pctxChunk->nLevel);
    printf("// %s\n", NIFFIONameListType(pctxChunk->pchunk->fccType));
    doindent(pctxChunk->nLevel);
    printf("'%s' ( '%s' // sizeData = %ul\n",
           strId, strType, pctxChunk->pchunk->sizeData);

    printf("\n");

    return RIFFIO_OK;

}

RIFFIOSuccess
cbListEnd(NIFFIOChunkContext *pctxChunk)
{

    char strType[RIFFIO_FOURCC_LIM];

    assert (pctxChunk);
    assert(pctxChunk->pnf);
    assert(pctxChunk->pchunk);


    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);

    doindent(pctxChunk->nLevel);
    printf(")\n");
    doindent(pctxChunk->nLevel);
    printf("// %s\n", NIFFIONameListType(pctxChunk->pchunk->fccType));
    doindent(pctxChunk->nLevel);
    doline(pctxChunk->nLevel);

    printf("\n");

    return RIFFIO_OK;

}

RIFFIOSuccess
cbFormStart(NIFFIOChunkContext *pctxChunk)
{
    char strId[RIFFIO_FOURCC_LIM];
    char strType[RIFFIO_FOURCC_LIM];
        
    assert(pctxChunk != 0);
    assert(pctxChunk->pnf != 0);
    assert(pctxChunk->pchunk != 0);


    RIFFIOFOURCCToString(pctxChunk->pchunk->fccId, strId);
    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);

    doline(0);
    printf("// NIFF Form\n");
    printf("'%s' ( '%s' // sizeData=%lu\n",
           strId, strType, pctxChunk->pchunk->sizeData);

    printf("\n");

    return RIFFIO_OK;
}

RIFFIOSuccess
cbFormEnd(NIFFIOChunkContext *pctxChunk)
{
    assert (pctxChunk != 0);
    assert (pctxChunk->pnf != 0);
    assert (pctxChunk->pchunk != 0);

    printf(")\n");
    printf("// NIFF Form End\n");
    doline(0);
    return RIFFIO_OK;

}

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

    doindent(pctxChunk->nLevel);
    doline(pctxChunk->nLevel);
    doindent(pctxChunk->nLevel);
    printf("// %s\n", NIFFIONameChunkId(pctxChunk->pchunk->fccId));
    doindent(pctxChunk->nLevel);
    printf("'%s' ( // sizeData=%lu\n",
           strId, pctxChunk->pchunk->sizeData);

    printf("\n");
    
}


/*
 * cbChunkEnd
 * ==========
 * Print the closing parenthesis of a chunk
 */
RIFFIOSuccess
cbChunkEnd(NIFFIOChunkContext *pctxChunk)
{

    doindent(pctxChunk->nLevel);
    printf(")\n\n");

    return RIFFIO_OK;
}

/*
 * cbChunk
 * =======
 * Default callback for any chunk 
 */
RIFFIOSuccess
cbChunk(NIFFIOChunkContext *pctxChunk)
{
    cbChunkStart(pctxChunk);

    doindent(pctxChunk->nLevel + 1);
    printf("// Don't know how to decode this chunk (yet).\n");

    printf("\n");

    return cbChunkEnd(pctxChunk);

}

/* cbTag
 * =====
 * Default callback for any tag
 */
RIFFIOSuccess
cbTagStart(NIFFIOTagContext *pctxTag)
{
    assert(pctxTag != 0);
    assert(pctxTag->pnf != 0);
    assert(pctxTag->pchunkParent != 0);

    doindent(pctxTag->nLevel);
    doline(pctxTag->nLevel);
    
    doindent(pctxTag->nLevel);
    printf ("// %s\n", 
            NIFFIONameTagId(pctxTag->ptag->tagid));

    doindent(pctxTag->nLevel);
    printf("%uC ( // sizeData = %d\n", 
           pctxTag->ptag->tagid,
           pctxTag->ptag->tagsizeData);

    printf("\n");

    return RIFFIO_OK;


}

cbTagEnd(NIFFIOTagContext *pctxTag)
{
    doindent(pctxTag->nLevel);
    printf(")\n\n");

    return RIFFIO_OK;
}

RIFFIOSuccess
cbTag(NIFFIOTagContext *pctxTag)
{
    cbTagStart(pctxTag);

    doindent(pctxTag->nLevel+1);
    printf("Don't know how to decode tag (yet).\n");

    return cbTagEnd(pctxTag);
}

RIFFIOSuccess
cbSetupSectionStart(NIFFIOChunkContext *pctxChunk)
{
    char strId[5];
    char strType[5];

    assert(pctxChunk != 0);
    assert(pctxChunk->pnf != 0);
    assert(pctxChunk->pchunk != 0);

    RIFFIOFOURCCToString(pctxChunk->pchunk->fccId, strId);
    RIFFIOFOURCCToString(pctxChunk->pchunk->fccType, strType);

    doindent(pctxChunk->nLevel);
    printf("'%s' ( '%s' // Setup Section, sizeData=%lu\n",
           strId, strType, pctxChunk->pchunk->sizeData);


    return RIFFIO_OK;
        
}

RIFFIOSuccess
cbSetupSectionEnd(NIFFIOChunkContext *pctxChunk)
{
    assert(pctxChunk != 0);
    assert(pctxChunk->pnf != 0);
    assert(pctxChunk->pchunk != 0);

    doindent(pctxChunk->nLevel);
    printf(") // Setup Section\n");

    return RIFFIO_OK;

}

RIFFIOSuccess
cbInfoStart(NIFFIOChunkContext *pctxChunk, niffNiffInfo *ni)
{

    cbChunkStart(pctxChunk);

    doindent(pctxChunk->nLevel+1);
    printf("\"%.8s\" // NIF Version\n", ni->NIFFVersion);

    doindent(pctxChunk->nLevel+1);
    printf("%10dC // Program Type\n", ni->programType);

    doindent(pctxChunk->nLevel+1);
    printf("%10dC // Standard Units\n", ni->standardUnits);

    doindent(pctxChunk->nLevel+1);
    printf("%10d  // Absolute Units\n", ni->absoluteUnits);

    doindent(pctxChunk->nLevel+1);
    printf("%10d  // Midi clocks per quarter\n", ni->midiClocksPerQuarter);

    return RIFFIO_OK;

}

RIFFIOSuccess
cbInfoEnd(NIFFIOChunkContext *pctxChunk,niffNiffInfo *ni)
{
    return cbChunkEnd(pctxChunk);

}

RIFFIOSuccess
cbPartStart(NIFFIOChunkContext *pctxChunk, niffPart *np)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);
    
    printSHORT(     nLevel, 10, np->partID,         "Part ID");
    printSTROFFSET( nLevel, 10, np->name,           "name");
    printSTROFFSET( nLevel, 10, np->abbreviation,   "abbreviation");
    printBYTE(      nLevel, 10, np->numberOfStaves, "number of staves");
    printSIGNEDBYTE(nLevel, 10, np->midiChannel,    "MIDI channel");
    printSIGNEDBYTE(nLevel, 10, np->midiCable,      "MIDI cable");
    printSIGNEDBYTE(nLevel, 10, np->transpose,      "transpose");
    printf("\n");
}

RIFFIOSuccess
cbPartEnd(NIFFIOChunkContext *pctxChunk, niffPart *np)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbTimeSliceStart(NIFFIOChunkContext *pctxChunk, niffTimeSlice *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolTS, nLevel, 15,  p->type, "type");
    printRATIONAL(nLevel, 15, p->startTime, "start time");
    printf("\n");

}

RIFFIOSuccess
cbTimeSliceEnd(NIFFIOChunkContext *pctxChunk, niffTimeSlice *p)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbBarlineStart(NIFFIOChunkContext *pctxChunk, niffBarline *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolBARTYPE, 
                nLevel, 20, p->type, "type");

    printSymbol(NIFFIOSymbolBAREXT, 
                nLevel, 20, p->extendsTo, "extends to");

    printSHORT(nLevel, 20, p->numberOfStaves, "number of staves");

    printf("\n");

}

RIFFIOSuccess
cbBarlineEnd(NIFFIOChunkContext *pctxChunk, niffBarline *p)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbClefStart(NIFFIOChunkContext *pctxChunk, niffClef *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolCLEFSHAPE, 
                nLevel, 15, p->shape, "shape");

    printSIGNEDBYTE(nLevel, 15, p->staffStep, "staffStep");

    printSymbol(NIFFIOSymbolCLEFOCT, 
                nLevel, 15, p->octaveNumber, "octaveNumber");
    printf("\n");

}

RIFFIOSuccess
cbClefEnd(NIFFIOChunkContext *pctxChunk, niffClef *p)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbKeySignatureStart(NIFFIOChunkContext *pctxChunk, niffKeySignature *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSIGNEDBYTE(nLevel, 15, p->standardCode, "standardCode");

    printf("\n");

}

RIFFIOSuccess
cbKeySignatureEnd(NIFFIOChunkContext *pctxChunk, niffKeySignature *p)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbTimeSignatureStart(NIFFIOChunkContext *pctxChunk, niffTimeSignature *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSIGNEDBYTE(nLevel, 15, p->topNumber,    "top number");
    printBYTE      (nLevel, 15, p->bottomNumber, "bottom number");

    printf("\n");

}

RIFFIOSuccess
cbTimeSignatureEnd(NIFFIOChunkContext *pctxChunk, niffTimeSignature *p)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbNoteheadStart(NIFFIOChunkContext *pctxChunk, niffNotehead *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolNOTESHAPE, 
                nLevel, 20, p->shape, "shape");
    printSIGNEDBYTE(nLevel, 20, p->staffStep,    "staff step");
    printRATIONAL  (nLevel, 20, p->duration, "duration");

    printf("\n");

}

RIFFIOSuccess
cbNoteheadEnd(NIFFIOChunkContext *pctxChunk, niffNotehead *p)
{
    return cbChunkEnd(pctxChunk);
}

RIFFIOSuccess
cbRestStart(NIFFIOChunkContext *pctxChunk, niffRest *p)
{
    int nLevel;
    
    nLevel = pctxChunk->nLevel;

    cbChunkStart(pctxChunk);

    printSymbol(NIFFIOSymbolREST, nLevel, 15, p->shape, "shape");
    printSIGNEDBYTE(nLevel, 15, p->staffStep,    "staff step");
    printRATIONAL  (nLevel, 15, p->duration, "duration");

    printf("\n");

}

RIFFIOSuccess
cbRestEnd(NIFFIOChunkContext *pctxChunk, niffRest *p)
{
    return cbChunkEnd(pctxChunk);
}


RIFFIOSuccess
cbInvisible(NIFFIOTagContext *pctxTag)
{
    assert(pctxTag != 0);
    assert(pctxTag->pnf != 0);
    assert(pctxTag->pchunkParent != 0);

    doindent(pctxTag->nLevel);
    printf("%uC <> // Invisible\n", pctxTag->ptag->tagid);
    return RIFFIO_OK;


}

RIFFIOSuccess
cbLogicalPlacement(NIFFIOTagContext *pctxTag, niffLogicalPlacement *p)
{
    int nLevel;

    nLevel = pctxTag->nLevel;

    assert(pctxTag != 0);
    assert(pctxTag->pnf != 0);
    assert(pctxTag->pchunkParent != 0);

    cbTagStart(pctxTag);

    printSymbol(NIFFIOSymbolLOGPLACEH, 
                nLevel, 20, p->horizontal, "horizontal");

    printSymbol(NIFFIOSymbolLOGPLACEV, 
                nLevel, 20, p->vertical, "vertical");

    printSymbol(NIFFIOSymbolLOGPLACEPROX, 
                nLevel, 20, p->proximity, "proximity");

    cbTagEnd(pctxTag);

    return RIFFIO_OK;


}

RIFFIOSuccess
cbNumberOfFlags(NIFFIOTagContext *pctxTag, niffNumberOfFlags *p)
{
    int nLevel;

    nLevel = pctxTag->nLevel;

    assert(pctxTag != 0);
    assert(pctxTag->pnf != 0);
    assert(pctxTag->pchunkParent != 0);

    cbTagStart(pctxTag);

    printBYTE(nLevel, 10, *p, "");

    cbTagEnd(pctxTag);

    return RIFFIO_OK;


}

RIFFIOSuccess 
cbStringTable(NIFFIOChunkContext *pctxChunk)
{
    char strFOURCC[RIFFIO_FOURCC_LIM]; /* Buffer for printing FOURCCs */

    NIFFIOFile *pnf;
    RIFFIOChunk *pchunk;

    niffChklentabEntry entryCLT;

    char buffer;
    unsigned long offset;
    int bytesRead;

    pnf = pctxChunk->pnf;
    pchunk = pctxChunk->pchunk;
 

    cbChunkStart(pctxChunk);


    offset = 0;
    while (! NIFFIOChunkDataEnd(pnf, pchunk))
    {
        doindent(pctxChunk->nLevel+1);
        printf("// offset == %lu\n", offset);
        doindent(pctxChunk->nLevel+1);
        printf("\"");
        
        bytesRead = NIFFIORead(pnf, &buffer, 1);
        offset += bytesRead;

        while(buffer)
        {
            printf("%c", buffer);
            bytesRead = NIFFIORead(pnf, &buffer, 1);
            offset += bytesRead;
        }

        printf ("\"Z\n");
        printf ("\n");

    }

    cbChunkEnd(pctxChunk);

}

RIFFIOSuccess
cbChnkLenTable(NIFFIOChunkContext *pctxChunk)
{
    char strFOURCC[RIFFIO_FOURCC_LIM]; /* Buffer for printing FOURCCs */

    NIFFIOFile *pnf;
    RIFFIOChunk *pchunk;

    niffChklentabEntry entryCLT;

    
    pnf = pctxChunk->pnf;
    pchunk = pctxChunk->pchunk;

    cbChunkStart(pctxChunk);

    while (! NIFFIOChunkDataEnd(pnf, pchunk))
    {
        NIFFIOReadniffChklentabEntry(pnf, & entryCLT);
        RIFFIOFOURCCToString(entryCLT.chunkName, strFOURCC);
        doindent(pctxChunk->nLevel+1);
        printf("'%s' %5ldL\n", strFOURCC, entryCLT.offsetOfFirstTag);
    }
    

    doindent(pctxChunk->nLevel);
    printf("')'\n");

    printf("\n");

}

NIFFIOParser *
InitParser(void)
{
    NIFFIOParser *pparserNew;
        
    pparserNew = NIFFIOParserNew();

    NIFFIORegisterForm(pparserNew, cbFormStart, cbFormEnd);
    NIFFIORegisterDefaultList(pparserNew, cbListStart, cbListEnd);
    NIFFIORegisterDefaultAtomicChunk(pparserNew, cbChunk);
    NIFFIORegisterDefaultTaggedChunk(pparserNew, cbChunk, 0);
    NIFFIORegisterDefaultTag(pparserNew, cbTag);

    NIFFIORegisterChunkChnkLenTable(pparserNew, cbChnkLenTable);
    NIFFIORegisterChunkStringTable(pparserNew, cbStringTable);
    NIFFIORegisterChunkNiffInfo(pparserNew, cbInfoStart, cbInfoEnd);
    NIFFIORegisterChunkPart(pparserNew, cbPartStart, cbPartEnd);

    NIFFIORegisterChunkSystemHeader(pparserNew, cbChunkStart, cbChunkEnd);
    NIFFIORegisterChunkStaffHeader(pparserNew, cbChunkStart, cbChunkEnd);

    NIFFIORegisterChunkTimeSlice(pparserNew, 
                                 cbTimeSliceStart,
                                 cbTimeSliceEnd);

    NIFFIORegisterChunkBarline(pparserNew, 
                               cbBarlineStart,
                               cbBarlineEnd);

    NIFFIORegisterChunkClef(pparserNew, 
                            cbClefStart,
                            cbClefEnd);

    NIFFIORegisterChunkKeySignature(pparserNew, 
                                    cbKeySignatureStart,
                                    cbKeySignatureEnd);

    NIFFIORegisterChunkTimeSignature(pparserNew, 
                                     cbTimeSignatureStart,
                                     cbTimeSignatureEnd);

    NIFFIORegisterChunkNotehead(pparserNew, cbNoteheadStart, cbNoteheadEnd);
    NIFFIORegisterChunkRest(pparserNew, cbRestStart, cbRestEnd);
    NIFFIORegisterChunkStem(pparserNew, cbChunkStart, cbChunkEnd);


    /* Not really used yet */
    NIFFIORegisterTagInvisible(pparserNew, niffckidArticulation, cbInvisible);

    NIFFIORegisterTagNumberOfFlags(pparserNew, 
                                   NIFFIO_FOURCC_WILDCARD, 
                                   cbNumberOfFlags);

    NIFFIORegisterTagLogicalPlacement(pparserNew, 
                                      NIFFIO_FOURCC_WILDCARD, 
                                      cbLogicalPlacement);

    return pparserNew;
        
}

    
