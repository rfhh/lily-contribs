/* 
$Id: niffiop.h,v 1.1 1996/06/06 04:57:39 tim Exp $ 
*/
/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef _NIFFIOPH_
#define _NIFFIOPH_

#include "niff.h"
#include "niffio.h"


typedef RIFFIOSuccess 
(*NIFFIOCookedChunkCallback)(NIFFIOChunkContext *pchunkcntx, 
                             void *pvFixed);

typedef RIFFIOSuccess 
(*NIFFIOCookedTagCallback)(NIFFIOTagContext *ptagcntx,
                           void *pvFixed);

typedef RIFFIOSuccess (*NIFFIOPReader)(NIFFIOFile *pnf, void *pv);

typedef struct
{
    NIFFIOChunkCallback  cbStart;
    NIFFIOChunkCallback  cbEnd;
} NIFFIOPFormEntry;

typedef struct
{
    NIFFIOChunkCallback  cbStart;
    NIFFIOChunkCallback  cbEnd;
} NIFFIOPListEntry;


typedef struct
{
    NIFFIOPReader            cbRead;
    union
    {
        NIFFIOChunkCallback       raw;
        NIFFIOCookedChunkCallback cooked;
    } cbStart;
    
    union
    {
        NIFFIOChunkCallback       raw;
        NIFFIOCookedChunkCallback cooked;
    } cbEnd;
    
} NIFFIOPTaggedChunkEntry;

typedef struct
{
    NIFFIOChunkCallback     cbStart;
    NIFFIOChunkCallback     cbEnd;
} NIFFIOPDefaultTaggedChunkEntry;

typedef struct
{
    NIFFIOChunkCallback cb;
} NIFFIOPAtomicChunkEntry;

typedef struct
{
    NIFFIOChunkCallback cb;
} NIFFIOPDefaultAtomicChunkEntry;

typedef struct
{
    NIFFIOChunkCallback cb;
} NIFFIOPUserChunkEntry;

typedef struct
{
    FOURCC fcc;             /* limits matches to parent chunk id */
    NIFFIOPReader cbRead;
    union
    {
        NIFFIOTagCallback        raw;
        NIFFIOCookedTagCallback cooked;
    } cbTag;
} NIFFIOPTagEntry;

typedef struct
{
    NIFFIOTagCallback cbTag;
} NIFFIOPDefaultTagEntry;

typedef struct
{
    NIFFIOTagCallback cb;
} NIFFIOPUserTagEntry;

/*********************
 * Tag Callback Tables 
 *********************/

typedef struct NIFFIOPTCBTable NIFFIOPTCBTable;

extern NIFFIOPTCBTable *
NIFFIOPTCBTableNew(void);

extern void
NIFFIOPTCBTableDelete(NIFFIOPTCBTable *ptable);

extern RIFFIOSuccess
NIFFIOPTCBTableMakeEntry(NIFFIOPTCBTable *ptable, 
                         BYTE tagid,
                         const NIFFIOPTagEntry *ptagentry);

extern RIFFIOSuccess
NIFFIOPTCBTableLookup(NIFFIOPTCBTable *pTCBTable,
                      BYTE tagid,
                      NIFFIOPTagEntry *ptagentry);

/**************
 * Parser
 *************/

#define NIFFIOP_DEFAULT_CHUNKBUFFER 256
#define NIFFIOP_DEFAULT_TAGBUFFER   256

typedef struct NIFFIOPParser
{
    char               strName[40]; /* Identifies parser */
    int                isTracing;   /* True if tracing enabled */
    NIFFIOParserTracer tracer;      /* Callback to handle tracing output  */
    
    NIFFIOPFormEntry formentry;
    RIFFIOFCCTable   *pfcctblLists;        /* List callbacks */
    RIFFIOFCCTable   *pfcctblTaggedChunks; /* Chunk (tagfull) callbacks */
    RIFFIOFCCTable   *pfcctblAtomicChunks; /* Chunk (tagless) callbacks */
    NIFFIOPTCBTable  *ptcbtblTags;         /* Tag callbacks */
    
    NIFFIOPListEntry         defaultList;        /* Default list callbacks  */
    NIFFIOPDefaultTaggedChunkEntry 
    defaultTaggedChunk; /* Default chunk callbacks */
    NIFFIOPDefaultAtomicChunkEntry 
    defaultAtomicChunk; /* Default chunk callbacks */
    NIFFIOPDefaultTagEntry   defaultTag;         /* Default tag callbacks */
    
    void * pvChunkBuffer;
    void * pvTagBuffer;
    
} NIFFIOPParser;

extern int
NIFFIOPChunkIsAtomic(NIFFIOChunkLengthTable *pclt, FOURCC fccId);


#endif
