/***************************************************************************/
/*
 * NAME
 * ====
 * niffdump - display the contents of a niff file
 *
 * SYNOPSIS
 * ========
 * niffdump <file.nif>
 *
 * DESCRIPTION
 * ===========
 * The program is still under construction.
 * Right now it is just barely useful for viewing a niff file.
 * It will eventually print the niff file in a format that is compatible with
 * a future niff compiler.
 *   
 */
/***************************************************************************/


#include <assert.h>
#include <stdio.h>

#include "stdcriff.h"
#include "niffio.h"

extern NIFFIOParser *InitParser(void);
extern void doindent(unsigned indent);

static void usage();
static void dumpchunk(RIFFIOFile *rioFilep, int indent);
static void doerror(const char *message);

static char *Progname;

int
main(int argc, char **argv)
{

    char *strNiffFile;
    FILE *pFILENiff;
    NIFFIOFile *pnf;
    RIFFIOFOURCC fccFormType;
    NIFFIOParser *pparser;

    long bytesRead;
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
    strNiffFile = argv[1];

    /*
     * Open the riff file
     */
    pFILENiff = fopen(strNiffFile, "rb");
    if (!pFILENiff)
    {
        perror("Can't open NIFF File");
        return 1;

    }
    
    /* 
     * Initialize the niffio file
     */

    pnf = NIFFIOFileNewSTDC(pFILENiff);
    if (!pnf)
        doerror("Can't create new NIFFIOFile");

#if 0
    pnf = NIFFIOFileNew();
    if (pnf == 0)
    {
        doerror("Can't allocate NIFFIOFile");
    }

    if (! NIFFIOFileInit(pnf, pFILENiff, 
                         STDCRIFFRead, STDCRIFFWrite, 
                         STDCRIFFSeek, STDCRIFFTell))
    {
        doerror("Can't initialize NIFFIOFile");
    }
#endif
    /*
     * Initialize the parser 
     */
    pparser = InitParser();

    NIFFIOParserSetTracing(pparser, 0);

    NIFFIOParseFile(pparser, pnf,  0, 0 );

    /*
     * Check for any extra bytes past the Form
     */
    extraCount = 0;

    while ((bytesRead = NIFFIORead(pnf, &extraByte , 1)) == 1)
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

    NIFFIOParserDelete(pparser);

    NIFFIOFileDelete(pnf);
    fclose(pFILENiff);

    return 0;
}




void
doindent(unsigned indent)
{
    int i;
    
    /* Don't indent levels 0 and 1 */
    if ((--indent) <= 0 )
        return;
    
    indent *= 4; /* four spaces per indent level */
    for (i = 0; i < indent; i++)
        printf(" ");
}

static
void
usage()
{
    fprintf(stderr, "Usage: niffdump file.nif\n");
}

static
void
doerror(const char *message)
{
    fprintf(stderr, "%s:ERROR:%s\n", Progname, message);
    exit(1);
}
