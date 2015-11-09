/*
 * Internal header file: DMA functions (actually only dma_reserve() is in the
 * JUDASDMA module, rest are coded in assembler!)
 */

#define DMA_WRITE_ONESHOT 0x48
#define DMA_READ_ONESHOT 0x44
#define DMA_WRITE_LOOP 0x58
#define DMA_READ_LOOP 0x54
#define DMA_MAXSIZE 65536

extern unsigned dma_address;

int dma_reserve(int size);
void dma_program(unsigned char mode, unsigned offset, unsigned length);
unsigned dma_query();
void free_the_XXXXXXX_dma_memory(void);
