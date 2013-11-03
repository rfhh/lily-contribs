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
 * tststack - test RIFFIO stack operations
 * 
 * SYNOPSIS
 * ========
 * 
 * tststack 
 *
 * DESCRIPTION
 * ===========
 * Runs various tests on RIFFIOChunkStack operations. Core dumps on failure. 
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

/*
 * iToFCC 
 * ======
 * Create a FOURCC from an integer
 * 
 * The new FOURCC will be four ascii digits representing the integer
 * (mod 10000).
 */
RIFFIOFOURCC
iToFCC(int i)
{
    char strFOURCC[RIFFIO_FOURCC_LIM];
    RIFFIOFOURCC fcc;
    
    i = i % 10000;

    sprintf(strFOURCC, "%04d", i);
    
    fcc = RIFFIOMAKEFOURCC(strFOURCC[0], 
                           strFOURCC[1], 
                           strFOURCC[2], 
                           strFOURCC[3]);

    return fcc;

}

int
main(int argc, char **argv)
{

    RIFFIOChunkStack *pstack; 

    RIFFIOChunk chunk;

    int nFillUp = 2; /* Fill up the chunk stack more than once */
    int j;
    int i;            

    /*
     * Newly created stacks should be empty
     */

    pstack = RIFFIOChunkStackNew();
    assert(pstack);

    assert(RIFFIOChunkStackIsEmpty(pstack));

    for (j = 0; j < nFillUp; j++)
    {
        /* Push a bunch of chunks on the stack */
        for (i = 0; i < 100; i++)
        {
            chunk.fccId = iToFCC(i);
            
            RIFFIOChunkStackPush(pstack, &chunk);
        }
        
        /* Pop the chunks */
        for (i = 99; i >= 0; i --)
        {
            chunk = *RIFFIOChunkStackTop(pstack);
            RIFFIOChunkStackPop(pstack);
            
            assert (chunk.fccId == iToFCC(i));
            
        }

        assert(RIFFIOChunkStackIsEmpty(pstack));
        
    }

    RIFFIOChunkStackDelete(pstack);

    return 0;

}
