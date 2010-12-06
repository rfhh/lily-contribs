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
/***************************************************************************/
/*
 * NAME
 * ====
 * nif001 - Write a simple niff file
 *
 * SYNOPSIS
 * ========
 *
 * nif001
 *
 * 
/***************************************************************************/

#include <stdio.h>
#include <string.h>

#include "niffio.h"
#include "stdcriff.h"

extern void doerror(const char *strMessage);
void MyWriteStringTable(void);


int
main(int argc, char **argv)
{
    unsigned char version[8] = "6b";

    FILE          *outfp;
        
    RIFFIOSuccess success;
    NIFFIOFile    *pnf;
        
    NIFFIOStorage   *pstore;

    NIFFIOChunkLengthTable *pclt;

    RIFFIOChunk chunkForm;
    RIFFIOChunk chunkSetupSection;
    RIFFIOChunk chunkNiffInfo;
    RIFFIOChunk chunkCLT;
    RIFFIOChunk chunkPartsL;
    RIFFIOChunk chunkPart;
    RIFFIOChunk chunkPart2;
 
    niffNiffInfo info;
    niffPart part;
    STROFFSET offset;

    const char *str;

    /*   Housekeeping tasks to open a NIFF file for writing. */

    /*   Call to operating system function to open a real file for writing */

    outfp = fopen("nif001.nif", "wb");
    if (!outfp)
        doerror("Can't open nif001.nif for writing");

    /*  NIFF SDK call to allocate and initialize storage areas
        needed by the NIFF SDK for this file */

    pstore = NIFFIOStorageNewSTDC(outfp);
    if (!pstore)
        doerror("Can't create new NIFFIOStorage");
#if 0
    /* obsoleted by NIFFIOStorageNewSTDC */
    if (! NIFFIOFilePrepare(&outfp, 
                            &pnf, 
                            &pstore,
                            STDCRIFFRead, STDCRIFFWrite,
                            STDCRIFFSeek, STDCRIFFTell))
        doerror("Can't complete NIFF file preparation");
#endif
    /*   Begin writing to file.  Start with NIFF form. */

    if (! NIFFIOStartNiff())
        doerror ("NIFFIOStoreFormNiff failed");
                
    /*   Setup Section */

    if (! NIFFIOStartSetupSection())
        doerror ("NIFFIOStartSetupSection failed");

    /*  Special Chunk Length Table routine */
    if (! NIFFIOStoreDefaultCLT())
        doerror ("NIFFIOStoreDefaultCLT failed");

    /*  Other Setup Section chunks and lists */

    if (! NIFFIOchunkNiffInfo(version, progtypeNone, unitsNoUnits, -1 ,-1))
        doerror ("NIFFIOStoreListSetupSection failed");

    if (! NIFFIOStartParts())
        doerror ("NIFFIOStartParts failed");

    if (! NIFFIOchunkPart(0,0,1,2,-1,-1,-1))
        doerror ("NIFFIOchunkPart failed");

    if (! NIFFIOchunkPart(0,2,3,1,-1,-1,-1))
        doerror ("NIFFIOchunkPart failed");
    
    NIFFIOEndParts();

    if (! NIFFIOchunkStringTable())
        doerror ("NIFFIOStringTable failed");

    MyWriteStringTable();
        
    NIFFIOEndSetupSection(); /* SetupSection */
        
    NIFFIOEndNiff(); /* FormNiff  */

    NIFFIOStorageDelete(pstore);
    /* NIFFIOFileDelete(pnf); */
    fclose(outfp);

    return 0;

}

void MyWriteStringTable (void)
{
/* simplified example of how a writing program 
   creates a String Table. */
  
  char strings[256][20];
  
  NIFFIOStbl myStbl[20];
  short i;
  int nWritten;

  strcpy(strings[0], "piano");
  strcpy(strings[1], "p");
  strcpy(strings[2], "violin");
  strcpy(strings[3], "vl");

  for (i=0; i < 4; i++)
           myStbl[i].str = strings[i];

  nWritten = NIFFIOStoreStbl(myStbl, 4);
  if (nWritten != 4)
  {
      doerror("Couldn't write strings");
  }
  /*
  NIFFIOchunkStringTable(myStbl, 4);
  */
}

void
doerror(const char *strMessage)
{
        fprintf(stderr, "%s\n", strMessage);
        exit(1);
}

