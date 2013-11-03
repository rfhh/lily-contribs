/* 
$Id: riffio.h,v 1.3 1996/06/15 02:54:49 tim Exp $ 
*/
/*
 * Public Domain 1995,1996 Timothy Butler
 *
 * THIS DOCUMENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef _RIFFIOH_
#define _RIFFIOH_

#include <stdarg.h>

/***************************************************************************/
/*
 * NAME
 * ====
 * 
 * RIFFIO API - RIFFIO data types and concepts
 * 
 * SYNOPSIS
 * ========
 *
 * The RIFFIO API provides data types and functions for reading, writing, and 
 * navigating RIFF files.  The RIFF file format is documented in
 *
 *|     Microsoft Press, "Microsoft Windows Multimedia Programmer's
 *|     Reference", Redmond WA, 1991. 
 *|
 *|     Telephone: 1-800-MSPRESS, ISBN#1-55615389-9
 *
 * Although the RIFFIO API uses similar names and terminology as Microsoft's
 * mmio API, the RIFFIO types and functions are _not_ the same as those
 * described by Microsoft.
 *
 * RIFFIO types, macros, and functions all begin with the string "RIFFIO".
 */
/***************************************************************************/

/***************************************************************************/
/*
 * RIFFIOSuccess
 * =============
 * How do you know if a RIFFIO function succeeded?
 * 
 * Many RIFFIO functions return a zero status integer if they fail.
 * This is equivalent to a false boolean value in C. In other words,
 * a test for false is a test for failure.
 *
 * To help readability the success values
 * are encoded as an enumerated type, RIFFIOSuccess.
 *
 */

typedef enum
{
  RIFFIO_FAIL = 0,      /* false, operation failed     */
  RIFFIO_OK = 1         /* true, operation successful */
} RIFFIOSuccess;

/*              
 * WARNING
 * -------
 * Never test for equality to these values. Use them only 
 * as output from a function.
 */
/***************************************************************************/

/***************************************************************************/
/*
 * RIFFIO Errors
 * =============
 * How does RIFFIO deal with errors?
 *
 * No RIFFIO function will cause a correct program to exit.  There are
 * however, many assertions in the debug version of the RIFFIO library
 * that will terminate a program if they encounter a bug in the
 * library or programmer's code.
 * 
 * Upon encountering an error beyond a programmer's control some
 * RIFFIO functions will call an error handling function.  The default
 * error handling function will print an error message to stderr
 * (although it is possible for the library to be compiled with no
 * default error handler).
 * 
 * A user may provide their own error handling function with
 * RIFFIOInstallErrorHandler().
 * 
 * Error handling functions take the form
 *
 * RIFFIOErrorHandler
 * ------------------ 
 */
typedef void (*RIFFIOErrorHandler)(const char *strModule, 
                                   const char *strFormat, 
                                   va_list args);

/*
 * A RIFFIOErrorHandler is a function that handles an error, given the
 * name of a module <strModule> where an error has occurred and a
 * message about the error.  The message consists of a printf()
 * compatible format string, <strFormat>, followed by its corresponding
 * variable argument list <args>.  
 */

/***************************************************************************/


/***************************************************************************/
/*
 * Byte Ordering
 * =============
 * Byte ordering determines how integers are stored in a RIFF file.
 *
 * There are two conventions: 
 * 
 * RIFF form (little-endian, Intel) :
 *   Least significant byte first, most significant byte last.
 * 
 * RIFX form (big-endian, Motorola) :
 *   Most significant byte first, least signifcant byte last.
 * 
 * Byte ordering affects integers only; FOURCC's and strings are
 * stored the same way in RIFF and RIFX forms.  
 *
 * TWB - I don't know if byte order affects IEEE floating point numbers.
 *
 *
 * RIFFIOFormType
 * --------------
 * How is the byte order of a RIFF file represented by RIFFIO?
 *
 * Byte order information is conveyed by RIFFIO using an enumerated
 * type, RIFFIOFormType. 
 *
 * The form type (byte order) of a RIFF file.  
 */

typedef enum
{
        RIFFIO_FORM_UNKNOWN,   /* Unknown byte order */
        RIFFIO_FORM_RIFF,      /* Intel byte order, little-endian */
        RIFFIO_FORM_RIFX       /* Motorola byte order, big-endian */
} RIFFIOFormType;

/*
 * It allows for the possiblity that the byte order has not yet
 * been determined for a RIFF file.
 * 
 * NOTE
 * ----
 * This enumerated type is not the same as a four-character code
 * and must not be used as a RIFFIOFOURCC. 
 */
/***************************************************************************/

/***************************************************************************/
/*
 * RIFFIOFile
 * ==========
 * How are RIFF files represented?
 *
 * RIFF files are represented by a pointer to a RIFFIOFile structure.
 * For example 
 *
 *| RIFFIOFile *rfNiff; 
 * 
 * There are no public members of a RIFFIOFile.
 * 
 * RIFFIOOffset
 * ------------
 * An offset into a RIFFIOFile (in bytes).
 */
typedef unsigned long RIFFIOOffset; 

/*
 * RIFFIOFile Callbacks
 * ====================
 * Upon creation, a RIFFIOFile is useless.  Before you can use a 
 * RIFFIOFile you must tell it what file it will operate on and
 * also _how_ it will perform its I/O. 
 * 
 * RIFFIOFileInit() accepts a void pointer to a user-defined object
 * that represents a "file".  The user must also provide read, write,
 * seek, and tell callbacks that are compatible with that "file".
 *
 * Subsequent RIFFIOFile operations will supply the callbacks with the
 * user-defined file pointer.
 * 
 * RIFFIOReader
 * ------------ */
typedef 
long (*RIFFIOReader)(void *vpUserFile, void *vpBuffer, long nBytes);
/*
 * A RIFFIOReader is a function that reads <nBytes> from a user-defined file 
 * <*vpUserFile> and places them at <vpBuffer>. <vpBuffer> must be large
 * enough to hold the requested number of bytes.
 *
 * RETURNS
 * 
 *     *  the number of bytes actually read,
 *     *  0 on end of file,
 *     * -1 on error.
 *
 * RIFFIOWriter
 * ------------
 */
typedef 
long (*RIFFIOWriter)(void *vpUserFile, void *vpBuffer, long nBytes);
/*
 * A RIFFIOWriter is a function that writes <nBytes> from <vpBuffer> 
 * to a user-defined file <vpUserFile>.
 *
 * RETURNS
 * 
 *     *  the number of bytes actually written, or 
 *     *  -1 on error.
 *
 * RIFFIOSeeker
 * ------------
 */

typedef enum
{
  RIFFIO_SEEK_SET, /* Seek relative to file beginning    */
  RIFFIO_SEEK_CUR, /* Seek relative to current file position */
  RIFFIO_SEEK_END  /* Seek relative to file end          */ 
} RIFFIOSeekOrigin;

typedef 
RIFFIOSuccess (*RIFFIOSeeker)(void *vpUserFile, 
                             RIFFIOOffset offset, 
                             RIFFIOSeekOrigin origin);

/*
 * A RIFFIOSeeker sets the position of a user-defined file <vpUserFile>
 * relative to <origin> by <offset> bytes.
 * It returns 0 on failure.
 *
 * RIFFIOTeller
 * ------------
 */
typedef long  (*RIFFIOTeller)(void *vpUserFile);
/*
 * A RIFFIOTeller is a function that returns the current position of
 * a user-defined file <vpUserFile>.  The file position is reported 
 * in bytes offset from the start of file.
 */
/***************************************************************************/

/***************************************************************************/
/*
 * How are RIFF four-character codes supported?
 * ============================================
 * 
 * RIFFIOFOURCC
 * ------------
 * RIFFIO stores a four-character code in a RIFFIOFOURCC type.
 */
typedef unsigned long RIFFIOFOURCC;

/*
 * RIFFIO also provides a macro to construct a RIFFIOFOURCC out
 * of four individual characters.
 * 
 * RIFFIOMAKEFOURCC
 * ----------------
 */
#define RIFFIOMAKEFOURCC(first, second, third, fourth)  \
 ( ((RIFFIOFOURCC) (unsigned char) (first)      )       \
 | ((RIFFIOFOURCC) (unsigned char) (second) << 8)       \
 | ((RIFFIOFOURCC) (unsigned char) (third)  << 16)      \
 | ((RIFFIOFOURCC) (unsigned char) (fourth) << 24))

/*
 * Notice that the first character is stored in the least significant
 * part of the RIFFIOFOURCC, regardless of machine byte order.
 *
 * Use RIFFIOFOURCCToString() to decompose a RIFFIOFOURCC into 
 * its constituent characters.
 *
 * The memory used by RIFFIOFOURCCToString() must already be allocated
 * hold at least RIFFIO_FOURCC_LIM characters.  
 * 
 * TWB - This value is bigger than 5 because I would ultimately like
 * to allow strings formatted with escape codes. Microsoft implies that
 * FOURCC's can be formatted with escape codes in
 *
 *|      Win 3.1 SDK: Multimedia Prog. Reference
 *
 * We may allow 4 * 4 character escape codes + 1 NUL
 */
#define RIFFIO_FOURCC_LIM 17

/* 
 * RIFFIO provides some commonly used four-character codes.
 */
#define RIFFIO_FOURCC_RIFF RIFFIOMAKEFOURCC('R','I','F','F')
#define RIFFIO_FOURCC_RIFX RIFFIOMAKEFOURCC('R','I','F','X')
#define RIFFIO_FOURCC_LIST RIFFIOMAKEFOURCC('L','I','S','T')

/* 
 * INFO List Chunks 
 */
#define RIFFIO_FOURCC_INFO RIFFIOMAKEFOURCC('I','N','F','O')

#define RIFFIO_FOURCC_IARL RIFFIOMAKEFOURCC('I','A','R','L')
#define RIFFIO_FOURCC_IART RIFFIOMAKEFOURCC('I','A','R','T')
#define RIFFIO_FOURCC_ICMS RIFFIOMAKEFOURCC('I','C','M','S')
#define RIFFIO_FOURCC_ICMT RIFFIOMAKEFOURCC('I','C','M','T')
#define RIFFIO_FOURCC_ICOP RIFFIOMAKEFOURCC('I','C','O','P')
#define RIFFIO_FOURCC_ICRD RIFFIOMAKEFOURCC('I','C','R','D')
#define RIFFIO_FOURCC_ICRP RIFFIOMAKEFOURCC('I','C','R','P')
#define RIFFIO_FOURCC_IDIM RIFFIOMAKEFOURCC('I','D','I','M')
#define RIFFIO_FOURCC_IDPI RIFFIOMAKEFOURCC('I','D','P','I')
#define RIFFIO_FOURCC_IENG RIFFIOMAKEFOURCC('I','E','N','G')
#define RIFFIO_FOURCC_IGNR RIFFIOMAKEFOURCC('I','G','N','R')
#define RIFFIO_FOURCC_IKEY RIFFIOMAKEFOURCC('I','K','E','Y')
#define RIFFIO_FOURCC_ILGT RIFFIOMAKEFOURCC('I','L','G','T')
#define RIFFIO_FOURCC_IMED RIFFIOMAKEFOURCC('I','M','E','D')
#define RIFFIO_FOURCC_INAM RIFFIOMAKEFOURCC('I','N','A','M')
#define RIFFIO_FOURCC_IPLT RIFFIOMAKEFOURCC('I','P','L','T')
#define RIFFIO_FOURCC_IPRD RIFFIOMAKEFOURCC('I','P','R','D')
#define RIFFIO_FOURCC_ISBJ RIFFIOMAKEFOURCC('I','S','B','J')
#define RIFFIO_FOURCC_ISFT RIFFIOMAKEFOURCC('I','S','F','T')
#define RIFFIO_FOURCC_ISHP RIFFIOMAKEFOURCC('I','S','H','P')
#define RIFFIO_FOURCC_ISRC RIFFIOMAKEFOURCC('I','S','R','C')
#define RIFFIO_FOURCC_ISRF RIFFIOMAKEFOURCC('I','S','R','F')
#define RIFFIO_FOURCC_ITCH RIFFIOMAKEFOURCC('I','T','C','H')

/***************************************************************************/

/***************************************************************************/
/*
 * How are RIFF chunks represented?
 * ================================
 *
 * RIFFIOChunk
 * -----------
 * A RIFFIOChunk structure contains type and location information 
 * about a chunk in a RIFFIOFile.
 */
typedef unsigned long RIFFIOSize;    /* Size of a RIFF chunk */

typedef struct
{
  /* Public 
   * ------
   * users need to set and read these members 
   */

  RIFFIOFOURCC     fccId;      /* chunk identifier eg. LIST for Lists  */

  RIFFIOFOURCC     fccType;    /* Form or List type eg. NIFF           
                                * Ignored for chunks that aren't lists 
                                */      

  RIFFIOSize       sizeData;   /* size of data portion of chunk 
                                * INCLUDES 4 bytes for type field of 
                                * List and Form chunks
                                */

  /* Other private members 
   * ---------------------
   * not for RIFFIO users 
   */
/***************************************************************************/
  
  RIFFIOOffset     offsetData; /* offset of data portion of chunk, 
                                * relative to file start 
                                * WARNING: for lists this is the
                                * offset of the list type, NOT
                                * the offset of its first child chunk.
                                */
  
  int              isDirty;     /* true if chunk size needs updating */
                                /* used only for debugging           */
/***************************************************************************/

} RIFFIOChunk;

/*
 * How are RIFFIOChunks Used?
 * --------------------------
 *  - RIFFIOChunks convey chunk information between the chunk
 *    creation and navigation routines.
 *
 *  - Some RIFFIOChunk routines may use only a few of the RIFFIOChunk
 *    data members and ignore others.
 * 
 *  - Some RIFFIO routines, such as RIFFIOChunkFinalize(), will expect that a
 *    RIFFIOChunk has not been modified since a previous call to an
 *    associated function (eg. RIFFIOChunkCreate).  
 */
/***************************************************************************/

/*******************************************************************
 * RIFFIOFile 
 * ========== 
 *
 * All I/O happens through a RIFFIOFile using user-provided callbacks
 * for read, write, seek, and tell operations.
 *
 *******************************************************************/
typedef struct
{
   void *pvUserFile;        /* points to user-defined "file"      */

   RIFFIOReader read;       /* callbacks for basic I/O operations */
   RIFFIOWriter write;
   RIFFIOSeeker seek;
   RIFFIOTeller tell;

   RIFFIOFormType formType; /* Is this a RIFF or RIFX file?
                             * Set when we create or descend the 
                             * corresponding form 
                             */

} RIFFIOFile;

/***********************
 * Function declarations
 ***********************/


/****************
 * Error handling
 ****************/
extern RIFFIOErrorHandler  
RIFFIOInstallErrorHandler(RIFFIOErrorHandler ehNew);

extern void
RIFFIOError(const char *strModule, const char *strFormat, ...);


/*******************
 * FOURCC operations
 *******************/

extern int  RIFFIOFOURCCIsValid(RIFFIOFOURCC fcc);
extern void RIFFIOFOURCCToString(const RIFFIOFOURCC fcc, char *str);

/******************
 * Chunk operations
 ******************/
extern int RIFFIOChunkIsList(const RIFFIOChunk *pchunk);

/*******************************
 * Chunk navigation and creation 
 *******************************/
extern RIFFIOSuccess
RIFFIOChunkAscend(RIFFIOFile *prf, const RIFFIOChunk *pchunk);

extern RIFFIOSuccess
RIFFIOChunkCreate(RIFFIOFile *prf, RIFFIOChunk *pchunk);

extern RIFFIOSuccess
RIFFIOChunkFinalize(RIFFIOFile *prf, RIFFIOChunk *pchunk);

extern RIFFIOSuccess
RIFFIOChunkDescend(RIFFIOFile *prf, RIFFIOChunk *pchunk); 

RIFFIOOffset
RIFFIOChunkDataOffset(const RIFFIOChunk *pchunk);

extern RIFFIOSuccess
RIFFIOChunkSeekData(RIFFIOFile *prf, const RIFFIOChunk *pchunk);

extern int 
RIFFIOChunkDataEnd(RIFFIOFile *prf, const RIFFIOChunk *pchunk);

extern int 
RIFFIOChunkEnd(RIFFIOFile *prf, const RIFFIOChunk *pchunk);

/*****************
 * Fundamental I/0
 *****************/
extern RIFFIOFile *
RIFFIOFileNew(void);

extern RIFFIOSuccess 
RIFFIOFileInit(RIFFIOFile *prf,
               void *userFilep,
               RIFFIOReader, 
               RIFFIOWriter, 
               RIFFIOSeeker, 
               RIFFIOTeller);

extern RIFFIOFormType 
RIFFIOFileGetFormType(RIFFIOFile *prf);

extern  long 
RIFFIORead(RIFFIOFile *prf, void *bufferp, long n); 

extern  long 
RIFFIOWrite(RIFFIOFile *prf, void *bufferp, long n);

extern  RIFFIOSuccess 
RIFFIOSeek(RIFFIOFile *prf, 
           RIFFIOOffset offset, 
           RIFFIOSeekOrigin origin);

extern  long 
RIFFIOTell(RIFFIOFile *prf);



/**************
 * Compound I/O 
 **************/
extern RIFFIOSuccess RIFFIOWrite8(RIFFIOFile *prf, unsigned char uc); 
extern RIFFIOSuccess RIFFIOWrite16(RIFFIOFile *prf, unsigned short us);
extern RIFFIOSuccess RIFFIOWrite32(RIFFIOFile *prf, unsigned long ul);
extern RIFFIOSuccess RIFFIORead8(RIFFIOFile *prf, unsigned char *ucp);
extern RIFFIOSuccess RIFFIORead16(RIFFIOFile *prf, unsigned short *usp);
extern RIFFIOSuccess RIFFIORead32(RIFFIOFile *prf, unsigned long *ulp);

extern RIFFIOSuccess RIFFIOReadFOURCC(RIFFIOFile *prf, RIFFIOFOURCC *fccp);
extern RIFFIOSuccess RIFFIOWriteFOURCC(RIFFIOFile *prf, RIFFIOFOURCC fcc);

/***************
 * FOURCC Tables
 ***************/

typedef struct
{
    RIFFIOFOURCC fcc;
    union 
    {        
        unsigned long ul;
        long  l;
        void *pv;
    } value;
} RIFFIOFCCTableEntry;

typedef struct RIFFIOPTable RIFFIOFCCTable;

extern RIFFIOFCCTable *
RIFFIOFCCTableNew(void);

extern void 
RIFFIOFCCTableDelete(RIFFIOFCCTable *ptable);

extern RIFFIOSuccess
RIFFIOFCCTableMakeEntry(RIFFIOFCCTable *ptable, RIFFIOFCCTableEntry entry);

extern RIFFIOSuccess
RIFFIOFCCTableRemoveEntry(RIFFIOFCCTable *ptable, RIFFIOFCCTableEntry entry);

extern RIFFIOSuccess 
RIFFIOFCCTableLookup(RIFFIOFCCTable *ptable, RIFFIOFCCTableEntry *entryp);

extern void
RIFFIOFCCTableForEachEntry(RIFFIOFCCTable *ptable, 
                           void f(RIFFIOFCCTableEntry) );

extern void
RIFFIOFCCTableFreeEntries(RIFFIOFCCTable *ptable);

extern unsigned
RIFFIOFCCTableCount(RIFFIOFCCTable *ptable);

extern RIFFIOFCCTableEntry *
RIFFIOFCCTableCreateArray(RIFFIOFCCTable *ptable);

#ifndef NDEBUG 
#include <stdio.h>
extern void 
RIFFIOFCCTableDump(FILE *fp, RIFFIOFCCTable *ptable);
#endif


/*****************
 * Chunk Stacks
 *****************/

typedef struct RIFFIOPChunkStack RIFFIOChunkStack;

extern RIFFIOChunkStack *
RIFFIOChunkStackNew(void);

extern void 
RIFFIOChunkStackDelete(RIFFIOChunkStack *pstack);

extern int 
RIFFIOChunkStackIsEmpty(RIFFIOChunkStack *pstack);

extern RIFFIOChunk *
RIFFIOChunkStackTop(RIFFIOChunkStack *pstack);

extern void 
RIFFIOChunkStackPop(RIFFIOChunkStack *pstack);

extern RIFFIOSuccess 
RIFFIOChunkStackPush(RIFFIOChunkStack *pstack,
                     RIFFIOChunk *pchunk);

#endif
