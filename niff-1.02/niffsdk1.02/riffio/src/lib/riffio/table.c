#ifndef lint
static char rcsid[] = 
"$Id: table.c,v 1.2 1996/06/06 02:05:09 tim Exp $";
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
 * table - Maintain a table of FOURCC/value pairs
 *
 * SYNOPSIS
 * ========
 * 
 * - RIFFIOFCCTableNew()
 * - RIFFIOFCCTableDelete()
 * - RIFFIOFCCTableMakeEntry()
 * - RIFFIOFCCTableLookup()
 * - RIFFIOFCCTableCount()
 * - RIFFIOFCCTableCreateArray()
 * - RIFFIOFCCTableFreeEntries()
 * - RIFFIOFCCTableForEachEntry()
 * - RIFFIOFCCTableDump()
 */
/***************************************************************************/

#include <assert.h>
#include <stdlib.h>

#include "riffio.h"
#include "riffiop.h"

static unsigned _HashFunction(RIFFIOFOURCC fcc);
/***************************************************************************/
/*
 * RIFFIOFCCTableNew
 * =================
 * Return a newly created table.
 */
RIFFIOFCCTable *
RIFFIOFCCTableNew(void)
/* OBLIGATIONS
 * -----------
 * Use RIFFIOFCCTableDelete to free the table.
 * 
 * RETURNS
 * -------
 * The new table or null on failure.
 */
/***************************************************************************/
{
    RIFFIOFCCTable *ptableNew;      /* newly created hash table to return */
    int i;                          /* index into hash table */

    ptableNew = (RIFFIOPTable *) malloc(sizeof(struct RIFFIOPTable));
    if (ptableNew == 0)
        return (RIFFIOPTable *) 0;
    
    /*
     * Allocate memory for an empty hash table
     * Each bucket holds a pointer to the first list item.
     */
    ptableNew->abucket = (RIFFIOPTableBucket *) 
        malloc(RIFFIOP_HASH_LENGTH * sizeof(RIFFIOPTableBucket *)); 
    if  (ptableNew->abucket == 0)
    {
        free (ptableNew);
        return (struct RIFFIOPTable *) 0 ;
    }
    
    /*
     * Initialize all buckets to empty lists
     */
    for (i = 0; i < RIFFIOP_HASH_LENGTH; i++)
        ptableNew->abucket[i] = (RIFFIOPTableListItem *) 0;
    
    return ptableNew;


}

/***************************************************************************/
/*
 * RIFFIOFCCTableDelete
 * ====================
 * Free all memory allocated for a table.
 */
void
RIFFIOFCCTableDelete(RIFFIOFCCTable *ptable)
/* 
 * WARNING
 * -------
 * Orphans all table entry values if they happen to be pointers.
 *
 * ENTRY
 * -----
 * <*ptable> must have been created by RIFFIOFCCTableCreate.
 */
/***************************************************************************/
{
    
    RIFFIOPTableListItem *thisListItemp; /* points to entry about 
                                            to be freed  */
    RIFFIOPTableListItem *nextListItemp; /* points to entry 
                                            _after_ thisListItemp */
    
    int i; /* index into buckets */
    
    assert (ptable != 0);
    assert (ptable->abucket != 0);
    
    /*
     * Free every list item in each bucket's list
     */
    for (i = 0; i < RIFFIOP_HASH_LENGTH; i++)
    {
        /* 
         * Walk the list, freeing each list item as we go 
         */
        
        for (thisListItemp = ptable->abucket[i]; /* beginning of the list */
             thisListItemp != 0;                 /* not at end of list    */
             thisListItemp = nextListItemp)      /* advance to next entry */
        {
            /*
             * Save next entry _before_ we free this entry
             */
            nextListItemp = thisListItemp->next;  
            free(thisListItemp);
            
        }
        
    }
    
    /*
     * Free the buckets themselves
     */

    free(ptable->abucket);
        
    /* 
     * Free the table structure itself
     */
    free(ptable);
}

static
unsigned
_HashFunction(RIFFIOFOURCC fcc)
{
    return fcc % RIFFIOP_HASH_LENGTH;
}

/***************************************************************************/
/*
 * RIFFIOFCCTableMakeEntry
 * =======================
 * Make a new entry in a table.
 */
RIFFIOSuccess
RIFFIOFCCTableMakeEntry(RIFFIOFCCTable *ptable, RIFFIOFCCTableEntry newEntry)
/*
 * Replaces any existing entry with the value of the new entry.
 * 
 * RETURN
 * ------
 * Return the success status of the operation.
 */
/***************************************************************************/
{

    struct RIFFIOPTableListItem *listItemp; /* points to each item  
                                             * in linked list
                                             */
    RIFFIOPTableListItem *newListItemp;   /* pointer to a newly allocated 
                                           * list item  
                                           */
    
    assert(ptable != 0);
    assert(ptable->abucket != 0);
    
    /* 
     * Lookup bucket in hash table
     */
    listItemp = ptable->abucket[_HashFunction(newEntry.fcc)];
    
    /* 
     * Handle the special case where the linked list is empty
     */
    if (listItemp == 0)
    {
        /*
         * Allocate a new list item
         */
        newListItemp = (struct RIFFIOPTableListItem *) 
            malloc (sizeof (RIFFIOPTableListItem));
        if (newListItemp == 0)
            return RIFFIO_FAIL;
        
        /*
         * Fill in the new list item's  values
         */
        newListItemp->entry = newEntry;
        newListItemp->next = 0;
        
        /*
         * Fill in the old empty hash table slot
         */
        ptable->abucket[_HashFunction(newEntry.fcc)] = newListItemp;
        
        /* We are done with the special case of an empty list */
        return RIFFIO_OK;
    }
    
    
    /*
     * The bucket already has a non-empty linked list
     */  
    
    /*
     * Try to find an existing entry to replace
     * by scanning the list.

     * We will inevitably either
     *   - find a match, or
     *   - reach the end of the list
     * In either case we _will_ exit the while loop by
     * "return"ing from this function.
     */
    
    while ( (listItemp->entry).fcc != newEntry.fcc /* check for match       */
            && (listItemp->next != 0) )         /* AND check for end of list */
    {
        /* Advance to next list item */
        listItemp = listItemp->next;           
    }
    
    /*
     * Did we find a matching fcc?
     */
    if ((listItemp->entry).fcc == newEntry.fcc)
    {
        /*
         * We have we found a matching fcc.
         * Replace its old value with our new one
         */
        ((listItemp->entry).value) = newEntry.value;
        
        /*
         * We are done, we have replaced an existing entry 
         */
        return RIFFIO_OK;
    }
    
    
    /*
     * Did we reach the end of the list?
     */
    if (listItemp->next == 0)
    {
        /* 
         * Yes, Append new list item
         */
        
        /*
         * Allocate memory for the new list item                
         */
        newListItemp = (struct RIFFIOPTableListItem *)
            malloc(sizeof(RIFFIOPTableListItem));
        if (newListItemp == 0)
            return RIFFIO_FAIL;
        
        /*
         * Fill in the new list item
         */
        newListItemp->entry = newEntry; 
        newListItemp->next = 0;         /* we are at end of list */
        
        /*
         * Add reference from last item in list
         */
        listItemp->next = newListItemp;
        
        /* 
         * We are finished adding a new list item to the table
         */ 
        return RIFFIO_OK;
        
    }
    
    /* 
     * We should never get here
     * We must either find a match or reach the end of a bucket's list
     */
    assert(0);
    /*NOTREACHED*/
    
}

/***************************************************************************/
/*
 * RIFFIOFCCTableLookup
 * ====================
 * Lookup a value in a RIFFIOFCCTable given a FOURCC.
 */
RIFFIOSuccess
RIFFIOFCCTableLookup(RIFFIOFCCTable *ptable, RIFFIOFCCTableEntry *entryp)
/***************************************************************************/
{
    struct RIFFIOPTableListItem *listItemp;
    
    assert(ptable != 0);
    assert(ptable->abucket != 0);
    assert(entryp != 0);
    
    /* 
     * 
     * Initialize listItemp to the beginning of the bucket list
     * Scan the bucket list to its end for a matching FOURCC
     */
    for (listItemp = ptable->abucket[_HashFunction(entryp->fcc)];/*list start*/
         listItemp != 0;                /* not at end of list */
         listItemp = listItemp->next)   /* advance to next list item */
    {
        
        /*
         * Have we found a match?
         */
        if ((listItemp->entry).fcc == (entryp->fcc))
        {
            /* Yes, fill in the entryp */
            entryp->value = ((listItemp->entry).value);
            
            /* We are done, FOUND IT */
            return RIFFIO_OK;
        }
    }
    
    /*
     * We are done, DID NOT find matching FOURCC
     */
    return RIFFIO_FAIL;
    
}


/***************************************************************************/
/*
 * RIFFIOFCCTableCount
 * ===================
 * Return the number of entries in a table
 */
unsigned
RIFFIOFCCTableCount(RIFFIOFCCTable *ptable)
/***************************************************************************/
{

        unsigned count; /* counts each entry */ 
        int i;          /* index into buckets */
        RIFFIOPTableListItem *listItemp; /* point to each list Item in each list */
   
        assert(ptable != 0);
        assert(ptable->abucket != 0);

        count = 0;
        for (i = 0; i < RIFFIOP_HASH_LENGTH; i++)
        {
                for(listItemp = ptable->abucket[i]; /* beginning of list */
                        listItemp != 0;                 /* not at end of list */
                        listItemp = listItemp->next)    /* advance to the next list item */
                {
                        count++;
                }
        }
        
        return count;
}

/*
 * Treat all the entries in a table like pointers and 
 * free them all.
 */
void
RIFFIOFCCTableFreeEntries(RIFFIOFCCTable *ptable)
{

        int i;                           /* index into buckets */
        RIFFIOPTableListItem *listItemp; /* point to each list Item in each list */
   
        assert(ptable != 0);
        assert(ptable->abucket != 0);

        for (i = 0; i < RIFFIOP_HASH_LENGTH; i++)
        {
                for(listItemp = ptable->abucket[i]; /* beginning of list */
                        listItemp != 0;                 /* not at end of list */
                        listItemp = listItemp->next)    /* advance to the next list item */
                {
                        free(listItemp->entry.value.pv);
                }
        }
        
}

/***************************************************************************/
/*
 * RIFFIOFCCTableCreateArray
 * =========================
 * Return a newly allocated array of all the entries in a table.
 */
RIFFIOFCCTableEntry *
RIFFIOFCCTableCreateArray(RIFFIOFCCTable *ptable)
/*
 * EXIT
 * ----
 * The new array is NOT sorted in any particular order.
 *
 * OBLIGATIONS
 * -----------
 * The new array must be freed by the caller. 
 */
/***************************************************************************/
{
    unsigned count;             /* total number of entries in table */
    RIFFIOFCCTableEntry *array; /* array to return */
    unsigned i;                 /* index into array */
    
    int iabucket;                     /* index into buckets */
    RIFFIOPTableListItem *listItemp;  /* points to each list item */
    
    count = RIFFIOFCCTableCount(ptable);
    
    /*
     * Allocate memory for the new array
     */
    array = (RIFFIOFCCTableEntry *) 
        malloc(count * sizeof(RIFFIOFCCTableEntry));
    if (array == 0)
        return 0;
    
    /*
     * Iterate over each entry in the table 
     */
    
    i = 0;      
    for (iabucket = 0; iabucket < RIFFIOP_HASH_LENGTH; iabucket++)
    {
        for(listItemp = ptable->abucket[iabucket];   /* beginning of list */
            listItemp != 0;              /* not at end of list */
            listItemp = listItemp->next) /* advance to the next list item */
        {
            /* 
             * Copy the table entry into the array
             */
            array[i] = listItemp->entry;
            i++;
        }
    }
    
    return array;
    
}

/***************************************************************************/
/*
 * RIFFIOFCCTableForEachEntry
 * ===========================
 * Apply a function over every entry in a table.
 */
void
RIFFIOFCCTableForEachEntry(RIFFIOFCCTable *ptable, void f(RIFFIOFCCTableEntry))
/*
 * ENTRY
 * -----
 * <f> must not modify, add or delete any entries
 */
/***************************************************************************/
{
    int i;                      /* index into buckets */
    RIFFIOPTableListItem *listItemp; /* point to each list Item in each list */
    
    assert(ptable != 0);
    assert(ptable->abucket != 0);
    
    for (i = 0; i < RIFFIOP_HASH_LENGTH; i++)
    {
        for(listItemp = ptable->abucket[i];   /* beginning of list */
            listItemp != 0;                   /* not at end of list */
            listItemp = listItemp->next) /* advance to the next list item */
        {
            /* Apply the function */
            f(listItemp->entry);
        }
    }
}

#ifndef NDEBUG
/***************************************************************************/
/*
 * RIFFIOFCCTableDump
 * ==================
 * Print the contents of a RIFFIOFCCTable for debugging
 */
void
RIFFIOFCCTableDump(FILE *fp, RIFFIOFCCTable *ptable)
/***************************************************************************/
{

    int i;                           /* index into buckets */
    RIFFIOPTableListItem *listItemp; /* points to each list item */
    
    assert(fp != 0);
    assert(ptable != 0);
    assert(ptable->abucket != 0);
    
    for (i = 0; i < RIFFIOP_HASH_LENGTH; i++)
    {
        fprintf(fp, "bucket[%d]:\n", i);
        
        for(listItemp = ptable->abucket[i];   /* start of list */
            listItemp != 0;                   /* not at end of list */
            listItemp = listItemp->next)      /* advance to next list item */
        {
            char fccString[5];
            RIFFIOFOURCCToString((listItemp->entry).fcc,fccString);
            fprintf(fp, "\t%s\n", fccString);
        }
    }
}
#endif
