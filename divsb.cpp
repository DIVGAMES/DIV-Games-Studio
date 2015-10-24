
//------------------------------------------------------------------------------
// Copyright (c), David Welch, 1993
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <mem.h>
#include <conio.h>
#include "judas\judas.h"

void dspwrite ( unsigned char );
unsigned char dspread ( void );

unsigned char  *aligned[2] = { NULL, NULL };
unsigned short dma_selector[2];
unsigned char  aligned_physical[2];
unsigned char  DmaBuf=0;
unsigned char  DmaNum, DmaCha;

unsigned char dma_adr[8]  = {    0,    2,    4,    6, 0xc0, 0xc4, 0xc8, 0xcc };
unsigned char dma_cnt[8]  = {    1,    3,    5,    7, 0xc2, 0xc6, 0xca, 0xce };
unsigned char dma_page[8] = { 0x87, 0x83, 0x81, 0x82, 0x8f, 0x8b, 0x89, 0x8a };
unsigned char dma_mask[8] = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
unsigned char dma_mode[8] = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
unsigned char dma_clr[8]  = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

//------------------------------------------------------------------------------
void dspwrite ( unsigned char c )
{
    while(inp(judascfg_port+0x0C)&0x80);
    outp(judascfg_port+0x0C,c);
}
//------------------------------------------------------------------------------
unsigned char dspread ( void )
{
    while(!(inp(judascfg_port+0x0E)&0x80));
    return(inp(judascfg_port+0x0A));
}
//------------------------------------------------------------------------------
int sbinit ( void )
{
    unsigned short x;

    DmaNum = judascfg_dma1, DmaCha = (judascfg_dma1&3);
//    DebugData(judascfg_dma1);

    inp(judascfg_port+0x0E);
    outp(judascfg_port+0x06,0x01);
    inp(judascfg_port+0x06);
    inp(judascfg_port+0x06);
    inp(judascfg_port+0x06);
    inp(judascfg_port+0x06);
    outp(judascfg_port+0x06,0x00);
    for(x=0;x<100;x++)
    {
        if(inp(judascfg_port+0x0E)&0x80)
        {
            if(inp(judascfg_port+0x0A)==0xAA) break;
        }
    }

    if(x==100)
    {
//      printf("Sound Blaster not found.\n"); // OJO !!!
//      exit(1);
        return(0);
    }
    return(1);
}
//------------------------------------------------------------------------------
void sbmalloc ( void )
{
  union    REGS r;
  unsigned char *data;
  unsigned int  physical;
  unsigned int  Tam;
  unsigned int  con;

  Tam=65536*2;

  for(con=0; con<2; con++)
  {
  	memset(&r, 0, sizeof(r));
  	r.w.ax = 0x0100;				  // Funci¢n Reservar Bloque Mem. DOS
  	r.w.bx = (Tam + 15) >> 4; // Parrafos a reservar
  	int386 (0x31, &r, &r);
    if (r.x.cflag)
    {
      sbfree();
      return;
    }
    data              = (unsigned char *)r.w.ax;
    dma_selector[con] = r.w.dx;
    if(data==NULL)
    {
      sbfree();
      return;
    }

    physical=((unsigned int)data & 0xFFFF) << 4;
    physical+=0x0FFFFL;
    physical&=0xF0000L;
    aligned_physical[con]=(physical>>16)&15;
    aligned[con]=(unsigned char *)(((unsigned int)aligned_physical[con]<<16)&0xF0000);
  }
}
//------------------------------------------------------------------------------
void sbfree(void)
{
	union	REGS	regs;
  int         con;

  for(con=0; con<2; con++)
  {
    if(aligned[con])
    {
    	regs.w.ax = 0x101;
	    regs.w.dx = dma_selector[con];
  	  int386(0x31,&regs,&regs);
      aligned[con]=NULL;
    }
  }
}
//------------------------------------------------------------------------------
void sbsettc ( unsigned char tc )
// tc = time constant = 256L - (1000000UL/samples per second)
{
    inp(judascfg_port+0x0E);
    dspwrite(0x40);
    dspwrite(tc);
}
//------------------------------------------------------------------------------
void spkon ( void )
{
    dspwrite(0xD1);
}
//------------------------------------------------------------------------------
void spkoff ( void )
{
    dspwrite(0xD3);
}
//------------------------------------------------------------------------------
void sbrec ( unsigned short len )
// len = number of bytes to record to unsigned char *aligned (<=65000)
{
    len--;
    outp(dma_mask[DmaNum],4+DmaCha);
    outp(dma_clr[DmaNum],0x00);
    outp(dma_mode[DmaNum],0x44+DmaCha);
    outp(dma_adr[DmaNum],0);
    outp(dma_adr[DmaNum],0);
    outp(dma_cnt[DmaNum],(unsigned char)(len&0xFF));
    outp(dma_cnt[DmaNum],(unsigned char)((len>>8)&0xFF));
    outp(dma_page[DmaNum],aligned_physical[DmaBuf]);
    outp(dma_mask[DmaNum],DmaCha);
    if(judascfg_device == DEV_SB16) {
//    dspwrite(0xB8); // 16 bit single cycle input
//    dspwrite(0xC8); //  8 bit single cycle input
//    dspwrite(0x10);
      dspwrite(0x24);
    } else {
      dspwrite(0x24);
    }
    dspwrite((unsigned char)(len&0xFF));
    dspwrite((unsigned char)((len>>8)&0xFF));
}
//------------------------------------------------------------------------------
unsigned short dmacount ( void )
{
  unsigned short x;

  x=inp(dma_cnt[DmaNum]);
  x|=inp(dma_cnt[DmaNum])<<8;
  return(x);
}
//------------------------------------------------------------------------------
unsigned short dmastatus ( void )
{
  if(DmaNum<4) return(inp(0x08)&(1<<DmaCha));
  else         return(inp(0xD0)&(1<<DmaCha));
}
//------------------------------------------------------------------------------
// Copyright (c), David Welch, 1993
//------------------------------------------------------------------------------

