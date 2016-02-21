#include "internal.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * struct FLine {
  BYTE  *RawPtr, *PalPtr, *PixPtr;
  DWORD  Coord, Delta;
  SHORT  LeftCol, Count, Width2;
  struct Region *pRegion;
}; 
  */


#define FLOOP_LEN    32
#define FLOOP_SIZE   19


int16_t FLoopOffSet[]={
    0x0260,0x024D,0x023A,0x0227,0x0214,0x0201,0x01EE,0x01DB,
    0x01C8,0x01B5,0x01A2,0x018F,0x017C,0x0169,0x0156,0x0143,
    0x0130,0x011D,0x010A,0x00F7,0x00E4,0x00D1,0x00BE,0x00AB,
    0x0098,0x0085,0x0072,0x005F,0x004C,0x0039,0x0026,0x0013
 };
    
void _DrawFSpan(struct FLine *fline);

void DrawFSpan(struct FLine *fline) {
//	printf("%s\n",__FUNCTION__);
	__DrawFSpan(fline);
}

void __DrawFSpan(struct FLine *fline)   // Draw hor tex line
{
//	push ebp			; preserve caller's stack frame
//   mov ebp,esp			; point to our stack frame
//	pushad				; put all registers to stack
	int esi=0;
	BYTE *pix = fline->PixPtr;
	BYTE *src = fline->RawPtr;
	int i=0;

	int ebx = 0;				//	mov ebx, [ebp+8]		; pointer to HLine structure
	int eax = fline->Count;		//	movzx eax, word [ebx+Count]	; EAX=pixels_to_draw
	int LoopCount = eax;		//	mov [LoopCount], eax		; LoopCount=EAX
	eax = eax%(FLOOP_LEN-1);	//	and eax, FLOOP_LEN-1		; EAX=pixels_to_draw%LOOP_LEN
	esi = FLoopOffSet;			//	mov esi, FLoopOffset		; Address of the offset table
	eax = esi+eax*4;;			//	mov eax, [esi+eax*4]		; EAX=relative offset of the cell
//	eax+= FLoopStart;			//	add eax, .FLoopStart		; EAX=absolute address of start cell

for(i=0;i<fline->Count;i++)
	pix[i]=src[i];//FLoopOffSet[i%sizeof(FLoopOffSet)]];//rnd()*255;
	
//printf("EAX: %x %x\n",eax,ebx);
/*
 	push eax

	mov cl, [ebx+Width2]		; get log 2 of tex width
	movzx eax, word [ebx+LeftCol] ; put cur col to eax
	mov ebp, [ebx+Delta]		; get delta's
	mul ebp				; make cur coords
	mov edx, [ebx+Coord]		; get start coords
	add edx, eax;			; edx contains full coords
	mov esi, [ebx+RawPtr]		; Tex pointer
	mov edi, [ebx+PixPtr]		; Buffer pointer
	mov ebx, [ebx+PalPtr]		; Pal buffer pointer
*/
	return;						//	ret				; Jump to the start loop cell


}

void DrawWSpan(struct WLine *wline)   // Draw ver tex line
{
		printf("%s\n",__FUNCTION__);
	wline->PixPtr=rnd()*255;
	
	printf("Drawing WSpan: %x %x %x\n",wline->RawPtr,wline->PalPtr, wline->PixPtr);
	 
}
void DrawMaskWSpan(struct WLine *wline) // Draw masked ver tex line (wall)
{
		printf("%s\n",__FUNCTION__);
	wline->PixPtr=rnd()*255;
	
	printf("Drawing MaskWSpan: %x %x %x\n",wline->RawPtr,wline->PalPtr, wline->PixPtr);
}

void DrawTransWSpan(struct WLine *wline)  // Draw translucent ver tex line (wall)
{
		printf("%s\n",__FUNCTION__);
	memset(wline->PixPtr,rnd()*255,wline->Count);
	
	printf("%s\n",__FUNCTION__);
}


#define OLOOP_LEN    32
#define OLOOP_SIZE   12
#define OMLOOP_LEN   32
#define OMLOOP_SIZE  16
#define OTLOOP_LEN   32
#define OTLOOP_SIZE  15

void DrawOSpan(struct WLine *wline)    // Draw ver tex line (obj)
{
	uint16_t *coords=&wline->Coord;
	
	uint16_t *delta = &wline->Delta;
	BYTE col = 255;
	int i=0;
	BYTE *pix = (BYTE *)wline->PixPtr;
	BYTE *src = (BYTE *)wline->RawPtr;
	int esi=0;
	int ecx=0;
		
	int eax = wline->Count;
	int LoopCount = eax;

//	int esi = OloopOffset;

	
	printf("eax: %d ",eax);

	eax = eax & OLOOP_LEN-1;


	
	printf("eax: %d ",eax);
	
	eax=esi+(eax*4);
	
	printf("eax: %d\n",eax);
	
	
	
	esi = wline->RawPtr;//		; raw ptr in esi
	
	
	
//	push ebp			; preserve caller's stack frame
 //   mov ebp,esp			; point to our stack frame
//	pushad				; save all registers
	
	/*
	
	mov ebx, [ebp+8]

	movzx eax, word [ebx+Count]
	mov [LoopCount], eax
	and eax, OLOOP_LEN-1
	mov esi, OLoopOffset
	mov eax, [esi+eax*4]
	add eax, .OLoopStart
	push eax

	mov esi, [ebx+RawPtr]		; raw ptr in esi

	mov eax, [ebx+Coord-2]		; frac Coord in hi eax
	xor ax, ax

	mov ecx, [ebx+Delta-2]		; frac Delta in hi ecx
	xor cx, cx			; clear lower ecx

	xor edx, edx			; int Coord in low edx
	mov dx, [ebx+Coord+2]
	add esi, edx			; Advance raw ptr

	xor edx, edx			; int Delta in low edx
	mov dx, [ebx+Delta+2]

	movzx ebp, word [ebx+ViewWidth]

	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]
	* 
	* 
	

	ret
	* *.
	*/
	
	ecx = delta[0]; 
	
	for(i=0;i<wline->Count;i++) {
		pix[i*wline->BufWidth]=src[i+coords[0]];
		
//		col = (BYTE *)wline->RawPtr[i];
//wline->PixPtr=rnd()*255;
	//	pix[i*wline->BufWidth]=col;//(BYTE *)wline->PalPtr[((int)wline->PixPtr[i%wline->BufWidth])%255];
	}
		printf("DrawOSpan c1 %f c2 %f Delta %f  %x %x %d\n",coords[0],coords[1],wline->Delta,wline->RawPtr, wline->PixPtr,wline->Count);

//	memcpy(wline->PixPtr, wline->PalPtr, wline->Count);
//	memset(wline->PixPtr,rnd()*255,wline->Count);
	
//	printf("%s\n",__FUNCTION__);
}

void DrawMaskOSpan(struct WLine *wline) // Draw masked ver tex line (obj)
{
	wline->PixPtr=rnd()*255;
	
	printf("%s\n",__FUNCTION__);
}

void DrawTransOSpan(struct WLine *wline)  // Draw translucent ver tex line (obj)
{
	wline->PixPtr=rnd()*255;
	
	printf("%s\n",__FUNCTION__);
}



extern DWORD ScrBase;

