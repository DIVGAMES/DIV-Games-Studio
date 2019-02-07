/*
 * JUDAS DMA allocation.
 */

//#include <dos.h>
//#include <conio.h>
#include "judasmem.h"
#include "osdep.h"

#define DMA_MAXSIZE 65536

int dma_reserve(int size);

unsigned dma_address;
// static union REGS glenregs;
static char dma_initialized = 0;
unsigned short judas_dma_selector;

void DebugInfo(char *Msg);
void DebugData(int Val);

int dma_reserve(int size)
{
#ifdef NOTYET
	if (dma_initialized)
		return 1;

	/* Round size upward to paragraph limit */
	size += 15;
	size &= 0xfffffff0;

	/* Limit size to maximum */
	if (size > DMA_MAXSIZE)
		size = DMA_MAXSIZE;

	/* Buffer address will be used from interrupt, lock it! */
	if (!judas_memlock(&dma_address, sizeof dma_address))
		return 0;

	/* Use DPMI functions because _dos_allocmem() doesn't work reliably */
	glenregs.w.ax = 0x100;
	glenregs.w.bx = (size * 2) >> 4;
	int386(0x31, &glenregs, &glenregs);
	if (glenregs.w.cflag)
		return 0;
	judas_dma_selector = glenregs.w.dx;
	glenregs.w.ax = 0x6;
	glenregs.w.bx = glenregs.w.dx;
	int386(0x31, &glenregs, &glenregs);
	if (glenregs.w.cflag)
		return 0;
	dma_address = glenregs.w.cx << 16 | glenregs.w.dx;

	/* Check for DMA page wrap */
	if ((dma_address & 0xffff) > (0x10000 - size)) {
		dma_address += 65535;
		dma_address &= 0xffff0000;
	}

	dma_initialized = 1;
#endif
	return 1;
}

void free_the_XXXXXXX_dma_memory(void)
{
	if (!judas_memunlock(&dma_address, sizeof dma_address)) {
		return;
	}
#ifdef NOTYET
	glenregs.w.ax = 0x101;
	glenregs.w.dx = judas_dma_selector;
	int386(0x31, &glenregs, &glenregs);
	if (glenregs.w.cflag) {
		return;
	}
#endif
	dma_initialized = 0;
}
