/*
 * JUDAS file I/O functions. Used by sample/music loading. If you
 * use a datafile system and/or compression in your program, just modify these
 * functions appropriately. JUDAS needs to do the following:
 * - open file (in readonly binary mode)
 * - read from file
 * - seek
 * - close file
 */

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

int judas_open(char *name);
int judas_seek(int handle, int bytes, int whence);
int judas_read(int handle, void *buffer, int size);
void judas_close(int handle);

/*
 * Returns nonnegative file handle if successful, -1 on error
 */
int judas_open(char *name)
{
        return open(name, O_RDONLY | O_BINARY);
}

/*
 * Returns file position after seek or -1 on error
 */
int judas_seek(int handle, int bytes, int whence)
{
        return lseek(handle, bytes, whence);
}

/*
 * Returns number of bytes actually read, -1 on error
 */
int judas_read(int handle, void *buffer, int size)
{
        return read(handle, buffer, size);
}

/*
 * Returns nothing
 */
void judas_close(int handle)
{
        close(handle);
}
