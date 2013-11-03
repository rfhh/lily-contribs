#ifndef lint
static char rcsid[] =
"$Id: riffdump.c,v 1.2 1996/06/06 06:08:48 tim Exp $";
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
 * riffdump - dump chunk information about a riff file.
 *
 * SYNOPSIS
 * ========
 * 
 * riffdump file.rif
 *
 * 
 */
/***************************************************************************/

#include <assert.h>
#include <stdio.h>

#include "riffio.h"
#include "stdcriff.h"

static void usage();
static void dumpchunk(RIFFIOFile *prf, int indent);
static void doindent(int indent);
static void doerror(const char *message);

static char *Progname;

int
main(int argc, char **argv)
{

    char *riffFilename;
    FILE *rifffp;
    RIFFIOFile *prf;
    RIFFIOFOURCC formType;

    unsigned char extraByte;
    unsigned long extraCount;

    Progname = argv[0];

    /*
     * Check command line arguments
     */
    if (argc != 2)
    {
        usage();
        return 1;
    }
    riffFilename = argv[1];

    /*
     * Open the riff file
     */
    rifffp = fopen(riffFilename, "rb");
    if (!rifffp)
    {
        perror("Can't open RIFF File");
        return 1;
                
    }
    
    /* 
     * Initialize the riffio file
     */

    prf = RIFFIOFileNew();
    if (prf == 0)
    {
        doerror("Can't create RIFFIOFile");
    }


    if (! RIFFIOFileInit(prf, rifffp, 
                         STDCRIFFRead, STDCRIFFWrite, 
                         STDCRIFFSeek, STDCRIFFTell))
    {
        doerror("Can't initialize riff file");
        return 1;
    }
    
    /*
     * Check for a valid riff form
     */
    if (! RIFFIOReadFOURCC(prf, &formType))
        doerror("Cannot read riff form id");

    if (! ((formType == RIFFIO_FOURCC_RIFF) 
           || (formType == RIFFIO_FOURCC_RIFX)))
        doerror("File does not contain RIFF or RIFX form");
        
    /*
     * Back up to the beginning of the file
     */
    if (! RIFFIOSeek(prf, 0, RIFFIO_SEEK_SET))
        doerror("Can't seek to beginning of file");

    /*
     * Recursively dump the first chunk (should be RIFF or RIFX form)
     */
    dumpchunk(prf, 0);

    /*
     * Check for any extra bytes past the Form
     */
    extraCount = 0;
    while (RIFFIORead(prf, &extraByte , 1) == 1)
    {
        extraCount +=1;
    }
    if (extraCount != 0)
    {
        fprintf(stderr,"%s:WARNING:Found %lu extra bytes after Form\n",
                Progname,
                extraCount);
        return 1;
    }
        
        
    /*
     * Clean up
     */
    RIFFIOFileDelete(prf);
    fclose(rifffp);

    return 0;
}


/*
 * Recursively print chunks 
 */
static 
void
dumpchunk(RIFFIOFile *prf, int indent)
{
    char strFOURCC[5];
    RIFFIOChunk chunk;

    assert(prf != 0);

    /*
     * We are positioned at the start of the chunk to dump
     */
    if (! RIFFIOChunkDescend(prf, &chunk))
        doerror("Failed to read chunk");

    doindent(indent);
    printf("-------\n");
    RIFFIOFOURCCToString(chunk.fccId, strFOURCC);
    doindent(indent);
    printf("id: <%s>\n", strFOURCC);
    doindent(indent);

    printf("size: <%ld>\n", chunk.sizeData);

    if (RIFFIOChunkIsList(&chunk))
    {
        doindent(indent);
        RIFFIOFOURCCToString(chunk.fccType, strFOURCC);
        printf("type: <%s>\n", strFOURCC);

        while (! RIFFIOChunkEnd(prf, &chunk))
        {
            dumpchunk(prf, indent+1); 
        }

    }
   
    if (! RIFFIOChunkAscend(prf, &chunk))
        doerror("Failed to acscend from chunk");
}

static
void
doindent(int indent)
{
    int i;
    for (i = 0; i < indent; i++)
        printf("\t");
}

static
void
usage()
{
    fprintf(stderr, "Usage: riffdump riffile\n");
}

static
void
doerror(const char *message)
{
    fprintf(stderr, "%s:ERROR:%s\n", Progname, message);
    exit(1);
}





