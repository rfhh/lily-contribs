#ifndef lint
static char rcsid[] = 
"$Id: stack.c,v 1.2 1996/06/06 02:05:08 tim Exp $";
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
 * stack - maintain a stack of chunks.
 *
 * SYNOPSIS
 * ========
 *
 * - RIFFIOChunkStackNew()
 * - RIFFIOChunkStackDelete()
 * - RIFFIOChunkStackPush()
 * - RIFFIOChunkStackPop()
 * - RIFFIOChunkStackTop()
 * - RIFFIOChunkStackIsEmpty()
 */
/***************************************************************************/

#include <assert.h>
#include <stdlib.h>

#include "riffio.h"

/*
 * _nChunksToAllocate
 * ==================
 * The number of chunks to allocate for a newly created stack
 */
static 
const int  _nChunksToAllocate = 10;

/*
 * RIFFIOPChunkStack
 * =================
 * Private representation of a stack of chunks.
 * 
 * The stack is represented with an array.  The stack grows from
 * low array indicies to high indicies.
 *
 * A RIFFIOChunkStack owns its chunks.
 */
typedef struct RIFFIOPChunkStack
{
    
    RIFFIOChunk *pchunks; /* An array of chunks                      */
    int         size;     /* Size of the allocated array <pchunks>   */
    int         top;      /* The index of the top chunk, -1 if empty */
    
} RIFFIOPChunkStack;


/*
 * RIFFIOPChunkStackReallocate
 * ===========================
 * Resize a stack's private array of chunks.
 */
static 
RIFFIOSuccess
RIFFIOPChunkStackReallocate(RIFFIOChunkStack *pstack, int sizeNew)
/*
 * 
 */
{

    RIFFIOChunk *pchunksNew; /* points to newly allocated memory */

    assert (pstack);
    assert (sizeNew >= 0);
    assert (sizeNew >= pstack->top);
    
   
    /* Realloc() an existing array,
     * Malloc() a new array
     */
    if (pstack->pchunks)
        pchunksNew = (RIFFIOChunk *) 
            realloc(pstack->pchunks, 
                    sizeNew * sizeof(RIFFIOChunk));
    else
        pchunksNew = (RIFFIOChunk *)
            malloc(sizeNew * sizeof(RIFFIOChunk));
    
    if (! pchunksNew)
        return RIFFIO_FAIL;
    
    pstack->pchunks = pchunksNew;
    pstack->size = sizeNew;
    
    return RIFFIO_OK;
    
}

/***************************************************************************/
/*
 * RIFFIOChunkStackNew
 * ===================
 * Return an empty RIFFIOChunkStack.
 */
RIFFIOChunkStack *
RIFFIOChunkStackNew(void)
/*
 * RETURN
 * ------
 * NULL on failure
 * 
 * OBLIGATIONS
 * -----------
 * The newly created RIFFIOChunkStack must be freed with
 * RIFFIOChunkStackDelete().
 */
/***************************************************************************/
{
    RIFFIOChunkStack *pstack;
        
    pstack = (RIFFIOChunkStack *) malloc(sizeof(RIFFIOChunkStack));
    if (! pstack)
        return 0;

    pstack->pchunks = 0;
    pstack->top = -1;
    pstack->size = 0;

    if (! RIFFIOPChunkStackReallocate(pstack, _nChunksToAllocate))
    {
        free (pstack);
        return 0;
    }
    

    return pstack;

}

/***************************************************************************/
/*
 * RIFFIOChunkStackDelete
 * ======================
 * Free memory allocated to a RIFFIOChunkStack.
 */
void
RIFFIOChunkStackDelete(RIFFIOChunkStack *pstack)
/***************************************************************************/
{
    assert (pstack);
    assert (pstack->pchunks);
    
    free (pstack->pchunks);
    free (pstack);
    
}

/***************************************************************************/
/*
 * RIFFIOChunkStackIsEmpty
 * =======================
 * Return true if a RIFFIOChunkStack is empty.
 */
int
RIFFIOChunkStackIsEmpty(RIFFIOChunkStack *pstack)
/***************************************************************************/
{
    assert(pstack);
    
    return (pstack->top == -1);
}

/***************************************************************************/
/*
 * RIFFIOChunkStackTop
 * ===================
 * Peek at top of the stack without popping.
 */
RIFFIOChunk *
RIFFIOChunkStackTop(RIFFIOChunkStack *pstack)
/***************************************************************************/
{

    assert (pstack);
    assert (! RIFFIOChunkStackIsEmpty(pstack));
    
    return &((pstack->pchunks)[pstack->top]);
}

/***************************************************************************/
/*
 * RIFFIOChunkStackPop
 * ===================
 * Pop the top item of the RIFFIOChunkStack.
 */
void
RIFFIOChunkStackPop(RIFFIOChunkStack *pstack)
/*
 * ENTRY
 * -----
 * T <*pstack> must not be empty.
 */
/***************************************************************************/
{
    assert (pstack);
    assert (! RIFFIOChunkStackIsEmpty(pstack));
    
    --(pstack->top);
    
}

/***************************************************************************/
/*
 * RIFFIOChunkStackPush
 * ====================
 * Push a chunk on the top of the stack
 */
RIFFIOSuccess
RIFFIOChunkStackPush(RIFFIOChunkStack *pstack,
                     RIFFIOChunk *pchunk)
/*
 * OBLIGATIONS
 * -----------
 * You must still keep track of the memory occupied by <*pchunk>.
 * The RIFFIOChunkStack will make its own copy of <*pchunk>.
 */
/***************************************************************************/
{
    assert (pstack);
    assert (pchunk);
    
    /*
     * Do we need to allocate more chunks?
     */
    if (pstack->size == pstack->top + 1)
    {
        /* Yes */
        if (! RIFFIOPChunkStackReallocate(pstack, 
                                          pstack->size + _nChunksToAllocate))
            return RIFFIO_FAIL;
    }
    
    ++(pstack->top);
    (pstack->pchunks)[pstack->top] = *pchunk;
    
    return RIFFIO_OK;
    
}






