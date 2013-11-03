#ifndef lint
static char rcsid[] = 
"$Id: nfile.c,v 1.3 1996/06/12 07:00:14 tim Exp $";
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
 * nfile - NIFFIOFile routines
 *
 * SYNOPSIS
 * ========
 *
 * - NIFFIOFileNew()
 * - NIFFIOFileNewSTDC()
 * - NIFFIOFileInit()
 * - NIFFIOFileDelete()
 *
 * - NIFFIOFileGetCLT()
 * - NIFFIOFileReadCLT()
 * - NIFFIOFileAdoptCLT()
 *
 * - NIFFIOSeekChunkTags()
 */
/***************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "stdcriff.h"
#include "niffio.h"

/***************************************************************************/
/*
 * NIFFIOFileNew
 * =============
 * Return a pointer to a new NIFFIOFile, or null on failure.
 */
NIFFIOFile *
NIFFIOFileNew(void)
/*
 * OBLIGATIONS
 * -----------
 * - Don't forget to call NIFFIOFileInit() for your new file.
 * - You must eventually call NIFFIOFileDelete() to free the NIFFIOFile.
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOFileNew";
    
    NIFFIOFile *pnfNew;
        
    pnfNew = (NIFFIOFile *) malloc(sizeof(NIFFIOFile));

    if (pnfNew == 0)
    {
        RIFFIOError(strModule, "Failed to allocate NIFFIOFile");
        return (NIFFIOFile *) 0;
    }

    /*
     * Initialize the chunk length table
     */
    pnfNew->pclt = 0;

    return pnfNew;
}

/***************************************************************************/
/*
 * NIFFIOFileNewSTDC
 * =================
 * Create a new NIFFIOFile given a Standard C Library FILE pointer
 */
NIFFIOFile *
NIFFIOFileNewSTDC(FILE *fp)
/*
 * OBLIGATIONS
 * -----------
 * You must call NIFFIOFileDelete() to free the new NIFFIOFile.
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOFileNewSTDC";

    NIFFIOFile *pnf; /* file to return */

    assert(fp);

    pnf = NIFFIOFileNew();
    if (! pnf)
    {
        RIFFIOError(strModule, "Failed to allocate a new NIFFIOFile");
        return 0;
    }
    
    if (! NIFFIOFileInit(pnf, fp, 
                         STDCRIFFRead, STDCRIFFWrite, 
                         STDCRIFFSeek, STDCRIFFTell))
    {
        RIFFIOError(strModule, "Can't initialize NIFFIOFile");
        NIFFIOFileDelete(pnf);
        return 0;
    }
 
   return pnf;

}
/***************************************************************************/
/*
 * NIFFIOFileInit
 * ==============
 * Provide a NIFFIOFile with a user-defined file and I/O callbacks.
 */
RIFFIOSuccess
NIFFIOFileInit(NIFFIOFile *pnf,
               void *pvUserFile,
               RIFFIOReader reader,
               RIFFIOWriter writer,
               RIFFIOSeeker seeker,
               RIFFIOTeller teller)
/*
 * ENTRY
 * -----
 * 
 *  <pnf> : 
 *    is the NIFFIOFile being initialized.
 *
 *  <pvUserFile> :
 *    is a user representation of a file that will be passed to
 *    user-defined reader, writer, seeker, and teller functions.
 * 
 *  <reader>,<writer>,<seeker>,<teller> :
 *    are user-defined functions that perform operations on <pvUserFile>
 *
 * OBLIGATIONS
 * -----------
 * NIFFFIOFileInit() should be called immediately after NIFFIOFileNew()
 *
 */
/***************************************************************************/
{
    assert (pnf != 0);
        
    pnf->pclt = 0;

    return RIFFIOFileInit((RIFFIOFile *) pnf, 
                          pvUserFile, 
                          reader, writer, seeker, teller);

}

/***************************************************************************/
/*
 * NIFFIOFileDelete
 * ================
 * Free the memory associated with a NIFFIOFile.
 */
void
NIFFIOFileDelete(NIFFIOFile *pnf)
/*
 * WARNING
 * ------- 
 * This also frees the memory of any NIFFIOChunkLengthTable 
 * that may have been associated with the NIFFIOFile 
 * using NIFFIOFileAdoptCLT().
 */
/***************************************************************************/
{
    assert (pnf != 0);

    /*
     * Delete the chunk length table
     * if it exists
     */
    if (pnf->pclt != 0)
        NIFFIOCLTDelete(pnf->pclt);

    free(pnf);
}



/***************************************************************************/
/*
 * NIFFIOFileGetCLT
 * ================
 * Return a pointer to a NIFFIOFile's chunk length table.
 */
NIFFIOChunkLengthTable *
NIFFIOFileGetCLT(NIFFIOFile *pnf)
/* 
 * RETURN
 * ------
 * null if the chunk length table has not be set or read yet. 
 *
 * OBLIGATIONS
 * -----------
 * This pointer may become invalid after calls to
 * other NIFFIO routines.  Such as what ?????
 *
 */
/***************************************************************************/
{
        assert(pnf != 0);
        return pnf->pclt;
} 

/***************************************************************************/
/*
 * NIFFIOFileReadCLT
 * =================
 * Read and set a NIFFIOFile's chunk length table.
 */
RIFFIOSuccess
NIFFIOFileReadCLT(NIFFIOFile *pnf, RIFFIOChunk *pchunkCLT)
/*
 * ENTRY
 * -----
 * <*pchunkCLT> :
 *    is a chunk length table chunk that has been
 *    returned by NIFFIOChunkDescend().
 * 
 * <*pnf> :
 *    may be positioned anywhere
 * 
 * EXIT
 * ----
 * Leaves the NIFFIOFile positioned after the chunk length table chunk.
 */
/***************************************************************************/
{
    char strModule[] = "NIFFIOFileReadCLT";

    NIFFIOChunkLengthTable *pcltNew;
    niffChklentabEntry     niffcltentry;

    assert (pnf != 0);
    assert (pchunkCLT != 0);
        
    /*
     * Do we already have the CLT?
     */
    if (pnf->pclt != 0)
    {
        NIFFIOCLTDelete(pnf->pclt);
        pnf->pclt = 0;
    }

    /*
     * Seek to the first CLT entry
     */
    if (! NIFFIOChunkSeekData(pnf, pchunkCLT))
    {
        RIFFIOError(strModule, 
                    "Failed to seek to first chunk length table entry");
        return RIFFIO_FAIL;
    }
        
    pcltNew = NIFFIOCLTNew();
    if (pcltNew == 0)
        return RIFFIO_FAIL;

    /*
     * Read each entry
     */
    while (! NIFFIOChunkDataEnd(pnf, pchunkCLT))
    {
        if (!NIFFIOReadniffChklentabEntry(pnf, &niffcltentry))
        {
            RIFFIOError(strModule, "Failed to read chunk length table entry");
            NIFFIOCLTDelete(pcltNew);
            return RIFFIO_FAIL;
        }

        if (! NIFFIOCLTMakeEntry(pcltNew, niffcltentry))
        {
            RIFFIOError(strModule, 
                        "Failed to record chunk length table entry");
            NIFFIOCLTDelete(pcltNew);
            return RIFFIO_FAIL;
        }
    }

    /*
     * Leave the file positioned at the end of the chunk length table
     */

    if (! NIFFIOChunkAscend(pnf, pchunkCLT))
    {
        NIFFIOCLTDelete(pcltNew);
        return RIFFIO_FAIL;
    }

    pnf->pclt = pcltNew;


    return RIFFIO_OK;

}


/*
 * _CLTWriteHelper
 * ===============
 * Used by sort() to compare FOURCCs 
 *
 * Compare two chunk length table entries 
 * Return <0 if a comes before b
 *         0 if a is the same as b
 *        >0 if a comes after b
 *
 * The results of this function are 
 * compatible with bsearch() and qsort() 
 * in the Standard C Library
 */
static int
_CLTWriteHelper(const void *aEntryp, 
                                const void *bEntryp)
{
    assert(aEntryp != 0);
    assert(bEntryp != 0);
    
    return NIFFIOCompareFOURCC(
                               ((RIFFIOFCCTableEntry *) aEntryp)->fcc,
                               ((RIFFIOFCCTableEntry *) bEntryp)->fcc);
    
}

/***************************************************************************/
/*
 * NIFFIOFileAdoptCLT
 * ==================
 * Assign and write a chunk length table to a NIFFIOFile.
 */
RIFFIOSuccess
NIFFIOFileAdoptCLT(NIFFIOFile *pnf, NIFFIOChunkLengthTable *pclt)
/* 
 * ENTRY
 * -----
 * The NIFFIOFile must be positioned at the start of data
 * of its CLT chunk.
 *
 * EXIT
 * ----
 * Leaves the file positioned at the end of the CLT chunk data
 * that it just wrote.
 *
 * OBLIGATIONS
 * ----------- 
 * Upon return, the NIFFIOFile takes responsibility for
 * deleting the CLT you just gave it.  
 * Don't free or delete <*pclt>; that will be done by either another
 * call to NIFFIOFileAdoptCLT() or NIFFIOFileDelete().
 */
/***************************************************************************/
{

    char strModule[] = "NIFFIOFileAdoptCLT";

    unsigned count; 
    RIFFIOFCCTableEntry *tableEntryArray;
    niffChklentabEntry  niffcltentry;

    count = RIFFIOFCCTableCount(pclt);

    tableEntryArray = RIFFIOFCCTableCreateArray(pclt);
    if (tableEntryArray == 0)
        return RIFFIO_FAIL;

    qsort(tableEntryArray, 
          count, 
          sizeof(RIFFIOFCCTableEntry),
          _CLTWriteHelper);

    {
        int i;
        char strId[5];
#if 0
        printf("==================================\n");
        printf("Writing sorted Chunk Length Table\n");
#endif          
        for(i = 0; i < count; i++)
        {
            RIFFIOFOURCCToString(tableEntryArray[i].fcc, strId);

            niffcltentry.chunkName = tableEntryArray[i].fcc;
            niffcltentry.offsetOfFirstTag = tableEntryArray[i].value.l;
                        
            if (! NIFFIOWriteniffChklentabEntry(pnf, &niffcltentry))
            {
                RIFFIOError(strModule, 
                            "Failed to write chunk length table entry %s, size %lu",
                            strId, tableEntryArray[i].value.l);
                return RIFFIO_FAIL;
            }
#if 0
            printf("<%s>%ld\n", fccString, tableEntryArray[i].value.l);
#endif
        }

#if 0
        printf("===================================\n");
#endif
    }

    free(tableEntryArray);
        
    /*
     * Replace the clt associated with this niff file
     */

    if (pnf->pclt != 0)
        NIFFIOCLTDelete(pnf->pclt);
        
    pnf->pclt = pclt;

    return RIFFIO_OK;

}


/***************************************************************************/
/*
 * NIFFIOSeekChunkTags
 * ===================
 * Position a NIFFIOFile at the start of a given chunk's tags.
 */
RIFFIOSuccess
NIFFIOSeekChunkTags(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/*
 * This is done according to the chunk length table associated
 * with the NIFFIOFile.
 * 
 * It is an error if there is no associated CLT. Use NIFFIOFileGetCLT()
 * to find out.
 * 
 * RETURNS
 * -------
 *  RIFFIO_OK :
 *    on success
 *  
 *  RIFFIO_FAIL :
 *    if the lookup failed to find the chunks id in the CLT
 * 
 *  RIFFIO_FAIL :
 *    on a file seek error 
 */
/***************************************************************************/
{
    char strModule[] = "NIFFIOSeekChunkTags";

    char strID[RIFFIO_FOURCC_LIM];
    niffChklentabEntry cltentry;

    assert (pnf != 0);
    assert (pnf->pclt != 0); /* Make sure file has an assigned CLT */

    assert (pchunk != 0);

    RIFFIOFOURCCToString(pchunk->fccId, strID);

    /*
     * Lookup chunk fcc in CLT
     */
    cltentry.chunkName = pchunk->fccId;
        
    if (! NIFFIOCLTLookup(pnf->pclt, &cltentry))
    {
        RIFFIOError(strModule, "CLT lookup failed, id <%s>", strID);
        return RIFFIO_FAIL;
    }

    if (cltentry.offsetOfFirstTag == -1)
    {
        RIFFIOError(strModule, "Chunk tags not permitted, id <%s>", strID);
        return RIFFIO_FAIL;
    }

    /*
     * Seek to the first tag
     */
    if (! NIFFIOSeek(pnf, 
                     pchunk->offsetData + cltentry.offsetOfFirstTag,
                     RIFFIO_SEEK_SET))
    {
        RIFFIOError(strModule, "Seek failed, id <%s>", strID);
        return RIFFIO_FAIL;
    }

    return RIFFIO_OK;
}




