#ifndef lint
static char rcsid[] = 
"$Id: inherit.c,v 1.2 1996/06/12 07:00:13 tim Exp $";
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
 * inherit - RIFFIO-style file operations on NIFFIOFiles
 *
 * SYNOPSIS
 * ========
 * All of these routines behave just like their RIFFIO counterparts.
 *
 * - NIFFIOFileGetFormType()
 *
 * - NIFFIORead()
 * - NIFFIOWrite()
 * - NIFFIOSeek()
 * - NIFFIOTell()
 *
 * - NIFFIOChunkCreate()
 * - NIFFIOChunkFinalize()
 * - NIFFIOChunkDescend()
 * - NIFFIOChunkAscend()
 * 
 * - NIFFIOChunkDataSeek()
 * 
 * - NIFFIOChunkDataEnd()
 * - NIFFIOChunkEnd()
 *
 * - NIFFIOWrite8()
 * - NIFFIOWrite16()
 * - NIFFIOWrite32()
 * - NIFFIOWriteFOURCC()
 * - NIFFIORead8()
 * - NIFFIORead16()
 * - NIFFIORead32()
 * - NIFFIOReadFOURCC() 
 */ 
/***************************************************************************/
/* We simply cast the (NIFFIOFile *) to a (RIFFIOFile *)
 * This could have been implemented with macros, but then
 * we would have lost the type checking abilities of the compiler.
 */

#include "niffio.h"

/***************************************************************************/
/*
 * NIFFIOFileGetFormType
 * =====================
 * See RIFFIOFileGetFormType()
 */
RIFFIOFormType
NIFFIOFileGetFormType(NIFFIOFile *pnf)
/***************************************************************************/
{
    return(RIFFIOFileGetFormType((RIFFIOFile *) pnf));
}

/***************************************************************************/
/*
 * NIFFIORead
 * ==========
 * See RIFFIORead()
 */
long
NIFFIORead(NIFFIOFile *pnf, void *bufferp, long n)
/***************************************************************************/
{
    return RIFFIORead((RIFFIOFile *) pnf, bufferp, n);
}

/***************************************************************************/
/*
 * NIFFIOWrite
 * ===========
 * See RIFFIOWrite()
 */
long
NIFFIOWrite(NIFFIOFile *pnf, void *bufferp, long n)
/***************************************************************************/
{
    return RIFFIOWrite((RIFFIOFile *) pnf, bufferp, n);
}

/***************************************************************************/
/*
 * NIFFIOSeek
 * ==========
 * See RIFFIOSeek()
 */
RIFFIOSuccess
NIFFIOSeek(NIFFIOFile *pnf, RIFFIOOffset offset, RIFFIOSeekOrigin origin)
/***************************************************************************/
{
    return RIFFIOSeek((RIFFIOFile *) pnf, offset, origin);
}

/***************************************************************************/
/*
 * NIFFIOTell
 * ==========
 * See RIFFIOTell()
 */
long
NIFFIOTell(NIFFIOFile *pnf)
/***************************************************************************/
{
    return RIFFIOTell((RIFFIOFile *) pnf);
}


/***************************************************************************/
/*
 * NIFFIOChunkCreate
 * =================
 * See RIFFIOChunkCreate()
 */
RIFFIOSuccess
NIFFIOChunkCreate(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkCreate((RIFFIOFile *) pnf, pchunk);
}

/***************************************************************************/
/*
 * NIFFIOChunkFinalize
 * ===================
 * See RIFFIOChunkFinalize()
 */
RIFFIOSuccess
NIFFIOChunkFinalize(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkFinalize((RIFFIOFile *) pnf, pchunk);
}

/***************************************************************************/
/*
 * NIFFIOChunkAscend
 * =================
 * See RIFFIOChunkAscend()
 */
RIFFIOSuccess
NIFFIOChunkAscend(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkAscend((RIFFIOFile *) pnf, pchunk);
}

/***************************************************************************/
/*
 * NIFFIOChunkDescend
 * ==================
 * See RIFFIOChunkDescend()
 */
RIFFIOSuccess
NIFFIOChunkDescend(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkDescend((RIFFIOFile *) pnf, pchunk);
}
/***************************************************************************/
/*
 * NIFFIOChunkDataSeek
 * ===================
 * See RIFFIOChunkDataSeek()
 */
RIFFIOSuccess
NIFFIOChunkSeekData(NIFFIOFile *pnf, const RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkSeekData((RIFFIOFile *) pnf, pchunk);
}

/***************************************************************************/
/*
 * NIFFIOChunkDataEnd
 * ==================
 * See RIFFIOChunkDataEnd()
 */
int
NIFFIOChunkDataEnd(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkDataEnd((RIFFIOFile *) pnf, pchunk);
}

/***************************************************************************/
/*
 * NIFFIOChunkEnd
 * ==============
 * See RIFFIOChunkEnd()
 */
int
NIFFIOChunkEnd(NIFFIOFile *pnf, RIFFIOChunk *pchunk)
/***************************************************************************/
{
    return RIFFIOChunkEnd((RIFFIOFile *) pnf, pchunk);
}


/***************************************************************************/
/*
 * NIFFIOWrite8
 * ============
 * See RIFFIOWrite8()
 */
RIFFIOSuccess NIFFIOWrite8(NIFFIOFile *pnf, unsigned char uc)
/***************************************************************************/
{ return RIFFIOWrite8((RIFFIOFile *) pnf, uc); }

/***************************************************************************/
/*
 * NIFFIOWrite16
 * =============
 * See RIFFIOWrite16()
 */
RIFFIOSuccess NIFFIOWrite16(NIFFIOFile *pnf, unsigned short us)
/***************************************************************************/
{ return RIFFIOWrite16((RIFFIOFile *) pnf, us); }

/***************************************************************************/
/*
 * NIFFIOWrite32
 * =============
 * See RIFFIOWrite32()
 */
RIFFIOSuccess NIFFIOWrite32(NIFFIOFile *pnf, unsigned long ul)
/***************************************************************************/
{ return RIFFIOWrite32((RIFFIOFile *) pnf, ul); }

/***************************************************************************/
/*
 * NIFFIOWriteFOURCC
 * =================
 * See RIFFIOWriteFOURCC()
 */
RIFFIOSuccess NIFFIOWriteFOURCC(NIFFIOFile *pnf, FOURCC fcc)
/***************************************************************************/
{ return RIFFIOWriteFOURCC((RIFFIOFile *) pnf, fcc); }

/***************************************************************************/
/*
 * NIFFIORead8
 * ===========
 * See RIFFIORead8()
 */
RIFFIOSuccess NIFFIORead8(NIFFIOFile *pnf, unsigned char *puc)
/***************************************************************************/
{ return RIFFIORead8((RIFFIOFile *) pnf, puc); }

/***************************************************************************/
/*
 * NIFFIORead16
 * ============
 * See RIFFIORead16()
 */
RIFFIOSuccess NIFFIORead16(NIFFIOFile *pnf, unsigned short *pus)
/***************************************************************************/
{ return RIFFIORead16((RIFFIOFile *) pnf, pus); }

/***************************************************************************/
/*
 * NIFFIORead32
 * ============
 * See RIFFIORead32()
 */
RIFFIOSuccess NIFFIORead32(NIFFIOFile *pnf, unsigned long *pul)
/***************************************************************************/
{ return RIFFIORead32((RIFFIOFile *) pnf, pul); }

/***************************************************************************/
/*
 * NIFFIOReadFOURCC
 * ================
 * See RIFFIOReadFOURCC()
 */
RIFFIOSuccess NIFFIOReadFOURCC(NIFFIOFile *pnf, FOURCC *pfcc)
/***************************************************************************/
{ return RIFFIOReadFOURCC((RIFFIOFile *) pnf, pfcc); }




