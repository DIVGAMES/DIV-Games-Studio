#include "vpe.h"
#include "gfx.h"
#include <SDL.h>

DWORD ScrBase;
int ScrWidth, ScrHeight;
int GfxOn=FALSE;

/****************************************************************************
Func:	void InitGraph(void)
Info:	Initializes graphics system.
*****************************************************************************/

void InitGraph(char *buffer,int ancho,int alto)
{
  GfxOn=TRUE;
  ScrBase=(DWORD)buffer;
  ScrWidth=ancho;
  ScrHeight=alto;
}

/****************************************************************************
Func:	void ShutGraph(void)
Info:	Shuts down the gfx system
*****************************************************************************/
void ShutGraph(void)
{
  GfxOn=FALSE;
}

/****************************************************************************
Func:	int GetGraphWidth(void)
Info:	Returns width of the output device
*****************************************************************************/
int GetGraphWidth(void)
{
	return(ScrWidth);
}

/****************************************************************************
Func:	int GetGraphHeight(void)
Info:	Returns height of the output device
*****************************************************************************/
int GetGraphHeight(void)
{
	return(ScrHeight);
}

/****************************************************************************
Func:	void SetPalette(BYTE *pal_ptr)
Params:	pal_ptr - pointer to the pal data
Info:	Sets current palette to pal_ptr. It's called from inside of VPE.
*****************************************************************************/

extern SDL_Surface *vga;

void SetPalette(BYTE *pal_ptr)
{
#ifndef DOS
       SDL_Color colors[256];
       int i;
       int b=0;
       for(i=0;i<256;i++){
          colors[i].r=pal_ptr[b]*4;
          colors[i].g=pal_ptr[b+1]*4;
          colors[i].b=pal_ptr[b+2]*4;
          b+=3;
    }
       if(!SDL_SetPalette(vga, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256)) 
               printf("Failed to set palette :(\n"); 
       
       retrazo();
#else

	SHORT i;

	for(i=0;i<256;i++) {
		outp(0x3c8,i);
		outp(0x3c9,*pal_ptr++);
		outp(0x3c9,*pal_ptr++);
		outp(0x3c9,*pal_ptr++);
	}

#endif

}

/****************************************************************************
Func:	void DrawBuffer(struct View *V)
Params:	V - pointer to view to blit
Info:	Blits view buffer to the output device. Called from inside VPE
*****************************************************************************/
void draw_buffer(DWORD src, DWORD dest, DWORD w, DWORD h, DWORD delta);

void draw_buffer(DWORD src, DWORD dest, DWORD w, DWORD h, DWORD delta) {

//printf("draw_buffer\n");

memcpy(dest,src,w*h);

}


void DrawBuffer(struct View *v)
{
	DWORD Dest;

	Dest=ScrBase+v->ScrX+v->ScrY*ScrWidth;
	draw_buffer((DWORD)v->Buffer,Dest,(DWORD)v->Width>>2,(DWORD)v->Height,
		(DWORD)(ScrWidth-v->Width));
}

/****************************************************************************
 Internal function for Blitter
*****************************************************************************/
#pragma aux draw_buffer =		\
	"cld"				\
"scan_loop:"				\
	"mov ecx, edx"			\
	"rep movsd"			\
	"add edi, eax"			\
	"dec ebx"			\
	"jnz scan_loop"			\
	parm [esi] [edi] [edx] [ebx] [eax];

/****************************************************************************
 Internal functions for Filler
*****************************************************************************/
void ScrDraw(BYTE *src)
{
	int i,j;
	BYTE *dest;

	for(i=0;i<ScrWidth;i++) {
		dest=(BYTE *)ScrBase+i;
		for(j=0;j<ScrHeight;j++) {
			*dest=*src;
			dest+=ScrWidth;
			src++;
		}
	}
}

void ScrClear(void)
{
	memset((BYTE *)ScrBase,0,ScrWidth*ScrHeight);
}

