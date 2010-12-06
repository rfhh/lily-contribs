#ifndef lint
static char rcsid[] = 
"$Id: stdcriff.c,v 1.2 1996/06/06 02:05:11 tim Exp $";
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
 * stdcriff - routines to map the Standard C library to the RIFFIOFile 
 *            interface
 *
 * SYNOPSIS
 * ========
 * 
 * - STDCRIFFRead()
 * - STDCRIFFWrite()
 * - STDCRIFFSeek()
 * - STDCRIFFTell() 
 *
 *
 * DESCRIPTION
 * ===========
 * You can provide these routines to RIFFIOFileInit() along with an
 * open Standard C Library (FILE *).
 *
 * EXAMPLES
 * ========
 * 
 * See the L<hello> example.
 * 
 */
/***************************************************************************/

#include <assert.h>
#include <stdio.h>

#include "riffio.h"
#include "stdcriff.h"


/***************************************************************************/
/*
 * STDCRIFFRead
 * ============
 * Read bytes from a Standard C library file
 */
long
STDCRIFFRead(void *pvUserFile, void *pvBuffer, long nBytes)
/***************************************************************************/
{
    long result;
    FILE *pFILE;
    
    assert(pvUserFile != 0);
    assert(pvBuffer != 0);
    assert(nBytes >= 0);
    
    pFILE = (FILE *) pvUserFile;
    
    if (feof(pFILE))
        return 0;
    
    result = fread(pvBuffer, 1, nBytes, (FILE *) pvUserFile);
    
    if (ferror(pFILE))
        return -1;
    
    return result;
}

/***************************************************************************/
/*
 * STDCRIFFWrite
 * =============
 * Write bytes to a Standard C library file
 */
long
STDCRIFFWrite(void *pvUserFile, void *pvBuffer, long nBytes)
/***************************************************************************/
{
    long result;

    assert(pvUserFile != 0);
    assert(pvBuffer != 0);
    assert(nBytes >= 0);
    
    result = fwrite(pvBuffer, 1, nBytes, (FILE *)pvUserFile);
    return result;

}

/***************************************************************************/
/*
 * STDCRIFFSeek
 * =============
 * Seek to a specific position in a Standard C library file
 */
RIFFIOSuccess
STDCRIFFSeek(void *pvUserFile, RIFFIOOffset offset, RIFFIOSeekOrigin origin)
/***************************************************************************/
{
    int stdresult; /* result of fseek */
    int stdorigin; /* stdio equivalent to origin */

    assert(pvUserFile != 0);

    switch(origin)
    {
      case RIFFIO_SEEK_SET:
          stdorigin = SEEK_SET;
          break;
            
      case RIFFIO_SEEK_CUR:
          stdorigin = SEEK_CUR;
          break;
          
      case RIFFIO_SEEK_END:
          stdorigin = SEEK_END;
          break;
      default:
          assert(0);
          
    }

    stdresult = fseek((FILE *) pvUserFile, offset, stdorigin);

    if(stdresult == 0)
        return RIFFIO_OK;
    else
        return RIFFIO_FAIL;

}

/***************************************************************************/
/*
 * STDCRIFFTell
 * ============
 * Return the file position in a Standard C library file
 */
long
STDCRIFFTell(void *pvUserFile)
/***************************************************************************/
{
    assert(pvUserFile != 0);
    return ftell((FILE *) pvUserFile);
}
