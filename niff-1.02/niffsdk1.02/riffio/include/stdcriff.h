/* 
$Id: stdcriff.h,v 1.2 1996/06/06 02:12:09 tim Exp $ 
*/
/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * These routines map the Standard C Library file operations to
 * RIFFIOFile operations.
 */
#ifndef _STDRIFFH_
#define _STDRIFFH_

#include "riffio.h"


extern long STDCRIFFRead(void *userFilep, void *buffer, long n);

extern long STDCRIFFWrite(void *userFilep, void *buffer, long n);

extern RIFFIOSuccess 
STDCRIFFSeek(void *userFilep, 
             RIFFIOOffset offset, 
             RIFFIOSeekOrigin origin);

extern long 
STDCRIFFTell(void *userFilep);

#endif

