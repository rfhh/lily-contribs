#ifndef lint
static char rcsid[] =
 "$Id: rfile.c,v 1.2 1996/05/31 06:02:16 tim Exp $";
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
 * rfile - RIFFIOFile routines
 *
 * SYNOPSIS
 * ======== 
 *
 * - RIFFIOFileNew()
 * - RIFFIOFileInit()
 * - RIFFIOFileDelete()
 *
 * - RIFFIOFileGetFormType()
 *
 * - RIFFIORead()
 * - RIFFIOWrite()
 * - RIFFIOSeek()
 * - RIFFIOTell()
 *
 */
/***************************************************************************/

#include <assert.h>
#include <stdlib.h>

#include "riffio.h"

/*
 * Create some bogus callbacks to catch anyone who
 * forgets to initialize a RIFFIOFile
 */
#ifndef NDEBUG

static
long 
_bogus_reader(void *vpUserFile, void *vpBuffer, long nBytes)
{
    assert(0);
}

static
long 
_bogus_writer(void *vpUserFile, void *vpBuffer, long nBytes)
{
    assert(0);
}

static 
RIFFIOSuccess 
_bogus_seeker(void *vpUserFile, 
              RIFFIOOffset offset, 
              RIFFIOSeekOrigin origin)
{
    assert(0);
}

static
long
_bogus_teller(void *vpUserFile)
{
    assert(0);
}


#endif

/***************************************************************************/
/*
 * RIFFIOFileNew
 * =============
 * Return a pointer to a new RIFFIOFile,
 * or null on failure
 */
RIFFIOFile *
RIFFIOFileNew(void)
/*
 * OBLIGATIONS
 * -----------
 * The file cannot be operated on until RIFFIOFileInit() is called.
 */
/***************************************************************************/
{
    char strModule[]= "RIFFIOFileNew";
    RIFFIOFile *prfNew;
    
    prfNew = (RIFFIOFile *) malloc(sizeof(RIFFIOFile));
    if (prfNew == 0)
    {
        RIFFIOError(strModule, "Failed to allocate memory");
        return (RIFFIOFile *) 0;
    }
    
    /*
     * Set some traps for a user who forgets to call RIFFIOFileInit()
     */
#ifndef NDEBUG
    prfNew->read  = _bogus_reader;
    prfNew->write = _bogus_writer;
    prfNew->seek  = _bogus_seeker;
    prfNew->tell  = _bogus_teller;
#endif

    return prfNew;
}
/***************************************************************************/
/*
 * RIFFIOFileInit
 * ==============
 * Provide a NIFFIOFile with a user-defined I/O callbacks.
 */
RIFFIOSuccess
RIFFIOFileInit(RIFFIOFile *prf,
               void *pvUserFile,
               RIFFIOReader reader,
               RIFFIOWriter writer,
               RIFFIOSeeker seeker,
               RIFFIOTeller teller)
/*
 * RIFFFIOFileInit() should be called immediately after NIFFIOFileNew()
 *
 * ENTRY
 * -----
 * 
 * <prf> :
 *   is the RIFFIOFile that is being initialized.
 *
 * <pvUserFile> :
 *   points to a user representation of a file that will be passed to
 *   user-defined reader, writer, seeker, and teller functions.
 * 
 * <reader>,<writer>,<seeker>,<teller> :
 *   are user-defined functions that perform operations on <*pvUserFile>.
 */
/***************************************************************************/
{

    /* Check arguments */
    assert(prf != 0);

    /* Assign file and I/O methods */
    prf->pvUserFile = pvUserFile;

    prf->read = reader;
    prf->write = writer;
    prf->seek = seeker;
    prf->tell = teller;

    /* 
     * We don't know what the form type is
     * until we read the first (form) chunk.
     * 
     * This should _not_ be done in RIFFIOFileNew() because the 
     * user could call us more than once.
     */
    prf->formType = RIFFIO_FORM_UNKNOWN;
    
    return RIFFIO_OK;
}

/***************************************************************************/
/*
 * RIFFIOFileDelete
 * ================
 * Free the memory allocated to a RIFFIOFile.
 */
void
RIFFIOFileDelete(RIFFIOFile *prf)
/***************************************************************************/
{
    assert (prf != 0);
    
    free(prf);
    
}


/***************************************************************************/
/*
 * RIFFIOFileGetFormType
 * =====================
 * Return the form type (UNKNOWN, RIFF, or RIFX) 
 * of a RIFFIOFile
 */
RIFFIOFormType 
RIFFIOFileGetFormType(RIFFIOFile *prf)
/*
 * The form type is unknown until the first form chunk of a RIFFIOFile
 * is descended into or created.
 */
/***************************************************************************/
{
        assert(prf != 0);
        return prf->formType;
}

/***************************************************************************/
/*
 * RIFFIORead
 * ==========
 * Read bytes from a RIFF file.
 */
long 
RIFFIORead(RIFFIOFile *prf, void *pvBuffer, long nBytes)
/*
 * ENTRY
 * -----
 * T <*pvBuffer> must be allocated to hold <nBytes>.
 *
 * EXIT
 * ----
 * - Up to <nBytes> have been read from <*prf> into <*pvBuffer>. 
 *
 * RETURN
 * ------
 *     *   The number of bytes actually read,
 *     *   0 on end of file,
 *     *  -1 on error.
 */
/***************************************************************************/
{
    long result;

    /*
     * Check input arguments, make sure nothing is Null
     */
    assert(prf != 0);
    assert(prf->read != 0);
    assert(pvBuffer != 0);

    /*
     * Perform the read
     */
    result = prf->read(prf->pvUserFile, pvBuffer, nBytes);

    return result;
}

/***************************************************************************/
/*
 * RIFFIOWrite
 * ===========
 * Write bytes to a RIFF file.
 */
long 
RIFFIOWrite(RIFFIOFile *prf, void *pvBuffer, long nBytes)
/*
 * Write <nBytes> from <*pvBuffer> to <*prf>.
 *
 * RETURN
 * ------
 * The number of bytes actually written.
 */
/***************************************************************************/
{
    long bytesWritten;

    /*
     * Check input arguments, make sure nothing is Null
     */
    assert(prf != 0);
    assert(prf->write != 0);
    assert(pvBuffer != 0);

    /*
     * Perform the write.
     * It's not up to us to check the result.
     */
    bytesWritten = prf->write(prf->pvUserFile, pvBuffer, nBytes);

    return bytesWritten;

}

/***************************************************************************/
/*
 * RIFFIOSeek
 * ==========
 * Seek to a location in a RIFF file.
 */
RIFFIOSuccess
RIFFIOSeek(RIFFIOFile *prf, RIFFIOOffset offset, RIFFIOSeekOrigin origin)
/* 
 * Seek <offset> bytes relative to <origin>.
 *
 * <origin> may be one of
 *
 *     RIFFIO_SEEK_SET:
 *       beginning of file
 *
 *     RIFFIO_SEEK_CUR:
 *       current file position
 *    
 *     RIFFIO_SEEK_END:
 *       end of file
 * 
 * ERRORS
 * ------ 
 * On failure, the file position is undefined.
 */
/***************************************************************************/
{

    /*
     * Check input arguments, make sure nothing is Null
     */
    assert(prf != 0);
    assert(prf->seek != 0);
    assert(origin >= RIFFIO_SEEK_SET);
    assert(origin <= RIFFIO_SEEK_END);

    /*
     * Perform the seek 
     */
    return  prf->seek(prf->pvUserFile, offset, origin);

}

/***************************************************************************/
/*
 * RIFFIOTell
 * ==========
 * Return the current RIFF file position.
 */
long
RIFFIOTell(RIFFIOFile *prf)
/*
 * A file's position is measured in bytes from the
 * beginning of the file.
 */
/***************************************************************************/
{
    long result;

    /*
     * Check input arguments, make sure nothing is Null
     */
    assert(prf != 0);

    /*
     * Perform the tell
     */
    result = prf->tell(prf->pvUserFile);

    return result;

}





