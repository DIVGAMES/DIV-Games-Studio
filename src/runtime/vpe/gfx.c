#include "vpe.h"
#include "gfx.h"
#include <SDL/SDL.h>

VPEDword ScrBase;
int ScrWidth, ScrHeight;
int GfxOn=FALSE;
void retrazo(void);

/****************************************************************************
Func:	void InitGraph(void)
Info:	Initializes graphics system.
*****************************************************************************/

void InitGraph(char *buffer,int ancho,int alto)
{
  GfxOn=TRUE;
  ScrBase=(VPEDword)buffer;
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
Func:	void SetPalette(VPEByte *pal_ptr)
Params:	pal_ptr - pointer to the pal data
Info:	Sets current palette to pal_ptr. It's called from inside of VPE.
*****************************************************************************/

extern SDL_Surface *vga;

void SetPalette(VPEByte *pal_ptr)
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

	VPEShort i;

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
void draw_buffer(VPEDword src, VPEDword dest, VPEDword w, VPEDword h, VPEDword delta);

void draw_buffer(VPEDword src, VPEDword dest, VPEDword w, VPEDword h, VPEDword delta) {

//printf("draw_buffer\n");

//memcpy(dest,src,w*h);

}


void DrawBuffer(struct View *v)
{
	VPEDword Dest;

	Dest=ScrBase+v->ScrX+v->ScrY*ScrWidth;
	draw_buffer((VPEDword)v->Buffer,Dest,(VPEDword)v->Width>>2,(VPEDword)v->Height,
		(VPEDword)(ScrWidth-v->Width));
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
void ScrDraw(VPEByte *src)
{
	int i,j;
	VPEByte *dest;

	for(i=0;i<ScrWidth;i++) {
		dest=(VPEByte *)ScrBase+i;
		for(j=0;j<ScrHeight;j++) {
			*dest=*src;
			dest+=ScrWidth;
			src++;
		}
	}
}

void ScrClear(void)
{
	memset((VPEByte *)ScrBase,0,ScrWidth*ScrHeight);
}

