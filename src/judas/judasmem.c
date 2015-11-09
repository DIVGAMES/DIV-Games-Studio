/*
 * JUDAS DPMI memory locking functions. Includes also functions to
 * allocate/free locked memory. Feel free to use in your own proggys.
 */

#include <stdlib.h>
//#include <dos.h>
#include <malloc.h>
#include "osdep.h"

int judas_memlock(void *start, unsigned size);
int judas_memunlock(void *start, unsigned size);
void *locked_malloc(int size);
void locked_free(void *address);

/*
 * We allocate 4 bytes more and store the block size before the actual block
 * passed to user. Afterwards, when freeing, we know the size (needed in
 * unlocking!)
 */
void *locked_malloc(int size)
{
        unsigned *block;

        _heapshrink();

        block = malloc(size + sizeof(unsigned));
        if (!block) return NULL;
        *block = size;
        if (!judas_memlock(block + 1, size))
        {
                free(block);
                return NULL;
        }
        return block + 1;
}

void locked_free(void *address)
{
        unsigned *block;

        if (!address) return;
        block = (unsigned *)address - 1;
        judas_memunlock(block + 1, *block);
        free(block);

        _heapshrink();
}

int judas_memlock(void *start, unsigned size)
{
#ifdef NOTYET
        union REGS glenregs;

        glenregs.w.ax = 0x600;
        glenregs.w.bx = (unsigned)start >> 16;
        glenregs.w.cx = (unsigned)start & 0xffff;
        glenregs.w.si = size >> 16;
        glenregs.w.di = size & 0xffff;
        int386(0x31, &glenregs, &glenregs);
        if (glenregs.w.cflag) return 0;
#endif
        return 1;
}

int judas_memunlock(void *start, unsigned size)
{
#ifdef NOTYET
        union REGS glenregs;

        glenregs.w.ax = 0x601;
        glenregs.w.bx = (unsigned)start >> 16;
        glenregs.w.cx = (unsigned)start & 0xffff;
        glenregs.w.si = size >> 16;
        glenregs.w.di = size & 0xffff;
        int386(0x31, &glenregs, &glenregs);
        if (glenregs.w.cflag) return 0;
#endif
        return 1;
}
