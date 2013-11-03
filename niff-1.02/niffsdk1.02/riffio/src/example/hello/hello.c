/***************************************************************************/
/*
 * NAME
 * ====
 * hello - the "hello world" program written using RIFFIO.
 *
 * SYNOPSIS
 * ========
 * 
 * hello
 *
 * DESCRIPTION
 * ===========
 *
 * Write the message "hello world" to its standard
 * output.  We won't be creating a RIFF file but we will demonstrate the 
 * RIFFIO interface.
 * 
 * BUGS
 * ====
 * Many RIFFIO functions return a zero RIFFIOSuccess value on failure.
 * The following examples ignore this result value for brevity.
 * Any real program does so at its own risk.
 *
 * We will be linking to the RIFFIO and STDCRIFF libraries,
 * so include their header files.
 */

#include "riffio.h"
#include "stdcriff.h"

int  
main()
{

/*
 * We need a variable to represent our output file.
 * RIFFIO uses pointers to RIFFIOFile structures. 
 */
    RIFFIOFile *prf;     /* File to write to    */

/*
 * <prf> doesn't point to anything yet so we need to create
 * a new RIFFIOFile and initialize it. 
 */
    prf = RIFFIOFileNew();

/*
 * RIFFIOFileNew allocates memory for a new RIFFIOFile so we must 
 * remember to free it later.
 */
    /*
     * Initialize the RIFFIOFile with I/O routines from the STDCRIFF library
     * In turn, the STDCRIFF library uses the Standard C library.
     *
     * Normally, we would have to open a FILE for writing with fopen().
     * In this case we will write to stdout, which is already open.
     */
    RIFFIOFileInit(prf,
                   stdout, 
                   STDCRIFFRead, 
                   STDCRIFFWrite, 
                   STDCRIFFSeek, 
                   STDCRIFFTell);
    
/*
 * When we initialize a RIFFIOFile we supply it with a pointer to an open
 * file object and the names of functions that perform read, write, seek,
 * and tell operations on that file.  In this case <stdout> is the pointer
 * to a Standard C FILE structure and <STDCRIFFRead>, <STDCRIFFWrite>,
 * <STDCRIFFSeek>, and <STDCRIFFTell> are library functions that operate on
 * Standard C FILE pointers.  
 *
 * STDCRIFFRead is an address of a function; also known as a "callback".
 * RIFFIO remembers the function's address so it can call the function back
 * when RIFFIO needs to read from the file. Callbacks appear in other places
 * withing the RIFFIO API.
 * 
 * If all of this seems convoluted, consider this.  The callback mechanism
 * allows you to use RIFFIO with any object that looks like a file.  In other
 * words, if you can supply a pointer to an object along with read, write,
 * seek, and tell operations on that object, then you can use RIFFIO on it.
 * 
 */
    /*
     * Hello World
     */
    RIFFIOWrite(prf, "hello world\n", 12);

/*
 * Finally we write the 12 bytes of our message to the RIFFIOFile.
 * Now we have to clean up after ourselves.
 */ 

/*
 * Virtually every time you create a RIFFIO object "xxx" with RIFFIOxxxNew you
 * will need a matching call to RIFFIOxxxDelete to free the memory allocated
 * to that object.
 */
    /*
     * Don't forget to free memory for the RIFFIOFile
     */
    RIFFIOFileDelete(prf);

/*
 * Typically we would have to close our FILE here with fclose().
 */

}

/***************************************************************************/

