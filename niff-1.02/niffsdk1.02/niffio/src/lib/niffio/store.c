#ifndef lint
static char rcsid[] = 
"$Id: store.c,v 1.4 1996/06/15 02:53:56 tim Exp $";
#endif

#include <stdio.h> 
#include "stdcriff.h"

/***************************************************************************/
/* 
 * NAME
 * ====
 * store - Handy routines for creating NIFF lists, chunks, and tags.
 *
 * SYNOPSIS
 * ========
 * 
 * Creation and Deletion
 * ---------------------
 * - NIFFIOStorageNew()
 * - NIFFIOStorageInit()
 * - NIFFIOStorageNewSTDC()
 * - NIFFIOStorageDelete()
 *
 * Selection
 * ---------
 * - NIFFIOStorageGetCurrent()
 * - NIFFIOStorageSetCurrent()
 *
 * Property Access 
 * ---------------
 * - NIFFIOStorageGetFile()
 *
 * - NIFFIOStorageIsListPending()
 * - NIFFIOStorageIsChunkPending()
 * - NIFFIOStorageIsTagPending()
 *
 * - NIFFIOStoragePendingList()
 * - NIFFIOStoragePendingChunk()
 * - NIFFIOStoragePendingTag()
 *
 * Operations
 * ----------
 * - NIFFIOStorageListStart()
 * - NIFFIOStorageListEnd()
 * - NIFFIOStorageChunkStart()
 * - NIFFIOStorageChunkEnd()
 * - NIFFIOStorageTagStart()
 * - NIFFIOStorageTagEnd()
 *
 * Special Objects
 * ---------------
 * - NIFFIOStoreStbl()
 * - NIFFIOStoreCLT()
 * - NIFFIOStoreDefaultCLT()
 *
 */
/***************************************************************************/

#include <assert.h>
#include <stdlib.h>

#include "niffio.h"
#include "niffiop.h"

/*
 * NIFFIOPStore
 * ============
 * Remember the state of writing lists, chunks, and tags.
 */
typedef struct NIFFIOPStore
{
    NIFFIOFile  *pnf;   /* file we are writing to */
    
    int isFileOurs;     /* true if we own <*pnf>, 
                         * see NIFFIOStorageNewSTDC() */

    int isChunkPending; /* true if there is an unfinalized chunk */
    int isTagPending;   /* true if there is an unfinalized tag */
    
    RIFFIOChunkStack *pstackLists;  /* The pending lists */
    RIFFIOChunk       chunk;        /* The pending chunk */ 
    NIFFIOTag         tag;          /* The pending tag */
    
} NIFFIOPStore;


/*
 * _pstoreCurrent
 * ==============
 * The current state.
 * 
 * The user may change this with NIFFIOStorage???()
 *
 */
static NIFFIOStorage *_pstoreCurrent;
/***************************************************************************/
/*
 * NIFFIOStorageNew
 * ================
 * Create a new NIFFIOStorage, return null on failure.
 */
NIFFIOStorage *
NIFFIOStorageNew(void)
/*
 * OBLIGATIONS
 * -----------
 * - You should call NIFFIOStorageInit() immediately.
 * - Don't forget to free the NIFFIOStorage with NIFFIOStorageDelete().
 * - You must eventually call NIFFIOStorageSetCurrent() to use the
 *   new NIFFIOStorage object.
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageNew";

    NIFFIOStorage *pstore;

    pstore = (NIFFIOStorage *) malloc(sizeof(NIFFIOPStore));
    if (! pstore)
    {
        RIFFIOError(strModule, "Failed to allocate memory for NIFFIOStorage");
        return 0;
    }

    pstore->pnf = 0;
    pstore->isFileOurs = 0;
    pstore->isChunkPending = 0;
    pstore->isTagPending = 0;

    pstore->pstackLists = RIFFIOChunkStackNew();
    if (!pstore->pstackLists)
    {
        RIFFIOError(strModule, "Failed to allocate memory for NIFFIOStorage");
        free (pstore);
        return 0;
    }

    return pstore;

}

/***************************************************************************/
/*
 * NIFFIOStorageNewSTDC
 * ====================
 * Create a new NIFFIOStorage given a Standard C File pointer.
 * Make it the current NIFFIOStorage.
 */
NIFFIOStorage *
NIFFIOStorageNewSTDC(FILE *fp)
/*
 * OBLIGATIONS 
 * ------------ 
 * The new NIFFIOStorage object will own a newly created NIFFIOFile.
 * That file is available through NIFFIOStorageGetFile() BUT YOU MUST
 * NEVER FREE IT. NIFFIOStorageDelete() will delete the NIFFIOFile.
 * This is different than NIFFIOStorageNew() followed by
 * NIFFIOStorageInit().
 * 
 * You must however, close <*fp> yourself. 
 *
 * ENTRY
 * -----
 * <fp> is a Standard C Library FILE pointer that has been opened for
 * binary writing.
 *
 * EXIT
 * ----
 * The returned NIFFIOStorage is the current one.
 *
 * RETURN
 * ------
 * Return null on failure.
 *
 * EXAMPLE
 * -------
 *|
 *|    FILE *fp;
 *|    NIFFIOStorage *pstore;
 *|
 *|    fp = fopen("filename.nif", "rw");
 *|    pstore = NIFFIOStorageNewSTDC(fp);
 *| 
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageNewSTDC";

    NIFFIOStorage *pstoreNew; /* Will point to the new NIFFIOStorage */

    NIFFIOFile *pnf;          /* The NIFFIOFile that will be ours    */
    
    assert (fp);

    /* Create the new NIFFIOStorage itself */
    pstoreNew = NIFFIOStorageNew();

    if (!pstoreNew)
    {
        RIFFIOError(strModule, "Failed to create an new NIFFIOStorage");
        return 0;
    }

    /* Create a NIFFIOFile */

    pnf = NIFFIOFileNew();
    if (!pnf)
    {
        RIFFIOError(strModule, "Failed to allocate a new NIFFIOFile");
        return 0;
    }

    /* Initialize the NIFFIOFile */
    if (! NIFFIOFileInit(pnf, fp,
                         STDCRIFFRead, STDCRIFFWrite, 
                         STDCRIFFSeek, STDCRIFFTell))
    {
        RIFFIOError(strModule, "Failed to initialize a new NIFFIOFile");
        RIFFIOFileDelete(pnf);
        return 0;
    }

    pstoreNew->pnf = pnf;
    pstoreNew->isFileOurs = 1;

    NIFFIOStorageSetCurrent(pstoreNew);
}


/***************************************************************************/
/*
 * NIFFIOStorageInit
 * =================
 * Associate a NIFFIOFile with a NIFFIOStorage.
 */
RIFFIOSuccess
NIFFIOStorageInit(NIFFIOStorage *pstore, NIFFIOFile *pnf)
/*
 * ENTRY
 * -----
 * T <*pnf> must exist and be open for writing.
 *
 * OBLIGATIONS
 * -----------
 * You are responsible for deleting <*pnf> after NIFFIOStorageDelete().
 * This is different than calling NIFFIOStorageNewSTDC().
 * 
/***************************************************************************/
{
    assert(pstore);
    assert(! pstore->isFileOurs); /* We aren't responsible (for pnf) */
    assert(pnf);
        
    pstore->pnf = pnf;

    return RIFFIO_OK;

}

/***************************************************************************/
/*
 * NIFFIOStorageDelete
 * ===================
 * Free the memory allocated to a NIFFIOStorage.
 */
void
NIFFIOStorageDelete(NIFFIOStorage *pstore)
/***************************************************************************/
{
    assert (pstore);
    assert (pstore->pstackLists);

    /* If we are debugging, give a warning if there are 
     * objects left unfinalized.
     */
#ifndef NDEBUG
    assert (!NIFFIOStorageIsTagPending());
    assert (!NIFFIOStorageIsChunkPending());
    assert (!NIFFIOStorageIsListPending());
#endif

    RIFFIOChunkStackDelete(pstore->pstackLists);

    /* Are we responsible for the NIFFIOFile? */
    if (pstore->isFileOurs)
    {
        NIFFIOFileDelete(pstore->pnf);
        pstore->pnf = 0;
    }

    free (pstore);

}

/***************************************************************************/
/*
 * NIFFIOStorageSetCurrent
 * =======================
 * Set the NIFFIOStorage that will be operated on by all
 * the NIFFIOStorage routines.
 */
void
NIFFIOStorageSetCurrent(NIFFIOStorage *pstore)
/***************************************************************************/
{
    _pstoreCurrent = pstore;
}

/***************************************************************************/
/*
 * NIFFIOStorageGetCurrent
 * =======================
 * Return a pointer to the current NIFFIOStorage.
 */
NIFFIOStorage *
NIFFIOStorageGetCurrent(void)
/***************************************************************************/
{
    return _pstoreCurrent;
}

/***************************************************************************/
/*
 * NIFFIOStorageGetFile
 * ====================
 * Return a pointer to the NIFFIOFile of the current NIFFIOStorage
 */
NIFFIOFile *
NIFFIOStorageGetFile(void)
/***************************************************************************/
{
    assert (_pstoreCurrent);
        
    return _pstoreCurrent->pnf;
}

/***************************************************************************/
/*
 * NIFFIOStorageIsTagPending
 * =========================
 * Return true if a tag is pending.
 */
int
NIFFIOStorageIsTagPending()
/***************************************************************************/
{
    assert (_pstoreCurrent);
        
    return (_pstoreCurrent->isTagPending);
}

/***************************************************************************/
/*
 * NIFFIOStorageIsChunkPending
 * ===========================
 * Return true if a chunk is pending.
 */
int 
NIFFIOStorageIsChunkPending(void)
/***************************************************************************/
{
    assert (_pstoreCurrent);
        
    return (_pstoreCurrent->isChunkPending);

}

/***************************************************************************/
/*
 * NIFFIOStorageIsListPending
 * ==========================
 * Return true if a list is pending.
 */
int
NIFFIOStorageIsListPending(void)
/***************************************************************************/
{
    assert (_pstoreCurrent);
    assert (_pstoreCurrent->pstackLists);

    return ! RIFFIOChunkStackIsEmpty(_pstoreCurrent->pstackLists);

}

/***************************************************************************/
/*
 * NIFFIOStoragePendingTag
 * =======================
 * Return a pointer to the current pending tag.
 */
NIFFIOTag *
NIFFIOStoragePendingTag(void)
/*
 * It is an error if there is no current pending tag.
 */
/***************************************************************************/
{
    assert (_pstoreCurrent);
    assert (NIFFIOStorageIsTagPending());

    return & (_pstoreCurrent->tag);
}

/***************************************************************************/
/*
 * NIFFIOStoragePendingChunk
 * =========================
 * Return a pointer to the current pending chunk.
 */
RIFFIOChunk *
NIFFIOStoragePendingChunk(void)
/* 
 * It is an error if there is no current pending chunk.
 */
/***************************************************************************/
{
    assert (_pstoreCurrent);
    assert (NIFFIOStorageIsChunkPending());

    return & (_pstoreCurrent->chunk);
}

/***************************************************************************/
/*
 * NIFFIOStoragePendingList
 * ========================
 * Return a pointer to the current pending list.
 */
RIFFIOChunk *
NIFFIOStoragePendingList(void)
/* 
 * It is an error if there is not current pending list.
 */
/***************************************************************************/
{
    assert (_pstoreCurrent);
    assert (NIFFIOStorageIsListPending);

    return RIFFIOChunkStackTop(_pstoreCurrent->pstackLists);

}

/***************************************************************************/
/*
 * NIFFIOStorageTagEnd
 * ====================
 * Finalize the current pending tag.
 */
RIFFIOSuccess
NIFFIOStorageTagEnd(void)
/*
 * This is a no-op if there is no tag pending.
 */ 
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageTagEnd";
        
    assert (_pstoreCurrent);
    assert (_pstoreCurrent->pnf);

    /*
     * If a tag is pending then finalize it
     */
    if (NIFFIOStorageIsTagPending())
    {
        if (! NIFFIOTagFinalize(_pstoreCurrent->pnf, 
                                NIFFIOStoragePendingTag()))
        {
            RIFFIOError(strModule, "Couldn't finalize pending tag");
            return RIFFIO_FAIL;
        }
    }

    _pstoreCurrent->isTagPending = 0;

    return RIFFIO_OK;
    
}

/***************************************************************************/
/*
 * NIFFIOStorageChunkEnd
 * =====================
 * Finalize the current pending chunk, including any pending tag.
 */
RIFFIOSuccess
NIFFIOStorageChunkEnd(void)\
/*
 * This is a no-op if there is no pending chunk or tag.
 */
/***************************************************************************/
{

    const char strModule[] = "NIFFIOStorageChunkEnd";
        
    assert (_pstoreCurrent);
    assert (_pstoreCurrent->pnf);

    /*
     * If a tag is pending then finalize it
     */

    if (! NIFFIOStorageTagEnd())
    {
        RIFFIOError(strModule, "Failed to finalize possible pending tag");
        return RIFFIO_FAIL;
    }

    /*
     * If a chunk is pending then finalize it
     */
    if (NIFFIOStorageIsChunkPending())
    {
        if (! NIFFIOChunkFinalize(_pstoreCurrent->pnf, 
                                  NIFFIOStoragePendingChunk()))
        {
            RIFFIOError(strModule, "Couldn't finalize pending chunk");
            return RIFFIO_FAIL;
        }
    }

    _pstoreCurrent->isChunkPending = 0;

    return RIFFIO_OK;
        
}

/***************************************************************************/
/*
 * NIFFIOStorageListEnd
 * ====================
 * Finalize the current pending list, including any pending chunk or tag.
 */
RIFFIOSuccess
NIFFIOStorageListEnd(void)
/* 
 * EXIT
 * ----
 * Any pending list is uncovered and made the current pending list.
 * 
 * This is a no-op if there is no pending tag, chunk, or list.
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageListEnd";
        
    assert (_pstoreCurrent);
    assert (_pstoreCurrent->pnf);
        
    /*
     * If a chunk is pending then finalize it
     * (as well as any pending tags)
     */

    if (! NIFFIOStorageChunkEnd())
    {
        RIFFIOError(strModule, 
                    "Failed to finalize possible pending chunks or tags");
        return RIFFIO_FAIL;
    }

    /*
     * If a list is pending then finalize it
     */

        
    if (NIFFIOStorageIsListPending())
    {
        if (! NIFFIOChunkFinalize(_pstoreCurrent->pnf, 
                                  NIFFIOStoragePendingList()))
        {
            RIFFIOError(strModule, "Couldn't finalize pending list");
            return RIFFIO_FAIL;
        }
                
        RIFFIOChunkStackPop(_pstoreCurrent->pstackLists);
    }

    return RIFFIO_OK;
}

/***************************************************************************/
/*
 * NIFFIOStorageTagStart
 * =====================
 * Write a tag header in a NIFFIOFile and remember the tag as pending.
 */
RIFFIOSuccess
NIFFIOStorageTagStart(NIFFIOTag *ptag)
/*
 * ENTRY
 * -----
 * There must be a pending chunk.
 *
 * OBLIGATIONS
 * -----------
 * You must finalize the chunk with a call to either NIFFIOStorageTagEnd(),
 * NIFFIOStorageChunkEnd(), or NIFFIOStorageListEnd().
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageTagStart";

    assert (_pstoreCurrent);
    assert (ptag);
    assert (NIFFIOStorageIsChunkPending()); /* You can't store a tag
                                               in a list */

    /*
     * Finalize any pending tag
     */
        
    if (! NIFFIOStorageTagEnd())
    {
        RIFFIOError(strModule, "Failed to finalize a pending tag");
        return RIFFIO_FAIL;
    }

    /*
     * Create the new tag
     */
    if (! NIFFIOTagCreate(NIFFIOStorageGetFile(), ptag))
    {
        RIFFIOError(strModule, "Failed to create tag");
        return RIFFIO_FAIL;
    }
        
    /*
     * Remember our pending tag
     */
    _pstoreCurrent->tag = *ptag;
    _pstoreCurrent->isTagPending = 1;

    return RIFFIO_OK;
}

/***************************************************************************/
/*
 * NIFFIOStorageChunkStart
 * =======================
 * Write a chunk header to the current NIFFIOFile and remember the chunk as 
 * pending. Any current pending chunk (and/or tag) is finalized.
 */
RIFFIOSuccess
NIFFIOStorageChunkStart(RIFFIOChunk *pchunk)
/*
 * OBLIGATIONS
 * -----------
 * You must finalize the chunk with a call to either NIFFIOStorageChunkEnd()
 * or NIFFIOStorageListEnd().
 */ 
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageChunkStart";

    assert (_pstoreCurrent);
    assert (pchunk);
        
    /*
     * Finalize any pending chunk
     */
        
    if (! NIFFIOStorageChunkEnd())
    {
        RIFFIOError(strModule, "Failed to finalize a pending chunk or tag");
        return RIFFIO_FAIL;
    }

    /*
     * Create the new chunk
     */
    if (! NIFFIOChunkCreate(NIFFIOStorageGetFile(), pchunk))
    {
        RIFFIOError(strModule, "Failed to create chunk");
        return RIFFIO_FAIL;
    }
        
    /*
     * Remember our pending chunk
     */
    _pstoreCurrent->chunk = *pchunk;
    _pstoreCurrent->isChunkPending = 1;

    return RIFFIO_OK;
}

/***************************************************************************/
/*
 * NIFFIOStorageListStart
 * ======================
 * Write a list chunk header to a NIFFIOFile and remember the list as pending.
 */
RIFFIOSuccess
NIFFIOStorageListStart(RIFFIOChunk *pchunk)
/*
 * The new list will be nested within any pending list.
 * Any pending chunk (not list) is finalized (along with its last tag).
 * 
 * OBLIGATIONS
 * -----------
 * You must finalize the list chunk with a call to NIFFIOStorageListEnd().
 */
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStorageListStart";

    assert (_pstoreCurrent);
    assert (pchunk);
        
    /*
     * Finalize any pending chunk
     */
        
    if (! NIFFIOStorageChunkEnd())
    {
        RIFFIOError(strModule, "Failed to finalize a pending chunk or tag");
        return RIFFIO_FAIL;
    }

    /*
     * Create the new list
     */
    if (! NIFFIOChunkCreate(NIFFIOStorageGetFile(), pchunk))
    {
        RIFFIOError(strModule, "Failed to create list");
        return RIFFIO_FAIL;
    }

    /*
     * Remember our pending list
     */
    if (! RIFFIOChunkStackPush(_pstoreCurrent->pstackLists,
                               pchunk))
    {
        RIFFIOError(strModule, "Failed to remember new list");
        return RIFFIO_FAIL;
    }

    return RIFFIO_OK;
}

/***************************************************************************/
/*
 * NIFFIOStoreStbl
 * ===============
 * Write a string table array using a NIFFIOStorage.
 */
int
NIFFIOStoreStbl(NIFFIOStbl *pstbl, int nEntries)
/*
 * Writes <nEntries> of *pstbl to the current NIFFIOStorage.
 * Each time you call NIFFIOStoreStbl() in sequence, the string table
 * entries are appended to the string table chunk.
 * 
 * ENTRY
 * -----
 * - T <*pstbl> points to an array of <nEntries> NIFFIOStbl's
 * 
 * - The current pending chunk must be a String Table.
 *
 * EXIT
 * ----
 * - Each string table offset is filled in with the correct value.
 *
 * - The current String Table chunk will still be pending.
 *
 * RETURN
 * ------
 * Returns the number of string table entries written.
 *
 * ERRORS
 * ------
 * On error the current NIFFIOStorage's file position is undefined.
/***************************************************************************/
{
    const char strModule[] = "NIFFIOStblStore";

    int nWritten = 0; /* Return the number of entries written */ 

    NIFFIOFile *pnf; 
    RIFFIOChunk *pchunkPending; 

    int i;

    assert(NIFFIOStorageGetCurrent());
    assert(pstbl);
    assert(nEntries >= 0);

    /* The NIFFIOStorage had better have a NIFFIOFile */
    pnf = NIFFIOStorageGetFile();
    assert (pnf);

    /* We must have a pending STBL chunk */
    assert(NIFFIOStorageIsChunkPending());
    pchunkPending = NIFFIOStoragePendingChunk();

    assert(pchunkPending->fccId == niffckidStringTable);

    for (i = 0; i < nEntries; i++)
    {
        if (!NIFFIOStblWrite(pnf, pchunkPending, pstbl + i))
        {
            RIFFIOError(strModule, "Failed to write string index %d", i);
            break;
        }

        nWritten++;
    }
    
    return nWritten;

}

/***************************************************************************/
/*
 * NIFFIOStoreCLT
 * ==============
 * Write chunk length table data to the current NIFFIOStorage.
 * Doesn't write the chunk header; use NIFFIOchunkCLT() for that.
 *
 */
RIFFIOSuccess
NIFFIOStoreCLT(NIFFIOChunkLengthTable *pclt)
/***************************************************************************/
/*
 * ENTRY
 * -----
 * The current pending chunk must be a chunk length table.
 *
 * EXIT
 * ----
 * The Chunk Length Table chunk will still be pending.
 *
 * OBLIGATIONS
 * -----------
 * You must NOT free <*pclt>. The NIFFIOStorage adopts the chunk length
 * table and associates it with its personal NIFFIOFile. 
 * 
 */
{
    const char strModule[] = "NIFFIOStoreCLT";

    RIFFIOSuccess status;

    NIFFIOFile *pnf;

    assert(pclt);

    assert(NIFFIOStorageGetCurrent());

    pnf = NIFFIOStorageGetFile();
    assert(pnf);

    status = NIFFIOFileAdoptCLT(pnf, pclt);
    if (! status)
    {
        RIFFIOError(strModule, "Failed to assign CLT to NIFFIOFile");
    }

    return status;

}
/***************************************************************************/
/*
 * NIFFIOStoreDefaultCLT
 * =====================
 * Write the default chunk length table data to the current NIFFIOStorage.
 * Writes the chunk header.
 *
 */
RIFFIOSuccess
NIFFIOStoreDefaultCLT(void)
/***************************************************************************/
/*
 * ENTRY
 * -----
 * The current pending list must be the setup section.
 *
 * EXIT
 * ----
 * The Chunk Length Table chunk will still be pending.
 *
 */
{
    const char strModule[] = "NIFFIOStoreDefaultCLT";
    
    RIFFIOChunk *chunkPending;
    NIFFIOChunkLengthTable *pclt;


    assert (NIFFIOStorageIsListPending());
    chunkPending = NIFFIOStoragePendingList();
    
    assert (chunkPending->fccType == nifflistSetupSection);

    pclt = NIFFIOCLTNew();

    if (!pclt)
    {
        RIFFIOError(strModule, "Failed to allocate a chunk length table");
        return RIFFIO_FAIL;
    }

    if (! NIFFIOCLTMakeDefaultEntries(pclt))
    {
        RIFFIOError(strModule, "Failed to make default entries in CLT");
        return RIFFIO_FAIL;
    }

    if (! NIFFIOchunkChnkLenTable())
    {
        RIFFIOError(strModule, "Failed to start a CLT chunk");

    }

    if (! NIFFIOStoreCLT(pclt))
        RIFFIOError(strModule, "NIFFIOStoreCLT failed");
    
    return RIFFIO_OK;

}

/***************************************************************************/
/*
 * NIFFIOStoreINFO
 * ===============
 * Write a chunk to the INFO list
 */
RIFFIOSuccess
NIFFIOStoreINFO(RIFFIOFOURCC fccId, const char *str)
/***************************************************************************/
/*
 * Writes a chunk to the current INFO list with <fccId> and fills the
 * chunk's data with a ZSTR (NUL-terminated text string).
 * 
 * ENTRY
 * -----
 * The current pending list must be the INFO list.
 *
 * EXIT
 * ----
 * - The info chunk is finalized.
 * - The INFO list will still be pending.
 *
 */
{
    const char strModule[] = "NIFFIOStoreINFO";

    int nBytesToWrite;         /* length of <*str> + 1 NUL */

    int nBytesWritten;         /* bytes actually written to file */

    RIFFIOChunk *chunkPending; /* The pending list, should be INFO */
    RIFFIOChunk chunkInfo;    /* The chunk we will write */

    /* Entry conditions */
    assert(str);

    assert (NIFFIOStorageIsListPending());
    chunkPending = NIFFIOStoragePendingList();
    
    assert (chunkPending->fccType == nifflistRiffInfo);

    /* Initialize variables */
    nBytesToWrite = strlen(str) + 1; /* Don't forget to write that NUL */

    /* Start the chunk */
    chunkInfo.fccId = fccId;
    
    if ( ! NIFFIOStorageChunkStart(&chunkInfo))
    {
        RIFFIOError(strModule, "Couldn't start chunk");
        return RIFFIO_FAIL;
    }

    /* Write out a ZSTR, (NUL terminated) */
    nBytesWritten = NIFFIOWrite(NIFFIOStorageGetFile(), 
                                (void *) str, 
                                nBytesToWrite);

    if (nBytesWritten != nBytesToWrite)
    {
        RIFFIOError(strModule, "Only wrote %d bytes out of %d",
                    nBytesWritten, nBytesToWrite);
        return RIFFIO_FAIL;
    }

    if (! NIFFIOStorageChunkEnd())
    {
        RIFFIOError(strModule, "Couldn't finalize chunk");
        return RIFFIO_FAIL;
    }
    
    return RIFFIO_OK;
}
