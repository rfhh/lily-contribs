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
#include <stdlib.h>
#include <string.h>

#include "stdcriff.h"
#include "niffio.h"

#include "niff/frontend.h"
#include "music/music.h"
#include "lily/xly_backend.h"


static char *Progname;

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


static void
doerror(const char *message)
{
    fprintf(stderr, "%s:ERROR:%s\n", Progname, message);
    exit(1);
}


static NIFFIOParser *
InitParser(void)
{
    NIFFIOParser *pparserNew;

    pparserNew = NIFFIOParserNew();

    nf2_frontend_init(pparserNew);
    nf2_music_init();
    xly_init();

    return pparserNew;
}


int
main(int argc, char **argv)
{

    char *strNiffFile = NULL;
    FILE *pFILENiff;
    NIFFIOFile *pnf;
    NIFFIOParser *pparser;

    long bytesRead;
    unsigned char extraByte;
    unsigned long extraCount;
    FILE       *lily_out = stdout;
    char       *lily_file = NULL;
    int         i;
    int         option;
    int         do_chording = 1;

    Progname = argv[0];

    /*
     * Check command line arguments
     */
    option = 0;
    for (i = 1; i < argc; i++) {
        if (0) {
#if VERBOSE
        } else if (strcmp(argv[i], "-v") == 0) {
            xly_verbose = 1;
#endif
        } else if (strcmp(argv[i], "-o") == 0) {
            lily_file = argv[++i];
        } else if (strcmp(argv[i], "--no-chords") == 0) {
            do_chording = 0;
        } else if (option == 0) {
            strNiffFile = argv[i];
            option++;
        } else {
			fprintf(stderr, "No such option: \"%s\"\n", argv[i]);
			fprintf(stderr, "Usage:\n");
#if VERBOSE
			fprintf(stderr, "\t-v\tverbose\n");
#endif
			fprintf(stderr, "\t-o <file>\toutput file name\n");
			fprintf(stderr, "\t--no-chords\tvoices i.s.o. chords\n");
		}
    }

    /*
     * Open the riff file
     */
    if (strNiffFile == NULL) {
        pFILENiff = stdin;
    } else {
        pFILENiff = fopen(strNiffFile, "rb");
        if (!pFILENiff)
        {
            perror("Can't open NIFF File");
            return 1;
        }

        if (lily_file == NULL) {
            char *f = strdup(strNiffFile);
            char *dot = strrchr(f, '.');
            if (dot != NULL) {
                if (strcasecmp(dot, ".nif") == 0 ||
                        strcasecmp(dot, ".niff") == 0) {
                    strcpy(dot, ".ly");
                    lily_file = f;
                    fprintf(stderr, "Output to file %s\n", lily_file);
                }
            }
        }
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

    fprintf(stderr, "Now parse niff input %s\n",
            strNiffFile == NULL ? "-" : strNiffFile);
    NIFFIOParseFile(pparser, pnf,  0, 0 );

    if (lily_file != NULL && strcmp(lily_file, "-") != 0) {
        lily_out = fopen(lily_file, "w");
        if (lily_out == NULL) {
            perror("Can't create lily file");
            return 2;
        }
    }

    fprintf(stderr, "Now generate lily output... \n");
    xly_dump(lily_out, do_chording);

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

#if 0
    NIFFIOParserDelete(pparser);

    NIFFIOFileDelete(pnf);
#endif
    fclose(pFILENiff);

    fclose(lily_out);

    fprintf(stderr, "niff2ly completed\n");

    return 0;
}
