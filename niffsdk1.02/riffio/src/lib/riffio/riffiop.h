/* 
$Id: riffiop.h,v 1.2 1996/06/06 02:05:07 tim Exp $ 
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
 * NAME
 * ====
 * riffiop - private RIFFIO data structures
 */
#ifndef _RIFFIOPH_
#define _RIFFIOPH_

/*
 * The length of each hash table.  Should be prime
 */
#define RIFFIOP_HASH_LENGTH 251


/*
 * RIFFIOPTableListItem
 * ====================
 * An individual element of a hash table bucket's list. 
 */
typedef struct RIFFIOPTableListItem
{
    RIFFIOFCCTableEntry          entry; /* FOURCC/value pair */
    struct RIFFIOPTableListItem *next;  /* Next item in list */
    
} RIFFIOPTableListItem;

/*
 * RIFFIOPTableBucket
 * ==================
 * Each hash table bucket points to a list of FOURCC/value pairs.
 */
typedef struct RIFFIOPTableListItem *RIFFIOPTableBucket;

/*
 * RIFFIOPTable
 * ============
 * Represents a table associating FOURCC keys with values
 * 
 * Implemented as a hash table.
 */
typedef struct RIFFIOPTable
{
    RIFFIOPTableBucket *abucket; /* An array of buckets */

} RIFFIOPTable;


#endif


