/*
 * JUDAS file I/O functions. Used by sample/music loading. If you
 * use a datafile system and/or compression in your program, just modify these
 * functions appropriately. JUDAS needs to do the following:
 * - open file (in readonly binary mode)
 * - read from file
 * - seek
 * - close file
 */

//#include <stdio.h>
//#include <io.h>
//#include <fcntl.h>
#include "osdep.h"

FILE * judas_open(char *name);
int judas_seek(FILE * handle, int bytes, int whence);
int judas_read(FILE * handle, void *buffer, int size);
void judas_close(FILE * handle);

/*
 * Returns nonnegative file handle if successful, -1 on error
 */
FILE * judas_open(char *name)
{
        return fopen(name, "rb");//O_RDONLY | O_BINARY);
}

/*
 * Returns file position after seek or -1 on error
 */
int judas_seek(FILE * handle, int bytes, int whence)
{
        return fseek( handle,bytes, SEEK_CUR);
}

/*
 * Returns number of bytes actually read, -1 on error
 */
int judas_read(FILE * handle, void *buffer, int size)
{
        return fread(buffer,1,size,handle);
}

/*
 * Returns nothing
 */
void judas_close(FILE * handle)
{
        fclose(handle);
}
