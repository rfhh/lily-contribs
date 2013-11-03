#ifndef lint
static char rcsid[] = 
"$Id";
#endif
/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
/**************************************************************************/
/*
 * NAME
 * ====
 * t_fcc - test RIFFIOFOURCC operations
 * 
 * SYNOPSIS
 * ========
 * 
 * t_fcc 
 *
 * DESCRIPTION
 * ===========
 * Runs various tests on RIFFIOFOURCC operations. Core dumps on failure. 
 *
 * RETURN VALUE
 * ============
 * Returns 0 if all tests pass.
 * Non-zero otherwise.
 */
/**************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "riffio.h"

int
main(int argc, char **argv)
{

   RIFFIOFOURCC fcc;                   /* This is a DWORD */ 
   char fccString[RIFFIO_FOURCC_LIM];  /* String representation of a FOURCC */

   /* Stick the four charactes in a FOURCC */
   /* Exactly the same as mmio MAKEFOURCC  */

   /* Create a valid FOURCC and convert it to a string */
   fcc = RIFFIOMAKEFOURCC('h','a','c','k'); 

   RIFFIOFOURCCToString(fcc, fccString);

   assert(RIFFIOFOURCCIsValid(fcc));
   assert(!strcmp(fccString, "hack"));

   /* Create an invalid FOURCC */
   fcc = RIFFIOMAKEFOURCC(' ', 'b', 'a', 'd');

   assert(!RIFFIOFOURCCIsValid(fcc));

   /*
    * All tests passed
    */
   return 0;
}
